#include "common.h"
#include "init_data.h"
#include "user_media_api.h"
#include "init_data.h"
#include "base_media.h"
#include "algorithm"

char* version()
{
    return "7g_media_version_1.0.0";
}

MEDIA_HANDLE init_media_qjapi(char* media_conf)
{
    if(media_conf == NULL)
    {
        return NULL;
    }
    printf("conf=%s\n",media_conf);
    cbase_media* base_media = NULL;
    Json::Reader reader;
    Json::Value root;
    try
    {
        if(reader.parse(media_conf,root))
        {
            //log初始化
            const Json::Value log_obj = root["log"];
            std::string log_path = log_obj["logpath"].asString();
            std::string log_level = log_obj["level"].asString();
            bool isdel = log_obj["isdelete"].asBool();
            bool isoutput = log_obj["isoutput"].asBool();
            cmylog::init_log(log_path,log_level,isdel,isoutput);
            cmylog::mylog("INFO","log path=%s\n",log_path.c_str());
            //媒体配置信息
            vector<media_conf_t> media_conf_list;
            const Json::Value media_obj = root["media"];
            for(auto i = 0 ; i < media_obj.size() ; i++)
            {
                media_conf_t conf;
                conf.m_id = media_obj[i]["id"].asString();
                conf.m_url = media_obj[i]["url"].asString();
                conf.m_decode_type = media_obj[i]["decode"].asString();
                conf.m_decode_data_type = media_obj[i]["decode_data"].asInt();
                media_conf_list.push_back(conf);
            }
            cmylog::mylog("INFO","load conf complete\n");
            //媒体资源初始化
            base_media = new cbase_media();
            UINT32 ret = base_media->init_resource(media_conf_list);
            if(ret != QJ_BOX_OP_CODE_SUCESS)
            {
                printf("init resource failure\n");
                base_media->destory_resource();
                if(base_media != NULL)
                {
                    delete base_media;
                    base_media = NULL;
                }
                return NULL;
            }
            cmylog::mylog("INFO","initialization media success\n");
            return base_media;
        }
        else
        {
            printf("data format except\n");
            return NULL;
        }
    }
    catch(...)
    {
        printf("catch except\n");
        if(base_media != NULL)
        {
            delete base_media;
            base_media = NULL;
        }
        return NULL;
    }
}
//回收媒体资源，出入句柄
void deinit_media_qjapi(MEDIA_HANDLE handle)
{
    auto* base_media = (cbase_media*)handle;
    if(base_media == nullptr)
    {
        cmylog::mylog("INFO","input handle is null\n");
        return;
    }
    base_media->destory_resource();
    delete base_media;
    handle = nullptr;
}
//启动所有媒体资源
int start_all_media(MEDIA_HANDLE handle)
{
    cbase_media* base_media = (cbase_media*)handle;
    if(base_media == NULL)
    {
        cmylog::mylog("ERR","input handle is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    cmylog::mylog("INFO","start all media task\n");
    return base_media->start_all_pull_media_task();
}
//启动指定ID的媒体资源
int start_sample_media(MEDIA_HANDLE handle,char* media_id)
{
    cbase_media* base_media = (cbase_media*)handle;
    if(base_media == NULL || media_id == NULL)
    {
        cmylog::mylog("ERR","input handle is null or id is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    std::string id = media_id;
    return base_media->start_pull_media_task(id);
}
//关闭所有媒体资源
int stop_all_media(MEDIA_HANDLE handle)
{
    cbase_media* base_media = (cbase_media*)handle;
    if(base_media == NULL)
    {
        cmylog::mylog("INFO","input handle is null \n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    base_media->stop_all_pull_media_task();
    return QJ_BOX_OP_CODE_SUCESS;
}
//关闭指定ID的媒体资源
int stop_sample_media(MEDIA_HANDLE handle,char* media_id)
{
    cbase_media* base_media = (cbase_media*)handle;
    if(base_media == NULL || media_id == NULL)
    {
        cmylog::mylog("INFO","input handle is null or id is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    std::string id = media_id;
    base_media->stop_pull_media_task(id);
    return QJ_BOX_OP_CODE_SUCESS;
}

int reset_media_conf_by_id(MEDIA_HANDLE handle,char* media_conf)
{
    cbase_media* base_media = (cbase_media*)handle;
    if(base_media == NULL || media_conf == NULL)
    {
        cmylog::mylog("ERR","input handle is null or id is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    //printf("conf=%s\n",media_conf);
    Json::Reader reader;
    Json::Value root;
    vector<media_conf_t> media_conf_list;
    try
    {
        if(reader.parse(media_conf,root))
        {
            const Json::Value media_obj = root["media"];
            for(auto i = 0 ; i < media_obj.size() ; i++)
            {
                media_conf_t conf;
                conf.m_id = media_obj[i]["id"].asString();
                conf.m_url = media_obj[i]["url"].asString();
                conf.m_decode_type = media_obj[i]["decode"].asString();
                conf.m_decode_data_type = media_obj[i]["decode_data"].asInt();
                media_conf_list.push_back(conf);
            }
            for(auto it = media_conf_list.begin();it != media_conf_list.end();++it)
            {
                base_media->reset_sample_resource(*it);
            }
            return QJ_BOX_OP_CODE_SUCESS;
        }
    }
    catch(...)
    {
        cmylog::mylog("ERR","input configure info exception\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    return QJ_BOX_OP_CODE_UNKOWNERR;
}

int get_media_by_id(MEDIA_HANDLE handle,char* media_id,int media_type,decode_data_st_t* decode_data)
{
    if(handle == nullptr || media_id == nullptr || decode_data == nullptr)
    {
        cmylog::mylog("INFO","input paramer is null \n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    auto* base_media = (cbase_media*)handle;
    int result =  base_media->get_media_by_id(media_id,media_type,reinterpret_cast<uint8_t *>(decode_data),sizeof(decode_data_st_t));
    if(result > 0)
    {
        return QJ_BOX_OP_CODE_SUCESS;
    }
    else
    {
        return QJ_BOX_OP_CODE_NOSUPPORT;
    }
}
#pragma mark - 以下为新增
/**
 * 解析json
 * @param confs
 * @param media_conf
 * @return
 */
static bool parse_conf(vector<media_conf_t> &confs, const char *media_conf) {
    Json::Reader reader;
    Json::Value root;
    bool result = false;
    try {
        if (reader.parse(media_conf, root)) {
            const Json::Value media_obj = root["media"];
            for (auto i = 0; i < media_obj.size(); i++) {
                media_conf_t conf;
                conf.m_id = media_obj[i]["id"].asString();
                conf.m_url = media_obj[i]["url"].asString();
                conf.m_decode_type = media_obj[i]["decode"].asString();
                conf.m_decode_data_type = media_obj[i]["decode_data"].asInt();
                confs.push_back(conf);
            }
            result = true;
        }
    }
    catch (...) {
        cmylog::mylog("ERR", "input configure info exception\n");
    }
    return result;

}

/**
 * 添加媒体资源
 * @param handle
 * @param media_conf
 * @return
 */
int add_media_by_handle(MEDIA_HANDLE handle, const char *media_conf) {
    auto *base_media = (cbase_media *) handle;
    if (base_media == nullptr || media_conf == nullptr) {
        cmylog::mylog("ERR", "input handle is null or id is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    Json::Reader reader;
    Json::Value root;
    vector<media_conf_t> media_conf_list;

    if (parse_conf(media_conf_list, media_conf)) {
        for (auto &conf: media_conf_list) {
            auto result = std::find_if(
                    begin(base_media->get_media_conf()), end(base_media->get_media_conf()),
                    [&conf](media_conf_t c) {
                        return conf.m_id == c.m_id;
                    });
            if (result != std::end(base_media->get_media_conf())) return QJ_BOX_OP_CODE_INPUTPARAMERR;
        }

        return base_media->add_resource(media_conf_list);
    }
    return QJ_BOX_OP_CODE_UNKOWNERR;
}

///**
// * 删除媒体资源
// * @param handle
// * @param media_id
// * @return
// */
//int remove_media_by_handle(MEDIA_HANDLE handle, const char *media_id) {
//    auto *base_media = (cbase_media *) handle;
//    if (base_media == nullptr || media_id == nullptr) {
//        cmylog::mylog("ERR", "input handle is null or id is null\n");
//        return QJ_BOX_OP_CODE_INPUTPARAMERR;
//    }
//    vector<media_conf_t> media_conf_list;
//    Json::Reader reader;
//    Json::Value root;
//
//    if (parse_conf(media_conf_list, media_id)) {
//        for (auto &conf: media_conf_list) {
//            auto result = std::find_if(
//                    begin(base_media->get_media_conf()), end(base_media->get_media_conf()),
//                    [&conf](media_conf_t c) {
//                        return conf.m_id == c.m_id;
//                    });
//            if (result == std::end(base_media->get_media_conf())) return QJ_BOX_OP_CODE_INPUTPARAMERR;
//        }
//        return base_media->remove_resource(media_conf_list);
//    }
//
//    return QJ_BOX_OP_CODE_UNKOWNERR;
//}
/**
 * 删除媒体资源
 * @param handle
 * @param media_id
 * @return
 */
int remove_media_by_handle(MEDIA_HANDLE handle, const char *media_id) {
    auto *base_media = (cbase_media *) handle;
    if (base_media == nullptr || media_id == nullptr) {
        cmylog::mylog("ERR", "input handle is null or id is null\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    Json::Reader reader;
    Json::Value root;

    auto result = std::find_if(
            begin(base_media->get_media_conf()), end(base_media->get_media_conf()),
            [&media_id](media_conf_t &c) {
                return media_id == c.m_id;
            });

    if (result == std::end(base_media->get_media_conf())) return QJ_BOX_OP_CODE_INPUTPARAMERR;
    return base_media->remove_resource(media_id);
    return QJ_BOX_OP_CODE_UNKOWNERR;
}
