#include "shared_data.h"

cshared_sample_cache::cshared_sample_cache(UINT32 size)
{
    if(size == 0)
    {
        this->m_data = new UINT8[12441626 ];
        this->m_data_len = 12441626;
    }
    else
    {
        this->m_data = new UINT8[size];
        this->m_data_len = size;
    }
    this->m_data_relen = m_data_len;
}
cshared_sample_cache::~cshared_sample_cache()
{
    if(this->m_data != NULL)
    {
        delete[] this->m_data;
        this->m_data = NULL;
        this->m_data_len = 0;
    }
}

UINT32 cshared_sample_cache::get_cache_size()
{
    return this->m_data_len;
}
UINT32 cshared_sample_cache::put(UINT8* data,UINT32 size)
{
    unique_lock<mutex> ql(this->m_mtx); 
    if(data == NULL || size == 0 || size > this->m_data_len)
    {
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }

    memset(this->m_data,0x00,m_data_len);
    memcpy(this->m_data,data,size);
    this->m_data_relen = size;

    return QJ_BOX_OP_CODE_SUCESS;
}
UINT32 cshared_sample_cache::get(UINT8* data,UINT32 size)
{
    unique_lock<mutex> ql(this->m_mtx); 
    if(data == nullptr || size == 0 )
    {
        return 0; 
    }
#pragma mark ÐÂÔö
    if(this->m_data_len==0) return 0;

    if(size > this->m_data_relen)
    {
        memcpy(data,this->m_data,this->m_data_relen);
        return this->m_data_relen;
    }
    else
    {
        memcpy(data,this->m_data,size);
        return size;
    }
}
