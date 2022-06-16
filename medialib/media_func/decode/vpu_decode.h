#ifndef __NPU_DECODE_H__
#define __NPU_DECODE_H__

#include "common.h"
#ifdef VPUMEDIA
#include "media_param.h"
#include "base_decode.h"
#include "init_data.h"

class cvpu_decode : public cbase_decode
{
public:
    cvpu_decode();
    ~cvpu_decode() override;
public:
    void init(void* param) override;
    void deinit() override;
    UINT32 data() override;
    UINT32 ctrl(std::string data) override;
    bool is_run()override;
public:

//#pragma mark -新增
    static std::timed_mutex vpu_lock;

    static void decode_func(void* in_this);
    std::thread* m_decode_func_thread= nullptr;
    ffmpeg_pull_flow_param_t* m_ffmpeg_param= nullptr;
    rk1808_flow_param_t* m_param= nullptr;
    CONDITION_VARIALBE_t m_cv;      //条件等待变量
    bool m_pkt_done= false;
    UINT8* m_yuv420p= nullptr;
private:
    bool is_run_= false;
};
#endif

#endif // __CPU_DECODE_H__