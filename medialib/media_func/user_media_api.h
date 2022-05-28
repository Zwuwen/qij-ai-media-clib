#ifndef __USER_MEDIA_H__
#define __USER_MEDIA_H__

#pragma once

#define QJ_MEDIA_API    extern "C"
#define MEDIA_HANDLE  void*

QJ_MEDIA_API char* version();
//初始化媒体资源
/*
    media configure={
        media:[
            {"id":"123","url":"rtsp://xxxx","decode":"NPU","decode_data":1},
            {"id":"456","url":"rtsp://xxxx"},
        ]
        log:{
            "isdelete": true,
            "isoutput": true,
            "logpath":  "xxxx",
            "level":    "INFO"
        }
    }
*/
QJ_MEDIA_API MEDIA_HANDLE init_media_qjapi(char* media_conf);
//回收媒体资源，出入句柄
QJ_MEDIA_API void deinit_media_qjapi(MEDIA_HANDLE handle);
//启动所有媒体资源
QJ_MEDIA_API int start_all_media(MEDIA_HANDLE handle);
//启动指定ID的媒体资源
QJ_MEDIA_API int start_sample_media(MEDIA_HANDLE handle,char* media_id);
//关闭所有媒体资源
QJ_MEDIA_API int stop_all_media(MEDIA_HANDLE handle);
//关闭指定ID的媒体资源
QJ_MEDIA_API int stop_sample_media(MEDIA_HANDLE handle,char* media_id);
//修改媒体配置信息
QJ_MEDIA_API int reset_media_conf_by_id(MEDIA_HANDLE handle,char* media_conf);
//指定ID获取媒体YUV数据
/*
    param:
        media_id:媒体ID，
        media_type:媒体类型，包括YUV420P,JPG
        data: 媒体数据缓存
        size: 期望的媒体数据缓存长度
        width,height:数据分辨率
    返回值：返回真实媒体数据长度
*/
QJ_MEDIA_API int get_media_by_id(MEDIA_HANDLE handle,char* media_id,int media_type,decode_data_st_t* decode_data);

QJ_MEDIA_API int remove_media_by_handle(MEDIA_HANDLE handle,const char* media_id);
QJ_MEDIA_API int add_media_by_handle(MEDIA_HANDLE handle,const char* media_conf);

#endif // __USER_MEDIA_H__