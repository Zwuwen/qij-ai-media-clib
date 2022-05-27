#ifndef __ASYNC_TASK_H__
#define __ASYNC_TASK_H__

#include "base_sync_task.h"

typedef int(*function_task)(void* param);

#define IMMEDIATE       std::launch::async
#define DELAY                 std::launch::deferred
#define COMBO              IMMEDIATE | DELAY

template<class FUNC_TYPE,class TASK_RESULT>
class casync_task    :   public  cbase_sync_task<FUNC_TYPE,TASK_RESULT> 
{
public:
    void register_task(FUNC_TYPE func,void* param,char task_type);
    TASK_RESULT wait(unsigned char timeout) ;
private:
    std::future<TASK_RESULT> m_task_handler;
};

#endif // __ASYNC_TASK_H__