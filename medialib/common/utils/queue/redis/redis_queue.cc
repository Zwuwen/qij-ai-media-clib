#include "common.h"
#include "redis_queue.h"

credis_queue::credis_queue()
{
    m_redis_contxt = NULL;
}
credis_queue:: ~credis_queue()
{

}
bool credis_queue::init(std::map<std::string,std::string>& redis_info)
{
    try{
        //printf("%s %d\n",redis_info["host"].c_str(),atoi(redis_info["port"].c_str()));
        //malloc resource
        struct timeval timeout = {atoi(redis_info["sec"].c_str()),atol(redis_info["vsec"].c_str())};
        this->m_redis_contxt = redisConnectWithTimeout((const char*)redis_info["host"].c_str(),atoi(redis_info["port"].c_str()), timeout);
        if(this->m_redis_contxt == NULL || this->m_redis_contxt->err)
        {
            return false;
        }
        redisReply* reply = (redisReply*)redisCommand(this->m_redis_contxt,"AUTH %s", redis_info["passwd"].c_str());
        assert(reply != NULL);
        freeReplyObject(reply);
        this->m_islink = true;

        return true;
    }
    catch(...){
        printf("catch\n");
        this->destroy();
        this->m_islink = false;
        return false;
    }
}
void credis_queue::destroy()
{
    printf("redis destroy\n");
    if(this->m_redis_contxt != NULL)
    {
        redisFree(this->m_redis_contxt);
        m_redis_contxt = NULL;
    }
    this->m_islink = false;
}
bool credis_queue::product(std::string key,std::string value)
{
    if(this->m_islink == false)
    {
        return false;
    }
    try{
        redisReply* reply = (redisReply*)redisCommand(this->m_redis_contxt,"RPUSH %s %s", key.c_str(), value.c_str());
        assert(reply != NULL);
        freeReplyObject(reply);
        return true;
    }
    catch(...){
        return false;
    }
}

const char* credis_queue::customer(std::string key)
{
    if(this->m_islink == false)
    {
        return NULL;
    }
    try{
        m_customer_reply = (redisReply*)redisCommand(this->m_redis_contxt,"LPOP %s", key.c_str());
        if(m_customer_reply != NULL)
            return m_customer_reply->str;
        return NULL;
    }
    catch(...){
        return NULL;
    }
}
void credis_queue::destroy_customer_res()
{
    if(m_customer_reply != NULL)
        freeReplyObject(m_customer_reply);
}

bool credis_queue::publish(std::string key,std::string value)
{
    if(this->m_islink == false)
    {
        return false;
    }
    try{
        redisReply* reply = (redisReply*)redisCommand(this->m_redis_contxt,"PUBLISH %s %s", key.c_str(), value.c_str());
        assert(reply != NULL);
        freeReplyObject(reply);
        return true;
    }
    catch(...){
        return false;
    }
}

const char* credis_queue::subscribe(std::string key)
{
    if(this->m_islink == false)
    {
        return NULL;
    }
    try{
        m_subscribe_reply = (redisReply*)redisCommand(this->m_redis_contxt,"SUBSCRIBE %s", key.c_str());
        if(m_subscribe_reply != NULL)
            return m_subscribe_reply->str;
        return NULL;
    }
    catch(...){
        return NULL;
    }
}
void credis_queue::destroy_subscribe_res()
{
    if(m_subscribe_reply != NULL)
        freeReplyObject(m_subscribe_reply);
}

/*
    credis_queue redis_queue;
    std::map<std::string,std::string> redis_info;
    redis_info.insert(pair<std::string,std::string>("host","127.0.0.1"));
    redis_info.insert(pair<std::string,std::string>("port","6379"));
    redis_info.insert(pair<std::string,std::string>("passwd","1"));
    redis_info.insert(pair<std::string,std::string>("sec","1"));
    redis_info.insert(pair<std::string,std::string>("vsec","500000"));
    if(redis_queue.init(redis_info) == false)
        return 0;
    printf("init complete\n");
    redis_queue.product("key12","123");
    redis_queue.customer("key12");
    redis_queue.destroy();
    return 0;
*/