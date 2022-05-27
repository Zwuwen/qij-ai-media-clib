#ifndef __SYSTEM_INC_H__
#define __SYSTEM_INC_H__

#define LINUX

#include<stdio.h>
#include<string>
#include<vector>
#include<json/json.h>
#include<fstream>
#include<iostream>
#include <memory>
#include <thread>
#include <ratio>
#include <chrono>
#include <future>
#include <mutex>
#include <condition_variable>

#ifdef LINUX
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
#endif
#include<assert.h>
#include <jpeglib.h>
#include <exception>

using namespace std;
using namespace std::chrono;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
};

//extern "C"
//{
//#include <async.h>
//#include <hiredis.h>
//};
#ifdef VPUMEDIA
extern "C"
{
#include <rk_mpi.h>
#include <mpp_log.h>
#include <mpp_mem.h>
#include <mpp_env.h>
#include <mpp_time.h>
#include <mpp_common.h>
#include <utils.h>
#include <mpi_impl.h>
#include <mpp_impl.h>
#include <mpp_info.h>
#include <mpp_enc_cfg.h>
}
#endif

#endif // __SYSTEM_INC_H__