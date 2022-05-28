#include "common.h"

static string g_log_file;
static string g_log_level;
static volatile BOOL g_outputstd = true;

static pthread_mutex_t g_mutex_lock;

void cmylog::init_log(string log,string log_level,BOOL is_delete,BOOL is_output_std)
{
    g_log_file = log + string("/media.log");
    if(is_delete)
    {
        remove(g_log_file.c_str());
    }

    pthread_mutex_init(&g_mutex_lock,NULL);
    g_log_level = log_level;
    g_outputstd = is_output_std;
}

void cmylog::deinit_log()
{
    pthread_mutex_destroy(&g_mutex_lock);
}

void cmylog::write_log(char* logmsg)
{
    try
    {
        //return;
        int fd = open((const char*)g_log_file.c_str(),O_RDWR|O_APPEND|O_CREAT,0664);
        if(fd < 0)
        {
            return;
        }
        write(fd,logmsg,strlen(logmsg) +1);
        close(fd);
    }
    catch(const std::exception& e)
    {
        return ;
    }
}

void cmylog::mylog(string log_level,char* fmt,... )
{
    char buf[2048] = {0};
    memset(buf,0x00,sizeof(buf));

    va_list args;
    //local time
    time_t timep;
    time(&timep);
    struct tm* cur_time = localtime(&timep);
    if(log_level == "INFO")
        snprintf(buf,2048,"\033[47;32m%s:%d-%d-%d %d:%d:%d \033[0m",log_level.c_str(),(1900+cur_time->tm_year),(1+cur_time->tm_mon),cur_time->tm_mday,
                                                                                                (cur_time->tm_hour+12),cur_time->tm_min,cur_time->tm_sec);
    if(log_level == "WAR")
        snprintf(buf,2048,"\033[47;33m%s:%d-%d-%d %d:%d:%d \033[0m",log_level.c_str(),(1900+cur_time->tm_year),(1+cur_time->tm_mon),cur_time->tm_mday,
                                                                                                (cur_time->tm_hour+12),cur_time->tm_min,cur_time->tm_sec);
    if(log_level == "ERR")
        snprintf(buf,2048,"\033[47;31m%s:%d-%d-%d %d:%d:%d \033[0m",log_level.c_str(),(1900+cur_time->tm_year),(1+cur_time->tm_mon),cur_time->tm_mday,
                                                                                                (cur_time->tm_hour+12),cur_time->tm_min,cur_time->tm_sec);
    va_start(args,fmt);
    vsnprintf(buf + strlen(buf),2048-strlen(buf),fmt,args);
    va_end(args);

    pthread_mutex_lock(&g_mutex_lock);
    if(g_outputstd)
        printf(buf);

    if(g_log_level == "INFO")
    {
        cmylog::write_log(buf); 
    }
    else if((g_log_level == "WAR" && log_level == "WAR") ||(g_log_level == "WAR" && log_level == "ERR")) 
    {
        cmylog::write_log(buf); 
    }
    else if(g_log_level == "ERR" && log_level == "ERR") 
    {
        cmylog::write_log(buf); 
    }
    pthread_mutex_unlock(&g_mutex_lock);
}

void cmylog::mylog(string log_level,const char* file,const char* func,UINT32 line,char* fmt,... )
{

}