from ctypes import *
import time

class MediaFunc:
    media_lib = cdll.LoadLibrary("/usr/lib/libqjmedia.so")
    handle = None
    @staticmethod
    def get_version():
        MediaFunc.media_lib.version.restype = c_char_p
        return str(MediaFunc.media_lib.version(),encoding="utf-8")
    @staticmethod
    def init(conf):
        MediaFunc.media_lib.init_media_qjapi.argtypes = [c_char_p]
        MediaFunc.media_lib.init_media_qjapi.restype = c_void_p
        MediaFunc.handle = MediaFunc.media_lib.init_media_qjapi(c_char_p(conf))
    @staticmethod
    def start_all():
        MediaFunc.media_lib.start_all_media.restype = c_int
        MediaFunc.media_lib.start_all_media.argtypes = [c_void_p]
        return MediaFunc.media_lib.start_all_media(MediaFunc.handle)
    @staticmethod
    def get_media_by_id(id):
        MediaFunc.media_lib.get_media_by_id.restype = c_int
        MediaFunc.media_lib.get_media_by_id.argtypes = [c_void_p,c_char_p,c_char_p,c_char_p,c_int]
        data = (c_char*(1920*1080*10))()
        size = MediaFunc.media_lib.get_media_by_id(MediaFunc.handle,id,b"YUV420SP",data,1920*1080*10)
        print(size)
        print(type(data))


if __name__ == '__main__':
    print(MediaFunc.get_version());
    conf = b"{ \"media\":[{\"id\":\"123\",\"url\":\"rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101\",\"decode\":\"NPU\",\"decode_data\":\"YUV420SP\"}],\"log\":{\"isdelete\":true,\"isoutput\":true,\"logpath\":\"./log\",\"level\":\"INFO\"}}"
    MediaFunc.init(conf)
    MediaFunc.start_all()
    while(True):
        MediaFunc.get_media_by_id(b"123")
        time.sleep(1)

    
