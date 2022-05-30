from ctypes import *
import time


def get_yuv_raw(src, length, w=1920, h=1080):
    # len = # 3,133,440
    y_len = w * h
    uv_len = w * h >> 2

    u_start = int(length * 2 / 3)  # 2088960
    v_start = int(length * 5 / 6)  # 2611200

    data_raw = src[:y_len] + src[u_start:u_start + uv_len] + src[v_start:v_start + uv_len]
    print(u_start, " ", v_start, " ", len(data_raw))  # 3,110,400
    return data_raw, len(data_raw)


class DecodeData(Structure):
    """ creates a struct to match decode_data_st_t in c lib """

    _fields_ = [
        ('yuv_len', c_int),
        ('jpg_len', c_int),
        ('yuv_buf', c_uint8 * (1920 * 1080 * 3)),
        ('jpg_buf', c_uint8 * (1920 * 1080 * 3)),
        ('width', c_int),
        ('height', c_int),
        ('dts', c_longlong)
    ]

    def __str__(self):
        result = f'yuv_len:{self.yuv_len},jpg_len:{self.jpg_len},width:{self.width},height:{self.height},'
        result += f'yuv_buf(len):{len(self.yuv_buf)},jpg_buf(len):{len(self.jpg_buf)},dts:{self.dts}'
        return result


class MediaFunc:
    media_lib = cdll.LoadLibrary("/usr/lib/libqjmedia.so")
    handle = None
    count = 0
    dts_map = dict()

    @staticmethod
    def get_version():
        return str(MediaFunc.media_lib.version())

    @staticmethod
    def init(conf):
        MediaFunc.media_lib.init_media_qjapi.argtypes = [c_char_p]
        MediaFunc.media_lib.init_media_qjapi.restype = c_void_p
        MediaFunc.handle = MediaFunc.media_lib.init_media_qjapi(c_char_p(conf))

    @staticmethod
    def reset_media(conf):
        MediaFunc.media_lib.reset_media_conf_by_id.argtypes = [c_void_p, c_char_p]
        MediaFunc.media_lib.reset_media_conf_by_id.restype = c_int
        return MediaFunc.media_lib.reset_media_conf_by_id(MediaFunc.handle, c_char_p(conf))

    @staticmethod
    def add_media_by_handle(conf):
        MediaFunc.media_lib.add_media_by_handle.argtypes = [c_void_p, c_char_p]
        MediaFunc.media_lib.add_media_by_handle.restype = c_int
        return MediaFunc.media_lib.add_media_by_handle(MediaFunc.handle, c_char_p(conf))

    @staticmethod
    def remove_media_by_handle(media_id):
        MediaFunc.media_lib.remove_media_by_handle.argtypes = [c_void_p, c_char_p]
        MediaFunc.media_lib.remove_media_by_handle.restype = c_int
        return MediaFunc.media_lib.remove_media_by_handle(MediaFunc.handle, media_id)

    @staticmethod
    def start_all():
        MediaFunc.media_lib.start_all_media.restype = c_int
        MediaFunc.media_lib.start_all_media.argtypes = [c_void_p]
        return MediaFunc.media_lib.start_all_media(MediaFunc.handle)

    @staticmethod
    def start_sample_media(id):
        MediaFunc.media_lib.start_sample_media.restype = c_int
        MediaFunc.media_lib.start_sample_media.argtypes = [c_void_p, c_char_p]
        return MediaFunc.media_lib.start_sample_media(MediaFunc.handle, id)

    @staticmethod
    def get_media_by_id(media_id, buf_type=2):
        MediaFunc.media_lib.get_media_by_id.restype = c_int
        MediaFunc.media_lib.get_media_by_id.argtypes = [c_void_p, c_char_p, c_int, POINTER(DecodeData)]
        decode_data = DecodeData()
        # buf_type = 2
        ret = MediaFunc.media_lib.get_media_by_id(MediaFunc.handle, media_id, buf_type, byref(decode_data))
        # print(decode_data)
        # 写文件
        if ret < 0:
            return

        # dts = decode_data.dts
        width = decode_data.width
        height = decode_data.height
        yuv_buf = decode_data.yuv_buf
        yuv_size = decode_data.yuv_len
        jpg_buf = decode_data.jpg_buf
        jpg_size = decode_data.jpg_len
        if media_id in MediaFunc.dts_map:
            if MediaFunc.dts_map[media_id] == decode_data.dts:
                print(f'old frame')
                return
        MediaFunc.dts_map[media_id] = decode_data.dts
        if yuv_size:
            print("yuv image size:", yuv_size)
            # path = "jpg/" + str(MediaFunc.count) + "-" + media_id.decode() + ".yuv"
            # with open(path, "wb") as f:
            #     f.write(bytes(yuv_buf[0:yuv_size]))
            #     MediaFunc.count = MediaFunc.count + 1
        if jpg_size:
            print("jpg image size:", jpg_size)
            # path = "jpg/" + str(MediaFunc.count) + "-" + media_id.decode() + ".jpg"
            # with open(path, "wb") as f:
            #     f.write(bytes(jpg_buf[0:jpg_size]))
            #     MediaFunc.count = MediaFunc.count + 1

        if yuv_size or jpg_size:
            return True


