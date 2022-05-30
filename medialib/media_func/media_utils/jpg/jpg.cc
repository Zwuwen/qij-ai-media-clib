#include "jpg.h"

cjpg:: cjpg()
= default;
cjpg:: ~cjpg()
= default;
void cjpg::init_conv_param(UINT32 width,UINT32 height)
{
    this->m_compress_buff = nullptr;
    this->m_compress_buff_size = 0;
    memset(this->m_yuvbuf,0x00,sizeof(this->m_yuvbuf));
    this->m_cinfo.err = jpeg_std_error(&this->m_jerr);
    jpeg_create_compress(&m_cinfo);
    jpeg_mem_dest(&m_cinfo,(UINT8**)&this->m_compress_buff,&this->m_compress_buff_size);
    this->m_cinfo.image_width = width;
    this->m_cinfo.image_height = height;
    this->m_cinfo.input_components = 3;
    this->m_cinfo.in_color_space = JCS_YCbCr;//设置输入图片的格式，支持RGB/YUV/YCC等等
    //this->m_cinfo.dct_method = JDCT_FLOAT;
    jpeg_set_defaults(&this->m_cinfo);  //其它参数设置为默认的！
    jpeg_set_quality(&this->m_cinfo, 60, (boolean)true); //设置转化图片质量，范围0-100
    this->m_cinfo.in_color_space = JCS_YCbCr;//设置输入图片的格式，支持RGB/YUV/YCC等等
    // 指定亮度及色度质量  
    this->m_cinfo.q_scale_factor[0] = jpeg_quality_scaling(100);  
    this->m_cinfo.q_scale_factor[1] = jpeg_quality_scaling(100); 
    this->m_cinfo.comp_info[0].h_samp_factor = 2;
    this->m_cinfo.comp_info[0].v_samp_factor = 2;
}

void cjpg::release_jpeg_mem()
{
    if(this->m_compress_buff != NULL)
    {
        free(this->m_compress_buff);
        this->m_compress_buff = NULL;
    }
    jpeg_destroy_compress(&this->m_cinfo);
}

bool cjpg::yuv2jpg(UINT8* yuv)
{
    if(yuv == nullptr)
    {
        return false;
    }
    jpeg_start_compress(&this->m_cinfo, (boolean)true);
    UINT32 width = this->m_cinfo.image_width;
    UINT32 height = this->m_cinfo.image_height;
    unsigned char* py = yuv;
    unsigned char* pu = yuv + width*height;
    //unsigned char* pv = yuv +  width * height*5 / 4; 
    unsigned char* pv = pu +  width * height / 4; 

    JSAMPROW row_pointer[1];

    UINT32 j = 0;
    while (this->m_cinfo.next_scanline < this->m_cinfo.image_height) 
    {
        int idx = 0;
        for (uint32_t i = 0; i<width; i++)
        {
  	        this->m_yuvbuf[idx++] = py[i + j * width];
  	        this->m_yuvbuf[idx++] = pu[(j>>1) * width/2 + (i>>1) ];
  	        this->m_yuvbuf[idx++] = pv[(j>>1) * width/2 + (i>>1) ];
        }
        row_pointer[0] = this->m_yuvbuf;
        jpeg_write_scanlines(&this->m_cinfo, row_pointer, 1);
        j++;
    }
    jpeg_finish_compress(&this->m_cinfo);

    return true;
}