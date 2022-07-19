#include "Loggers.h"
#include "vpu_decode.h"
#include "yuv.h"
#include "init_data.h"
#ifdef VPUMEDIA

std::timed_mutex cvpu_decode::vpu_lock;

cvpu_decode::cvpu_decode() {
    this->m_yuv420p = new UINT8[1920 * 1080 * 3];
}

cvpu_decode::~cvpu_decode() {
    if (m_yuv420p != nullptr) {
        delete[] this->m_yuv420p;
        this->m_yuv420p = nullptr;
    }
}

void cvpu_decode::init(void *param) {
    m_ffmpeg_param = (ffmpeg_pull_flow_param_t *) param;
    m_param = &m_ffmpeg_param->m_rk1808_param;
    m_decode_func_thread = nullptr;
    is_run_ = false;
    m_pkt_done = false;

    m_shared_cache = new cshared_sample_cache(12441626);
}

void cvpu_decode::deinit() {
    if (m_decode_func_thread != nullptr) {
        if (is_run_) {
            is_run_ = false;
            m_decode_func_thread->join();
            delete m_decode_func_thread;
            m_decode_func_thread = nullptr;
        }
    }
    if (m_shared_cache != nullptr) {
        delete m_shared_cache;
        m_shared_cache = nullptr;
    }
    if (m_param == nullptr) {
        return;
    }
    if (m_param->m_mpi->reset(m_param->m_ctx) != MPP_OK) {
        cmylog::mylog("ERR", "mpp reset failed\n");
        return;
    }
    if (m_param->m_packet) {
        mpp_packet_deinit(&m_param->m_packet);
        m_param->m_packet = NULL;
    }

    if (m_param->m_frame) {
        mpp_frame_deinit(&m_param->m_frame);
        m_param->m_frame = NULL;
    }

    if (m_param->m_ctx) {
        mpp_destroy(m_param->m_ctx);
        m_param->m_ctx = NULL;
    }

    if (m_param->m_buffer) {
        mpp_free(m_param->m_buffer);
        m_param->m_buffer = NULL;
    }

    if (m_param->m_data.frm_grp) {
        mpp_buffer_group_put(m_param->m_data.frm_grp);
        m_param->m_data.frm_grp = NULL;
    }
}

UINT32 cvpu_decode::data() {
    //return 0;
    if (m_ffmpeg_param->m_pkt.data == NULL || m_ffmpeg_param->m_pkt.size <= 0) {
        //printf("data invalid\n");
        return QJ_BOX_OP_CODE_DATAINVALID;
    }
    if (m_pkt_done) {
        return QJ_BOX_OP_CODE_SUCESS;
    }
    if ((unsigned )(m_param->m_data.packet_size) < (unsigned )m_ffmpeg_param->m_pkt.size) {
        cmylog::mylog("INFO", "decode size not enough\n");
        return QJ_BOX_OP_CODE_SUCESS;
    }
//    I frame
    if (m_ffmpeg_param->m_pkt.flags != 1) {
        return QJ_BOX_OP_CODE_SUCESS;
    }

    //缓存拷贝
    MppPacket packet = m_param->m_data.packet;
    char *buf = m_param->m_data.buf;
    memcpy(buf, m_ffmpeg_param->m_pkt.data, m_ffmpeg_param->m_pkt.size);
    unsigned int size = m_ffmpeg_param->m_pkt.size;
    mpp_packet_write(packet, 0, buf, size);
    //mpp_packet_set_data(packet, buf);
    mpp_packet_set_size(packet, size);
    mpp_packet_set_pos(packet, buf);
    mpp_packet_set_length(packet, size);
    mpp_packet_set_dts(packet, m_ffmpeg_param->m_pkt.dts);
    this->m_cv.notify_all();

    return QJ_BOX_OP_CODE_SUCESS;
}

