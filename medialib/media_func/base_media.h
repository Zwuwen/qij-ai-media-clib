#ifndef __BASE_MEDIA_H__
#define __BASE_MEDIA_H__
#include "common.h"
#include "init_data.h"
#include "media_param.h"
#include "decode_temp.h"
#include "cpu_decode.h"
#include "vpu_decode.h"

class cbase_media
{
public:
    cbase_media();
    ~cbase_media();
public:
    /*对外接口*/
    UINT32 init_resource(vector<media_conf_t>& media_conf_list);
    UINT32 start_pull_media_task(std::string media_flow_id);
    UINT32 start_all_pull_media_task();
    void stop_pull_media_task(std::string media_flow_id);
    void stop_all_pull_media_task();
    void reset_sample_resource(media_conf_t& media_conf);
    void destory_resource();
    UINT32 get_media_by_id(char* media_id,char media_type,char* data,int size);
    /*内部使用函数*/
    static void pull_flow_thread(void* pthis,ffmpeg_pull_flow_param_t& pull_flow_param,media_conf_t& conf);
    static void pull_flow_thread_for_decode(void* pthis,ffmpeg_pull_flow_param_t& pull_flow_param,media_conf_t& conf,cbase_decode* decode);
    void clean_pull_resource(std::string media_flow_id);
    void clean_all_pull_resource();
    void clean(ffmpeg_pull_flow_param_t& pull_param);
private:
    //拉流参数
    std::vector<ffmpeg_pull_flow_param_t> m_pull_flow_param_list;
    //流配置信息
    std::vector<media_conf_t> m_media_conf_list;
public:
    std::map<std::string,std::string> m_redis_info;
};

#endif