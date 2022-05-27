#ifndef __REDIS_QUEUE_H__
#define __REDIS_QUEUE_H__

#include <map>

class credis_queue
{
public:
    /*
        redis_info = {
            "host": "127.0.0.1",
            "isunix":   true,
            "port": 6379,
            "timeout":  {"sec":1,"secv":500000}  #1.5S
        }
    */
    credis_queue();
    virtual ~credis_queue();
    bool product(std::string key,std::string value);
    const char* customer(std::string key);
    void destroy_customer_res();
    bool publish(std::string key,std::string value);
    const char* subscribe(std::string key);
    void destroy_subscribe_res();
    bool init(std::map<std::string,std::string>& redis_info);
    void destroy();
private:
    bool m_islink;
    redisContext* m_redis_contxt;
    std::string m_passwd;
    redisReply* m_customer_reply;
    redisReply* m_subscribe_reply;
};
#endif //__REDIS_QUEUE_H__