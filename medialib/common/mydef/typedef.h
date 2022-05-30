#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__
#include "system_inc.h"
#include <mutex>
#include <condition_variable>
using namespace std;

typedef unsigned char  UINT8;
typedef unsigned short  UINT16;
typedef unsigned int  UINT32;
typedef unsigned long long int  UINT64;
typedef unsigned char*  PCHAR;
typedef bool BOOL;

//#define TRUE true
//#define FALSE false

//条件变量数据结构定义
typedef struct CONDITION_VARIALBE
{
    std::mutex m_mtx;   //锁
    std::condition_variable m_cv;   //条件变量
    void wait()
    {
        unique_lock<mutex> ql(this->m_mtx); 
        this->m_cv.wait(ql);
    }
    bool wait_for(int sec)
    {
        unique_lock<mutex> ql(this->m_mtx); 
        while(this->m_cv.wait_for(ql,std::chrono::seconds(sec)) == 
                    std::cv_status::timeout)
        {
            return false;
        }
        return true;
    }
    void notify_all()
    {
        unique_lock<mutex> ql(this->m_mtx); 
        this->m_cv.notify_all();
    }
}CONDITION_VARIALBE_t;

//线程模块数据结构定义
typedef struct THREAD_MOD
{
    typedef void (*FUNC)(void* param);
    std::thread* m_monitor_thread;
    bool m_running;
    FUNC m_func;
    void* m_param;
    THREAD_MOD(FUNC func,void* param)
    {
        this->m_running = false;
        this->m_monitor_thread = nullptr;
        this->m_func = func;
        this->m_param = param;
    }
    ~THREAD_MOD()
    {
        if(this->m_monitor_thread != nullptr)
        {
            this->m_running = false;
            this->m_monitor_thread->join();
            delete this->m_monitor_thread;
            this->m_monitor_thread = nullptr;
        }
    }
    void start()
    {
        this->m_monitor_thread = new std::thread([this]()mutable{
                        this->m_running = true;
                        while (this->m_running)
                        {
                            this->m_func(this->m_param);
                        }
                    });
    }
    void join()
    {
        while(this->m_monitor_thread != nullptr)
        {
            this->m_running = false;
            this->m_monitor_thread->join();
            delete this->m_monitor_thread;
            this->m_monitor_thread = nullptr;
            break;
        }
    }
}THREAD_MOD_t;

#endif // __TYPE_DEF_H__