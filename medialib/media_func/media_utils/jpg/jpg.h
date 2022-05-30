#ifndef __JPG_H__
#define __JPG_H__
#include "common.h"

class cjpg
{
public:
    struct jpeg_compress_struct m_cinfo;
    struct jpeg_error_mgr m_jerr;
    UINT8* m_compress_buff{};
    unsigned long m_compress_buff_size{};
    //UINT8 m_yuvbuf[1920*3];
    UINT8 m_yuvbuf[1920*1080*3]{};
public:
    cjpg();
    virtual ~cjpg();
    void init_conv_param(UINT32 width,UINT32 height);
    void release_jpeg_mem();
    bool yuv2jpg(UINT8* yuv);
};

#endif