#ifndef __INIT_DATA_H__
#define __INIT_DATA_H__

#include<vector>

using namespace std;

//log information
typedef struct _log_info
{
    bool m_is_del;
    bool m_is_outputstd;
    string m_log_levl; 
    string m_log_path;

    _log_info()
    {
        this->m_is_del = true;
        this->m_is_outputstd = true;
        this->m_log_levl = "";
        this->m_log_path = "";
    }

    struct _log_info& operator=(const struct _log_info& log)
    {
        this->m_is_del = log.m_is_del;
        this->m_is_outputstd = log.m_is_outputstd;
        this->m_log_levl = log.m_log_levl;
        this->m_log_path = log.m_log_path;
        return *this;
    }

}log_info,*plog_info;

//流信息，网络流，本地文件流
#define NET_FLOW_TYPE                           "net_type"
#define LOCAL_FILE_FLOW_TYPE           "local_file_type"
//解码参数
#define CPU_DECODE_TYPE                         "CPU"
#define VPU_DECODE_TYPE                         "VPU"
//解码数据类型
#define DECODE_DATA_YUV420P                            0x01 
#define DECODE_DATA_JPG                                      0x02
#define DECODE_DATA_YUVJPG                              0x0f
typedef struct _media_conf
{
    std::string m_id;
    std::string m_url;
    std::string m_buffer_size;
    std::string m_max_delay;
    std::string m_type;     //net,local file
    std::string m_net_type;
    std::string m_unlink_timeout;
    std::string m_decode_type;      //解码类型
    UINT8 m_decode_data_type;     //jpg,yuv
    _media_conf()
    {
        this->m_id = "";
        this->m_url = "";
        this->m_buffer_size = "204800";
        this->m_max_delay = "500000";
        this->m_type = NET_FLOW_TYPE;
        this->m_net_type = "tcp";
        this->m_unlink_timeout = "150";
        this->m_decode_type = CPU_DECODE_TYPE;
        this->m_decode_data_type = DECODE_DATA_YUVJPG;
    }
    struct _media_conf& operator=(const struct _media_conf& conf)
    {
        this->m_id = conf.m_id; 
        this->m_url = conf.m_url;
        this->m_buffer_size = conf.m_buffer_size;
        this->m_max_delay = conf.m_max_delay;
        this->m_type = conf.m_type;
        this->m_net_type = conf.m_net_type;
        this->m_unlink_timeout = conf.m_unlink_timeout;
        this->m_decode_type = conf.m_decode_type;
        this->m_decode_data_type = conf.m_decode_data_type;
        return * this;
    }
}media_conf_t,*pmedia_conf_t;

typedef struct _decode_data_st
{
    _decode_data_st()
    {
        memset(m_yuv_buf,0x00,sizeof(m_yuv_buf));
        m_yuv_buf_len = 0;
        memset(m_jpg_buf,0x00,sizeof(m_yuv_buf));
        m_jpg_buf_len = 0;
        m_width = 0;
        m_height = 0;
    }
    char m_yuv_buf[1920*1080*3];
    int m_yuv_buf_len; 
    char m_jpg_buf[1920*1080*3];
    int m_jpg_buf_len; 
    int m_width;
    int m_height;
}decode_data_st_t;

#endif // __INIT_DATA_H__