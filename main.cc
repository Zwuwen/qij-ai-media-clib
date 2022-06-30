#include <stdio.h>
#include <unistd.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<time.h>
#include<fcntl.h>
# include <string.h>
#include <iostream>

typedef struct _decode_data_st {
    int m_yuv_buf_len{};
    int m_jpg_buf_len{};
    uint8_t m_yuv_buf[1920*1080*3]{};
    uint8_t m_jpg_buf[1920*1080*3]{};
    int m_width{};
    int m_height{};
    long long dts{};
} decode_data_st_t;

#include "user_media_api.h"

char data[1920 * 1080 * 10] = {0};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("INFO: input parameter failure\n");
        return -1;
    }
#if 1
    printf("%s\n", version());
    char *conf = "{ \"media\":[{\"id\":\"123\",\"url\":\"rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101\",\"decode\":\"VPU\",\"decode_data\":2}],\"log\":{\"isdelete\":true,\"isoutput\":true,\"logpath\":\"./log\",\"level\":\"INFO\"}}";
    void *handle = init_media_qjapi(conf);
    if (handle != NULL) {
        int retvalue = start_all_media(handle);
        if (retvalue != 0) {
            printf("start pull failure\n");
        }
        decode_data_st_t *decode_inf = new decode_data_st_t();
        int loop_count = 3000;
        while (--loop_count!=0) {
            time_t timep;
            time(&timep);
            struct tm *cur_time = localtime(&timep);
            memset(data, 0x00, sizeof(data));
            int size = 0;
            //0x01:YUV420P ,0x02 jpg,0x0f all image
#if 1
            size = get_media_by_id(handle, "123", 0x01, decode_inf);
            printf("#########################################################info-%d:%d:%d,size=%d\n",
                   (cur_time->tm_hour + 12), cur_time->tm_min, cur_time->tm_sec, size);
            printf("1-w:%d\n", decode_inf->m_width);
            printf("h:%d\n", decode_inf->m_height);
            printf("jpglen:%d\n", decode_inf->m_yuv_buf_len);
            printf("yuvlen:%d\n", decode_inf->m_jpg_buf_len);
//            sleep(1);
            size = get_media_by_id(handle, "123", 0x02, decode_inf);
            printf("#########################################################info-%d:%d:%d,size=%d\n",
                   (cur_time->tm_hour + 12), cur_time->tm_min, cur_time->tm_sec, size);
            printf("2-w:%d\n", decode_inf->m_width);
            printf("h:%d\n", decode_inf->m_height);
            printf("jpglen:%d\n", decode_inf->m_yuv_buf_len);
            printf("yuvlen:%d\n", decode_inf->m_jpg_buf_len);
//            sleep(1);
            size = get_media_by_id(handle, "123", 0x0f, decode_inf);
            printf("#########################################################info-%d:%d:%d,size=%d\n",
                   (cur_time->tm_hour + 12), cur_time->tm_min, cur_time->tm_sec, size);
            printf("f-w:%d\n", decode_inf->m_width);
            printf("h:%d\n", decode_inf->m_height);
            printf("jpglen:%d\n", decode_inf->m_yuv_buf_len);
            printf("yuvlen:%d\n", decode_inf->m_jpg_buf_len);
//            sleep(1);
#endif
#if 0
            char* media_conf = "{ \"media\":[{\"id\":\"123\",\"url\":\"rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101\",\"decode\":\"VPU\",\"decode_data\":1}]}";
            reset_media_conf_by_id(handle,media_conf);
            sleep(10);
#endif
#if 0
            char* media_conf1 = "{ \"media\":[{\"id\":\"123\",\"url\":\"rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101\",\"decode\":\"VPU\",\"decode_data\":2}]}";
            reset_media_conf_by_id(handle,media_conf1);
            sleep(10); 
            size = get_media_by_id(handle,"123",0x0f,decode_inf);
            //printf("#########################################################info-%d:%d:%d,size=%d\n", (cur_time->tm_hour+12),cur_time->tm_min,cur_time->tm_sec,size);
            printf("f-w:%d\n",decode_inf->m_width);
            printf("h:%d\n",decode_inf->m_height);
            printf("jpglen:%d\n",decode_inf->m_yuv_buf_len);
            printf("yuvlen:%d\n",decode_inf->m_jpg_buf_len);
            sleep(1);
#endif
        }
        remove_media_by_handle(handle,"123");
        add_media_by_handle(handle,conf);
        start_sample_media(handle,"123");
        std::cout<<"sleep:"<<std::endl;
        sleep(20);
        int loop_count_2 = 1000;
        while (--loop_count_2!=0) {
            auto size = get_media_by_id(handle, "123", 0x0f, decode_inf);
            std::cout<<"size:"<<size<<std::endl;
            printf("f-w:%d\n", decode_inf->m_width);
            printf("h:%d\n", decode_inf->m_height);
            printf("jpglen:%d\n", decode_inf->m_yuv_buf_len);
            printf("yuvlen:%d\n", decode_inf->m_jpg_buf_len);
        }

        delete decode_inf;
        stop_all_media(handle);
        deinit_media_qjapi(handle);
    } else {
        printf("init media failure\n");
        return -1;
    }
    return 0;
#endif
}
