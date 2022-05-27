#include "async_task.h"
#include "err_code.h"

template<class FUNC_TYPE,class TASK_RESULT>
void casync_task<FUNC_TYPE,TASK_RESULT>::register_task(FUNC_TYPE func,void* param,char task_type)
{
    this->m_task_handler = std::async(static_cast<std::launch>(task_type),func,param);
}

template<class FUNC_TYPE,class TASK_RESULT>
TASK_RESULT casync_task<FUNC_TYPE,TASK_RESULT>::wait(unsigned char timeout)
{
    this->m_task_handler.wait_for(std::chrono::seconds(timeout));
    return static_cast<TASK_RESULT>(this->m_task_handler.get());
}

/*
int test(void* param)
{
    printf("%s\n",static_cast<char*>(param));
    return 1;
}

int main(void)
{

    cbase_sync_task<function_task,int>* sync_task = new casync_task<function_task,int>();
    sync_task->register_task(test,(void*)("123"),static_cast<char>(IMMEDIATE));
    int result = sync_task->wait(1);
    printf("=========%d\n",result);
    delete  sync_task;

    return 0;
}
*/