#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__
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
    UINT32 ctrl(std::string data) override;
//#pragma mark -新增
    bool is_run() override;
public:
    ffmpeg_pull_flow_param_t* m_param{};
};


#endif // __CPU_DECODE_H__