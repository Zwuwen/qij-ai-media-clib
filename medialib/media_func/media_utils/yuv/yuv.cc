#include "common.h"
#include "yuv.h"

UINT32 cyuv::yuv420sp2yuv420p(void* src,UINT32 src_len,void* dst,UINT32 dst_len,UINT32 width,UINT32 height,UINT32 h_stride,UINT32 v_stride)
{
//    //420sp point param
//    auto* sp_y = (UINT8*)src;
//    UINT8* sp_uv = (UINT8*)(src) + h_stride*v_stride;
//    //420p point param
//    auto* p_y = (UINT8*)dst;
//    UINT8* p_u = (UINT8*)(dst) + h_stride*v_stride;
//    UINT8* p_v = (UINT8*)(dst) + h_stride*v_stride*5/4;
//    //UINT8* p_v = (UINT8*)(p_u + h_stride*v_stride/4);
//
//    //change running
//    memset(dst,0x00,dst_len);
//    //cp y
//    memcpy(p_y,sp_y,h_stride*v_stride);
//    //cp u
//    int j = 0;
//    for(int i = 0 ; i < h_stride*v_stride/4;i++)
//    {
//        p_u[i] = sp_uv[j];
//        j +=2;
//    }
//    //cp v
//    j = 1;
//    for(int i = 0 ; i < h_stride*v_stride/4;i++)
//    {
//        p_v[i] = sp_uv[j];
//        j +=2;
//    }
//    int yuv_len = h_stride*v_stride +  (h_stride*v_stride>>1);
//    // return strlen(dst) + 1;
//    return yuv_len;

    //420sp point param
    auto* sp_y = (UINT8*)src;
    UINT8* sp_uv = sp_y + h_stride*v_stride;

    //420p point param
    uint32_t out_y_size = width*height;
    uint32_t out_u_or_v_size = width * height >> 2;//u or v size

    auto* p_y = (UINT8*)dst;
    UINT8* p_u = p_y + out_y_size;
    UINT8* p_v = p_u + out_u_or_v_size;

    //change running
//    memset(dst,0x00,dst_len);
    //cp y
    memcpy(p_y,sp_y,out_y_size);
    //cp u
    int j = 0;
    for(uint32_t i = 0 ; i < out_u_or_v_size;i++)
    {
        p_u[i] = sp_uv[j];
        j +=2;
    }
    //cp v
    j = 1;
    for(uint32_t i = 0 ; i < out_u_or_v_size;i++)
    {
        p_v[i] = sp_uv[j];
        j +=2;
    }
    uint32_t yuv_len = out_y_size +  (out_u_or_v_size<<1);
    return yuv_len;
}