#ifndef __MEDIA_PARAM_H__
#define __MEDIA_PARAM_H__
#include "common.h"
#include "jpg.h"
#include "base_decode.h"

#ifdef VPUMEDIA
#define MPI_DEC_STREAM_SIZE         (SZ_4K)
#define MPI_DEC_LOOP_COUNT          4
#define MAX_FILE_NAME_LENGTH        256
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE  5 * 1024 * 1024 //8000000    
typedef struct MpiDecLoopData{
    MpiDecLoopData()
    {
        this->mpi = NULL;
        buf = nullptr;
        fp_input = nullptr;
        fp_output = nullptr;
        frame = NULL;
        frm_grp = NULL;
        packet = NULL;
    }
    MppCtx          ctx{};
    MppApi          *mpi{};

    volatile RK_U32 loop_end{};

    /* buffer for stream data reading */
    char            *buf;

    /* input and output */
    MppBufferGroup  frm_grp{};
    MppPacket       packet{};
    size_t          packet_size{};
    MppFrame        frame{};

    FILE            *fp_input;
    FILE            *fp_output;
    RK_U64          frame_count{};
    size_t          max_usage{};
} MpiDecLoopData;
typedef struct rk1808_flow_param
{
    rk1808_flow_param()
    {
        this->m_buffer = nullptr;
        this->m_packet    = NULL;
        this->m_frame     = NULL;
        this->m_ctx          = NULL;
        this->m_mpi         = NULL;
        this->m_param      = NULL;
        this->m_need_split   = 1;
        this->m_mpi_cmd = MPP_CMD_BASE;
        this->m_timeout = 5000;
    }
    char* m_buffer;
    MppPacket m_packet{};
    MppFrame  m_frame{};
    // base flow context
    MppCtx m_ctx{};
    MppApi* m_mpi{};
    MppParam m_param{};
    RK_U32 m_need_split{};
    MpiCmd m_mpi_cmd{};
    MppPollType m_timeout{};
    MpiDecLoopData m_data;
}rk1808_flow_param_t;
#endif

//ffmpeg 拉流参数数据结构
typedef struct ffmpeg_pull_flow_param
{
     //ffmpeg
    ffmpeg_pull_flow_param()
    {
        this->m_id = "";        //拉流参数ID
        this->m_is_running = false;
        this->m_thread = nullptr;
        this->m_video_index = -1;
        this->m_avcodec =NULL;
        this->m_avcodectxt =NULL;
        this->m_frame=NULL;
        this->m_sw_frame=NULL;
        this->m_aviotxt=NULL;
        this->m_input_ctx=NULL;
        this->m_opts=NULL;
        this->m_avcodec_params = NULL;
        this->m_img_conver_ctx=nullptr;
        //this->m_decode = NULL;
    }
    std::string m_id;
    bool m_is_running;
    std::thread* m_thread;
    int m_video_index;
    const AVCodec* m_avcodec{};
    AVCodecContext* m_avcodectxt{};
    AVCodecParameters* m_avcodec_params{};
    AVFrame* m_frame{};
    AVFrame* m_sw_frame{};
    struct SwsContext* m_img_conver_ctx;
    AVPacket m_pkt{};
    AVIOContext* m_aviotxt{};
    AVFormatContext* m_input_ctx{};
    AVDictionary* m_opts{};
    #ifdef VPUMEDIA
    rk1808_flow_param_t m_rk1808_param;
    #endif
    cjpg m_jpg_module;
    cbase_decode* m_decode{};
}ffmpeg_pull_flow_param_t,*pffmpeg_pull_flow_param_t;

#endif // __MEDIA_PARAM_H__