#ifndef __BASE_MEDIA_H__
#define __BASE_MEDIA_H__

#include <map>
#include <list>
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

#pragma mark - 新增
    std::list<media_conf_t>& get_media_conf();
    int add_resource(vector<media_conf_t>& media_conf_list);
    int remove_resource(const string& media_id);

    void destory_resource();
    UINT32 get_media_by_id(const char* media_id,int media_type,uint8_t * data,int size);
    /*内部使用函数*/
    static void pull_flow_thread(void* pthis,ffmpeg_pull_flow_param_t& pull_flow_param,media_conf_t& conf);
    static void pull_flow_thread_for_decode(void* p_this, ffmpeg_pull_flow_param_t& pull_flow_param, media_conf_t& conf, cbase_decode* decode);
    void clean_pull_resource(std::string media_flow_id);
    void clean_all_pull_resource();
    void clean(ffmpeg_pull_flow_param_t& pull_param);
private:
//    //拉流参数
//    std::vector<ffmpeg_pull_flow_param_t> m_pull_flow_param_list;
//    //流配置信息
//    std::vector<media_conf_t> m_media_conf_list;
    //拉流参数
    std::list<ffmpeg_pull_flow_param_t> m_pull_flow_param_list;
    //流配置信息
    std::list<media_conf_t> m_media_conf_list;
//public:
//    std::map<std::string,std::string> m_redis_info;
};

#endif