void cvpu_decode::decode_func(void *in_this) {
    rk1808_flow_param_t *param = ((cvpu_decode *) in_this)->m_param;
    bool *running = &((cvpu_decode *) in_this)->is_run_;
    auto *vpu_decode = (cvpu_decode *) in_this;
    decode_data_st_t *decode_info = vpu_decode->m_decode_info;
    while (*running) {
        //等待媒体数据
        if (!((cvpu_decode *) in_this)->m_cv.wait_for(2)) {
            continue;
        }
        if (vpu_decode->m_pkt_done) {
            continue;
        }
//#pragma mark -新增
        /*这里等待获取全局的vpu锁*/
        std::unique_lock<std::timed_mutex> lck(cvpu_decode::vpu_lock, std::defer_lock);
        if (!lck.try_lock_for(std::chrono::milliseconds(2000))) continue;
        //获取解码的帧数据
        ((cvpu_decode *) in_this)->m_pkt_done = true;
        MPP_RET ret = param->m_mpi->decode_put_packet(param->m_ctx, param->m_packet);
        int times = 30;
        while (times > 0) {
            ret = param->m_mpi->decode_get_frame(param->m_ctx, &param->m_frame);
            if (ret == MPP_OK) {
                cmylog::mylog("INFO","decode complete\n");
                break;
            }
            if (MPP_ERR_TIMEOUT == ret) {
                times--;
                cmylog::mylog("INFO", "decode timeout,times=%d\n", times);
                msleep(1);
                continue;
            }
            if (ret) {
                break;
            }
        }
        if (ret != MPP_OK) {
            ((cvpu_decode *) in_this)->m_pkt_done = false;
            msleep(1);
            SPDLOG_ERROR("mpp decode failed");
            continue;
        }
        if (param->m_frame) {
            if (mpp_frame_get_info_change(param->m_frame)) {

//                SPDLOG_INFO("info change");
                RK_U32 width = mpp_frame_get_width(param->m_frame);
                RK_U32 heigth = mpp_frame_get_height(param->m_frame);
                RK_U32 hor_stride = mpp_frame_get_hor_stride(param->m_frame);
                RK_U32 ver_stride = mpp_frame_get_ver_stride(param->m_frame);
                RK_U32 buf_size = mpp_frame_get_buf_size(param->m_frame);
                if(width && heigth && hor_stride && ver_stride){};
                if (param->m_data.frm_grp == NULL) {
                    if (mpp_buffer_group_get_internal(&param->m_data.frm_grp, MPP_BUFFER_TYPE_ION)) {
                        cmylog::mylog("ERR", "get mpp buffer group failed \n");
                        goto RESULT;
                    }
                    /* Set buffer to mpp decoder */
                    if (param->m_mpi->control(param->m_ctx, MPP_DEC_SET_EXT_BUF_GROUP, param->m_data.frm_grp)) {
                        cmylog::mylog("ERR", "set buffer group failed \n");
                        goto RESULT;
                    }
                } else {
                    if (mpp_buffer_group_clear(param->m_data.frm_grp)) {
                        cmylog::mylog("ERR", "clear buffer group failed \n");
                        goto RESULT;
                    }
                }
                if (mpp_buffer_group_limit_config(param->m_data.frm_grp, buf_size, 24)) {
                    cmylog::mylog("ERR", " limit buffer group failed \n");
                    goto RESULT;
                }
                if (param->m_mpi->control(param->m_ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL)) {
                    cmylog::mylog("ERR", "info change ready failed  \n");
                    goto RESULT;
                }
            } else {
//                SPDLOG_INFO("mpp get buffer");
                MppBuffer buffer = mpp_frame_get_buffer(param->m_frame);
                RK_U32 buf_size = mpp_frame_get_buf_size(param->m_frame);
                RK_U32 width = mpp_frame_get_width(param->m_frame);
                RK_U32 height = mpp_frame_get_height(param->m_frame);
                RK_U32 hor_stride = mpp_frame_get_hor_stride(param->m_frame);
                RK_U32 ver_stride = mpp_frame_get_ver_stride(param->m_frame);
                RK_U8 *base = (RK_U8 *) mpp_buffer_get_ptr(buffer);
                MppFrameFormat fmt = mpp_frame_get_fmt(param->m_frame);
                RK_S64 dts = mpp_frame_get_dts(param->m_frame);
                if(fmt){};
                UINT32 yuv420p_size = cyuv::yuv420sp2yuv420p(base, buf_size, vpu_decode->m_yuv420p,
                                                             sizeof(vpu_decode->m_yuv420p),
                                                             width, height, hor_stride, ver_stride);
                memset(decode_info, 0x00, sizeof(decode_data_st_t));
                std::cout<<"DTS:"<<dts<<std::endl;
                cmylog::mylog("INFO","vpu_decode->m_shared_cache_type = %d\n", vpu_decode->m_shared_cache_type);
                if (vpu_decode->m_shared_cache_type & DECODE_DATA_JPG) {
                    cjpg jpg;
                    jpg.init_conv_param(width, height);
                    jpg.yuv2jpg(vpu_decode->m_yuv420p);
                    memcpy(decode_info->m_jpg_buf, (char *) jpg.m_compress_buff, jpg.m_compress_buff_size);
                    decode_info->m_jpg_buf_len = jpg.m_compress_buff_size;
#if 0   //  debug
                    //put jpeg to buffer
                    char path[1024] = {0};
                    UINT32 rand_num = (UINT32)rand();
                    sprintf(path,"./jpg/%d.jpg",rand_num);
                    printf("###########write1 jpeg :%s###########\n",path);
                     FILE* fp = fopen(path,"wb");
                     fwrite(jpg.m_compress_buff,jpg.m_compress_buff_size,1,fp);
                     fclose(fp);
#endif
                    jpg.release_jpeg_mem();
//                    std::cout << "jpg data len:" << decode_info->m_jpg_buf_len << std::endl;
                }
                if (vpu_decode->m_shared_cache_type & DECODE_DATA_YUV420P) {
                    memcpy(decode_info->m_yuv_buf, vpu_decode->m_yuv420p, yuv420p_size);
                    decode_info->m_yuv_buf_len = yuv420p_size;
//                    std::cout << "yuv data len:" << decode_info->m_yuv_buf_len << std::endl;
                }

                decode_info->m_width = width;
                decode_info->m_height = height;
                decode_info->dts=dts;
                if (QJ_BOX_OP_CODE_SUCESS !=
                    vpu_decode->m_shared_cache->put((UINT8 *) decode_info, sizeof(decode_data_st_t))) {
                    cmylog::mylog("ERR", "put data failed  \n");
                }
            }
            RESULT:
            ((cvpu_decode *) in_this)->m_pkt_done = false;
            mpp_frame_deinit(&param->m_frame);
            if (param->m_frame != NULL) {
                free(param->m_frame);
            }
            param->m_frame = NULL;
        } else {
            ((cvpu_decode *) in_this)->m_pkt_done = false;
            SPDLOG_ERROR("mpp decode failed ,frame is null, maake sure the rtsp video stream is not h265 and reachable!!! ");
        }
    }
    if (param->m_frame) {
        mpp_frame_deinit(&param->m_frame);
        param->m_frame = NULL;
    }

}

