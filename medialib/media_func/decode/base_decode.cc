#include "common.h"
#include "base_decode.h"

cbase_decode::cbase_decode()
{
    m_shared_cache_type = DECODE_DATA_JPG; 
    m_decode_info = new decode_data_st_t();
    m_shared_cache = nullptr;
}
cbase_decode::~cbase_decode()
{
    if(m_decode_info != nullptr)
    {
        delete m_decode_info;
        m_decode_info = nullptr;
    }
    if(m_shared_cache != nullptr)
    {
        delete m_shared_cache;
        m_shared_cache = nullptr;
    }
}

UINT32 cbase_decode::get(UINT8* data,UINT32 size)
{
    if(this->m_shared_cache != nullptr)
    {
        return this->m_shared_cache->get(data,size);
    }
    else
    {
        return 0;
    }
}

void cbase_decode::set_cache_type(UINT8 type)
{
    this->m_shared_cache_type = type;
}
