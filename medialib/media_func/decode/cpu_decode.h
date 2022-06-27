#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__
#include <list>
#include "common.h"
#include "media_param.h"
#include "base_decode.h"
#include "init_data.h"

class ccpu_decode : public cbase_decode 
{
public:
    ccpu_decode();
    ~ccpu_decode() override;
public:
    void init(void* param) override;
    void deinit() override;
    UINT32 data() override;
    void decode_process();
    UINT32 ctrl(std::string data) override;
//#pragma mark -新增
    bool is_run() override;
public:
    ffmpeg_pull_flow_param_t* m_param{};
private:
    std::list<AVPacket> pktList{};
    std::mutex packet_list_lock_{};
    std::thread* m_decode_func_thread{};
    bool is_run_= false;
};


#endif // __CPU_DECODE_H__