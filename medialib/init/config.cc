#include "common.h"
#include "config.h"

UINT32 cconfig::load_conf(char* file,config_info& conf)
{
    if(file == nullptr || !is_file_exist((const char*)file))
    {
        printf("INFO:config file isnot exist\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }

    std::ifstream is;
    try
    {
        is.open(file,std::ios::binary);
        //解析json配置文件
        Json::Reader reader;
        Json::Value root;
        if(reader.parse(is,root,FALSE))
        {
            config_info info;
            //media json
            if(!root["media"].isNull())
            {
                const Json::Value media_obj = root["media"]["mediaList"];
                for(int i=0;i<media_obj.size();i++)
                {
                    /*
                    media_conf conf;
                    conf.url = media_obj[i]["url"].asString();
                    conf.media_type = media_obj[i]["mediaType"].asString();
                    conf.output_filepath = media_obj[i]["outputFile"].asString();
                    conf.codec = media_obj[i]["codec"].asString();
                    conf.net_type = media_obj[i]["netType"].asString();
                    conf.timeout = media_obj[i]["timeout"].asString();
                    conf.net_sec_timeout = media_obj[i]["netSecTimeout"].asInt();
                    info.m_media_info.m_media_res_list.push_back(conf);*/
                }
                printf("INFO:media resource list configure \n");
                
                //log json
                info.m_log_info.m_is_del = root["log"]["isDelete"].asBool();
                info.m_log_info.m_is_outputstd = root["log"]["isOuputStd"].asBool();
                info.m_log_info.m_log_path = root["log"]["logPath"].asString();
                info.m_log_info.m_log_levl = root["log"]["level"].asString();
                printf("INFO log configure:%d,%d,%s,%s\n",info.m_log_info.m_is_del,info.m_log_info.m_is_outputstd,
                info.m_log_info.m_log_path.c_str(),info.m_log_info.m_log_levl.c_str());

                //load config success;
                conf = info;
                is.close();
            }
            else
            {
                is.close();
                printf("ERR:json file error\n");
                return QJ_BOX_OP_CODE_INPUTPARAMERR;
            }
        }
        else
        {
            printf("ERROR: file parse failure\n");
            if(is.is_open())
                is.close();
            return QJ_BOX_OP_CODE_ACCESSFAILURE;
        }
    }
    catch(...)
    {
        if(is.is_open())
            is.close();
        printf("ERROR: file refuse access\n");
        return QJ_BOX_OP_CODE_ACCESSFAILURE;
    }

    return QJ_BOX_OP_CODE_SUCESS;
}