if __name__ == '__main__':
    import json

    # YUV420P JPG
    print(MediaFunc.get_version())

    media1 = {"id": "media1", "url": "", "decode": "VPU", "decode_data": 2}
    media2 = {"id": "media2", "url": "", "decode": "VPU", "decode_data": 2}
    media3 = {"id": "media3", "url": "", "decode": "VPU", "decode_data": 2}
    media4 = {"id": "media4", "url": "", "decode": "VPU", "decode_data": 2}

    conf_dic1 = {"media": [], "log": {"isdelete": False, "isoutput": True, "logpath": "./log", "level": "INFO"}}
    conf_dic1["media"].append(media1)
    conf_dic1["media"].append(media1)
    conf_dic1["media"].append(media1)
    conf_dic1["media"].append(media1)

    media1 = {"id": "media1", "url": "rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101", "decode": "VPU",
              "decode_data": 2}
    # media2 = {"id": "media2", "url": "rtsp://admin:test0883@192.168.54.53:554/Streaming/Channels/101", "decode": "VPU",
    #           "decode_data": 3}
    conf_dic2 = {"media": [], "log": {"isdelete": False, "isoutput": True, "logpath": "./log", "level": "INFO"}}
    conf_dic2["media"].append(media1)
    # conf_dic2["media"].append(media2)

    media3 = {"id": "media3", "url": "rtsp://admin:Admin123@192.168.55.13/Streaming/Channels/101", "decode": "VPU",
              "decode_data": 2}
    # media4 = {"id": "media4", "url": "rtsp://admin:test0883@192.168.54.53:554/Streaming/Channels/101", "decode": "VPU",
    #           "decode_data": 3}
    conf_dic3 = {"media": [], "log": {"isdelete": False, "isoutput": True, "logpath": "./log", "level": "INFO"}}
    # conf_dic3["media"].append(media1)
    conf_dic3["media"].append(media3)

    # media_configure_str1 = json.dumps(conf_dic1)
    # MediaFunc.init(media_configure_str1.encode())
    media_configure_str = json.dumps(conf_dic2)
    MediaFunc.init(media_configure_str.encode())

    MediaFunc.start_all()
    # time.sleep(10)
    # MediaFunc.start_sample_media(b"media1")

    count = 0
    count_media3_add = 0
    first = True
    media3_is_use = True
    while True:
        # MediaFunc.get_media_by_id(b"media1")
        # time.sleep(1)
        # print(f"get midia")
        MediaFunc.get_media_by_id(b"media1", 1)
        time.sleep(1)

        # if media3_is_use:
        #     # print(f"add midia:{conf_dic3}")
        #     media_configure_str = json.dumps(conf_dic3)
        #     result = MediaFunc.add_media_by_handle(media_configure_str.encode())
        #     # print(f"add midia result:{result}")
        #
        #     if result == 0:
        #         result = MediaFunc.start_sample_media(b"media3")
        #         # time.sleep(5)
        #         print(f'start media3:{result}')
        #
        #     # print(f"get midia3")
        #     result = MediaFunc.get_media_by_id(b"media3", 2)
        #     if result: count += 1
        #
        # if count == 1000:
        #     result = MediaFunc.remove_media_by_handle(b"media3")
        #     # print(f"remove media3 result:{result}")
        #     if result == 0: print(f"remove media3{'😂' * 8}")
        #     media3_is_use = False
        #     count_media3_add += 1
        #
        # if count_media3_add == 1000:
        #     print(f'reuse media3{"🚕" * 8}')
        #     count_media3_add = 0
        #     count = 0
        #     media3_is_use = True
        # time.sleep(1)
        # MediaFunc.get_media_by_id(b"media2", 2)
        # time.sleep(1)
        #
        # MediaFunc.get_media_by_id(b"media2", 3)
        # time.sleep(1)
        #
        # count = count + 1
        # if count >= 10 and first == True:
        #     print("media_configure_str2:", conf_dic2)
        #     media_configure_str2 = json.dumps(conf_dic2)
        #     ret = MediaFunc.reset_media(media_configure_str2.encode())
        #     print("reset_media ret:", ret)
        #     first = False
        #     #
        #     ret = MediaFunc.start_sample_media(b"media2")
        #     print("start_sample_media ret:", ret)
        #     time.sleep(10)
