#ifndef __MYLOG_H__
#define __MYLOG_H__

#define INFO            "INFO"
#define WAR             "WAR"
#define ERR              "ERR"

class cmylog
{
public:
    static void init_log(string log,string log_level,BOOL is_delete,BOOL is_output_std);
    static void deinit_log();
    static void write_log(char* logmsg);
    static void mylog(string log_level,char* fmt,... );
    static void mylog(string log_level,const char* file,const char* func,UINT32 line,char* fmt,... );
};

#endif // __MYLOG_H__