#ifndef __YUV_H__
#define __YUV_H__

class cyuv
{
public:
    //返回值为转换后的数据长度,y_size = w*h
    static UINT32 yuv420sp2yuv420p(void* src,UINT32 src_len,void* dst,UINT32 dst_len,UINT32 width,UINT32 heigh,UINT32 h_stride,UINT32 v_stride);
};

#endif //YUV_H