UINT32 cvpu_decode::ctrl(std::string data) {
    size_t packet_size = DUMMY_SINK_RECEIVE_BUFFER_SIZE;//DUMMY_SINK_RECEIVE_BUFFER_SIZE;//MPI_DEC_STREAM_SIZE
    m_param->m_buffer = mpp_malloc(char, packet_size);
    if (nullptr == m_param->m_buffer) {
        cmylog::mylog("ERR", "npu malloc resource failure\n");
        return QJ_BOX_OP_CODE_NOENOUGHSPACE;
    }
    cmylog::mylog("INFO", "npu malloc resource complete\n");
    if (mpp_packet_init(&m_param->m_packet, m_param->m_buffer, packet_size)) {
        cmylog::mylog("ERR", "mpp packet init failure\n");
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    cmylog::mylog("INFO", "mpp packet init complete\n");
    if (mpp_create(&m_param->m_ctx, &m_param->m_mpi) != MPP_OK) {
        cmylog::mylog("ERR", "mpp create failure\n");
        return QJ_BOX_OP_CODE_CREATEFAILURE;
    }
    cmylog::mylog("INFO", "mpp create success\n");
    m_param->m_mpi_cmd = MPP_DEC_SET_PARSER_SPLIT_MODE;
    RK_U32 need_split = -1;
    m_param->m_param = &need_split;
    if (m_param->m_mpi->control(m_param->m_ctx, m_param->m_mpi_cmd, m_param->m_param) != MPP_OK) {
        cmylog::mylog("ERR", "mpp ioctrl cmd=MPP_DEC_SET_PARSER_SPLIT_MODE failure\n");
        return QJ_BOX_OP_CODE_ACCESSFAILURE;
    }
    cmylog::mylog("INFO", "mpp ioctrl cmd=MPP_DEC_SET_PARSER_SPLIT_MODE success\n");
    /*
     m_param->m_param = &m_param->m_timeout;
     if(m_param->m_mpi->control(m_param->m_ctx, MPP_SET_OUTPUT_TIMEOUT, m_param->m_param) != MPP_OK)
     {
          cmylog::mylog("ERR","mpp ioctrl cmd=output timeout failure\n");
         return QJ_BOX_OP_CODE_ACCESSFAILURE;
     }
     cmylog::mylog("INFO","mpp ioctrl cmd=output timeout success\n");
     */

    if (mpp_init(m_param->m_ctx, MPP_CTX_DEC, MPP_VIDEO_CodingAVC) != MPP_OK) {
        cmylog::mylog("ERR", "mpp mpp_init failure\n");
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
//    if (mpp_init(m_param->m_ctx, MPP_CTX_DEC, MPP_VIDEO_CodingHEVC) != MPP_OK) {
//        cmylog::mylog("ERR", "mpp mpp_init failure\n");
//        return QJ_BOX_OP_CODE_UNKOWNERR;
//    }
    cmylog::mylog("INFO", "mpp mpp_init success\n");
    m_param->m_data.ctx = m_param->m_ctx;
    m_param->m_data.mpi = m_param->m_mpi;
    //m_param->m_data.eos            = 0;
    m_param->m_data.buf = m_param->m_buffer;
    m_param->m_data.packet = m_param->m_packet;
    m_param->m_data.packet_size = packet_size;
    m_param->m_data.frame = m_param->m_frame;
    m_param->m_data.frame_count = 0;
    //m_param->m_data.frame_num = 0;

    this->is_run_ = true;
    m_decode_func_thread = new std::thread([this]()mutable {
        cvpu_decode::decode_func(this);
    });
    assert(m_decode_func_thread != NULL);

    return QJ_BOX_OP_CODE_SUCESS;
}

//#pragma mark -新增

bool cvpu_decode::is_run() {
    return is_run_;
}

#endif