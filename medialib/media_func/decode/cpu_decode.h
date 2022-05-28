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
    ~ccpu_decode();
public:
    void init(void* param);
    void deinit();
    UINT32 data();
    UINT32 ctrl(std::string data);
#pragma mark -新增
    bool is_run() override;
public:
    ffmpeg_pull_flow_param_t* m_param;
};


#endif // __CPU_DECODE_H__