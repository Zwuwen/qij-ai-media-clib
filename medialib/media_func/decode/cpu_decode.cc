#include "cpu_decode.h"

ccpu_decode::ccpu_decode()
{
    
}
ccpu_decode::~ccpu_decode()
{
    
}

void ccpu_decode::init(void* param)
{
    this->m_param = (ffmpeg_pull_flow_param_t*)param;
    m_shared_cache = new cshared_sample_cache(12441626);
}
void ccpu_decode::deinit()
{
     if(m_shared_cache != NULL)
    {
        delete m_shared_cache;
        m_shared_cache = NULL;
    }
}

UINT32 ccpu_decode::data()
{
    if(m_param->m_avcodectxt == NULL)
    {
        cmylog::mylog("ERR","pktm_avcodectxt is null ,id=%s\n",m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_DATAINVALID;
    }
 //解码过程
    int send_pkt = avcodec_send_packet(m_param->m_avcodectxt, &m_param->m_pkt);
    if(send_pkt !=0)
    {
        cmylog::mylog("ERR","avcodec send packet error,send packet=%d,url=%s\n",send_pkt,m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    int recv_frame = avcodec_receive_frame(m_param->m_avcodectxt,m_param->m_frame);
    if(recv_frame !=0)
    {
        cmylog::mylog("ERR","avcodec receive frame error,recv frame=%d,url=%s\n",recv_frame,m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    //格式转换
    int retvalue = sws_scale(m_param->m_img_conver_ctx,
                 (const uint8_t *const *) m_param->m_frame->data, 
                m_param->m_frame->linesize,
                0, m_param->m_avcodectxt->height,
                m_param->m_sw_frame->data, m_param->m_sw_frame->linesize);
    decode_data_st_t* decode_info = m_decode_info;
    memset(decode_info,0x00,sizeof(decode_data_st_t));
    if(m_param->m_avcodectxt->width <= 0)
    {
        return QJ_BOX_OP_CODE_SUCESS;
    }
    decode_info->m_width = m_param->m_avcodectxt->width;
    decode_info->m_height = m_param->m_avcodectxt->height;
    if(m_shared_cache_type  & DECODE_DATA_YUV420P)
    {
        memcpy(decode_info->m_yuv_buf,m_param->m_sw_frame->data[0],decode_info->m_width*decode_info->m_height); 
        decode_info->m_yuv_buf_len = decode_info->m_width*decode_info->m_height;
    }

     if(m_shared_cache_type  & DECODE_DATA_JPG)
    {
        cjpg jpg;
        jpg.init_conv_param(decode_info->m_width,decode_info->m_height);
        if(jpg.yuv2jpg(m_param->m_sw_frame->data[0]) == false)
        {
            return QJ_BOX_OP_CODE_SUCESS;
        }
        memcpy(decode_info->m_jpg_buf,(char*)jpg.m_compress_buff,jpg.m_compress_buff_size);
        decode_info->m_jpg_buf_len = jpg.m_compress_buff_size;
        //cmylog::mylog("INFO","jpg data \n");
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
    }
    if(QJ_BOX_OP_CODE_SUCESS != m_shared_cache->put((UINT8*)decode_info,sizeof(decode_data_st_t)))
    {
        cmylog::mylog("ERR","put data failed  \n");
    }
    return QJ_BOX_OP_CODE_SUCESS;
}

UINT32 ccpu_decode::ctrl(std::string data)
{
    //查找解码器,获取video 索引
    int video_index = atoi((const char*)data.c_str());
    
    m_param->m_avcodec_params = m_param->m_input_ctx->streams[video_index]->codecpar;
    m_param->m_avcodec = avcodec_find_decoder(m_param->m_avcodec_params->codec_id);
    if(m_param->m_avcodec == NULL)
    {
        cmylog::mylog("ERR","could not find codec,url=%s\n",m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    cmylog::mylog("INFO","find codec success,url=%s\n",m_param->m_id.c_str());
    //指定codec初始化avcodec context
    m_param->m_avcodectxt = avcodec_alloc_context3(m_param->m_avcodec);
    if(!m_param->m_avcodectxt)
    {
        cmylog::mylog("ERR","could not allocate video codec context,url=%s\n",m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    avcodec_parameters_to_context(m_param->m_avcodectxt, m_param->m_avcodec_params);

    int retvalue = avcodec_open2(m_param->m_avcodectxt,m_param->m_avcodec,NULL);
    if(retvalue < 0)
    {
        cmylog::mylog("ERR","avcodec open2 result=%d,url=%s\n",retvalue,m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    m_param->m_frame = av_frame_alloc();
    m_param->m_sw_frame = av_frame_alloc();
    av_image_alloc(m_param->m_sw_frame->data,m_param->m_sw_frame->linesize, 
                                    m_param->m_avcodectxt->width,m_param->m_avcodectxt->height,AV_PIX_FMT_YUV420P ,1);
    if(m_param->m_sw_frame == NULL)
    {
        cmylog::mylog("ERR","alloc frame failure,url=%s\n",m_param->m_id.c_str());
        return QJ_BOX_OP_CODE_UNKOWNERR;
    }
    // 由于解码出来的帧格式不一定是YUV420P的,在转jpg之前需要进行格式转换
    m_param->m_img_conver_ctx = sws_getContext(m_param->m_avcodectxt->width, m_param->m_avcodectxt->height,
                                                        m_param->m_avcodectxt->pix_fmt,
                                                        m_param->m_avcodectxt->width, m_param->m_avcodectxt->height,
                                                        AV_PIX_FMT_YUV420P,
                                                        SWS_BICUBIC, NULL, NULL, NULL);

    return QJ_BOX_OP_CODE_SUCESS;
}
