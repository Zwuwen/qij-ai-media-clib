#ifndef __BASE_DECODE_H__
#define __BASE_DECODE_H__

#pragma once
#include "init_data.h"

class cbase_decode 
{
public:
    cbase_decode();
    virtual ~cbase_decode();
public:
    virtual void init(void* param) = 0;
    virtual void deinit() = 0;
    virtual UINT32 data() = 0;
    virtual UINT32 ctrl(std::string data) = 0;
public:
    UINT32 get(UINT8* data,UINT32 size);
    void set_cache_type(UINT8 type);
public:
    cshared_sample_cache* m_shared_cache;
    decode_data_st_t* m_decode_info;
    UINT8 m_shared_cache_type;
};


#endif // __CPU_DECODE_H__