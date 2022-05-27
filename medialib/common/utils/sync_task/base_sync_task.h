#ifndef __BASE_SYNC_TASK_H__
#define __BASE_SYNC_TASK_H__
#include<future>
#include<thread>
#include <iostream>  
using namespace std;

template<class FUNC_TYPE,class TASK_RESULT>
class cbase_sync_task
{
public:
    virtual void register_task(FUNC_TYPE func,void* param,char task_type) = 0;
    virtual  TASK_RESULT wait(unsigned char timeout) = 0;
};

#endif // __BASE_SYNC_TASK_H__