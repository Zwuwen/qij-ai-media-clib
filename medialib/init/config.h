#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "init_data.h"

typedef struct config_info
{
    log_info m_log_info;
    struct config_info& operator=(struct config_info conf)
    {
        this->m_log_info = conf.m_log_info;
        return *this;
    }
}config_info,*pconfig_info;

class cconfig
{
public:
    static UINT32 load_conf(char* file,config_info& conf);
};


#endif // __CONFIG_H__