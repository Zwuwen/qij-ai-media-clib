#ifndef __DECODE_H__
#define __DECODE_H__
#include "common.h"
#include "data_def.h"

template<class DECODE>
class cdecode_temp 
{
public:
    bool init(redis_key_info_t& key,std::map<std::string,std::string>& redis_info,void* param);
    void start();
    void deinit();
public:
    DECODE m_decode;    //解码器
    redis_key_info_t m_redis_key;
    credis_queue m_redis_queue;
    std::thread* m_ctrl_thread;
    std::thread* m_data_thread;
    bool m_running;
};

template<class DECODE>
bool cdecode_temp<DECODE>::init(redis_key_info_t& key,std::map<std::string,std::string>& redis_info,void* param)
{
    m_ctrl_thread = NULL;
    m_data_thread = NULL;
    m_running = false;
    if(m_redis_queue.init(redis_info) == false)
        return false;
    
    this->m_decode.init(param);

    this->m_redis_key = key;
    return true;
}

template<class DECODE>
void cdecode_temp<DECODE>::start()
{
    this->m_running = true;
    //listen ctrl msg
    this->m_ctrl_thread = new std::thread([this]()mutable{
        while(this->m_running)
        {
            printf("ctrl subscribe key=%s\n",m_redis_key.m_ctrl_key.c_str()); 
            const char* data = this->m_redis_queue.customer(this->m_redis_key.m_ctrl_key);
            if(data == NULL)
            {
                printf("ctrl subscribe null\n");
                continue;
            }
            this->m_redis_queue.destroy_customer_res();
            printf("ctrl thread complete\n");
        }
    });
    assert(this->m_ctrl_thread != NULL);
    //listen data msg
    this->m_data_thread = new std::thread([this]()mutable{
        printf("data customer key=%s\n",m_redis_key.m_data_key.c_str()); 
        while(this->m_running)
        {
            const char* data = this->m_redis_queue.customer(this->m_redis_key.m_data_key);
             if(data == NULL)
            {
                printf("data customer null\n");
                continue;
            }
            printf("customer h264 data,customer=%s\n",data);
            printf("decode h264 complete\n");
            this->m_redis_queue.destroy_customer_res();
            printf("release decode\n");
        }
    });
    assert(this->m_ctrl_thread != NULL);
}

template<class DECODE>
void cdecode_temp<DECODE>::deinit()
{
    this->m_running = false;
    this->m_redis_queue.destroy();
    if(this->m_ctrl_thread != NULL)
    {
        this->m_ctrl_thread->join();
        delete this->m_ctrl_thread;
        this->m_ctrl_thread = NULL;
    }
    if(this->m_data_thread != NULL)
    {
        this->m_data_thread->join();
        delete this->m_data_thread;
        this->m_data_thread = NULL;
    }
}

#endif // __DECODE_H__