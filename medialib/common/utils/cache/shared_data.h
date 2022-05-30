#ifndef __SHARED_DATA_H__
#define __SHARED_DATA_H__
#include "common.h"

class cshared_sample_cache
{
public:
    cshared_sample_cache(UINT32 size);
    ~cshared_sample_cache();
    UINT32 get_cache_size() const;
    UINT32 put(UINT8* data,UINT32 size);
    /*
        size:欲读取字节数，
        返回值为实际读取字节数
    */
    UINT32 get(UINT8* data,UINT32 size);
private:
    std::mutex m_mtx;
    UINT8* m_data;
    UINT32 m_data_len;          /*cache总长度*/
    UINT32 m_data_relen;    /*数据真实长度*/
};

#endif // __SHARED_DATA_H__