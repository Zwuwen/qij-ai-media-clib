#####arch gcc#####
#ARCH = x86
ARCH = arm

ifeq ($(ARCH),x86)
  C++ = g++
endif
ifeq ($(ARCH),arm)
#  C++ =  /home/peng/work/tmp/host/usr/bin/aarch64-linux-g++
  C++ =  /home/dev/data/wrm/host/bin/aarch64-linux-g++
endif

#FLAGS= -Wall -O2 -MD -shared -std=c++11 -U_FORTIFY_SOURCE -g
#FLAGS := -Wall -O2 -MD -shared -fPIC -std=c++11 -U_FORTIFY_SOURCE  -fpermissive
#FLAGS := -Wall -O0 -MD -shared -fPIC -std=c++11 -U_FORTIFY_SOURCE  -fpermissive
FLAGS := -Wall -O0 -MD -shared -fPIC -std=c++14 -U_FORTIFY_SOURCE  -fpermissive

MEDIAARCH := vpu
ifeq ($(MEDIAARCH),cpu)
	COMPILE_FLAG= -DCPUMEDIA
endif
ifeq ($(MEDIAARCH),vpu)
	COMPILE_FLAG= -DVPUMEDIA
endif

PWD=$(shell pwd)

#####h.cc#####
ifeq ($(ARCH),arm)
#SYS_INC := -I /usr/local/aarch64/ffmpeg/include 
SYS_INC := -I /usr/local/aarch64/jpeg-8d/include
SYS_INC += -I /usr/local/aarch64/sysroot/usr/include
#SYS_INC += -I /home/dev/data/wrm/host/aarch64-buildroot-linux-gnu/sysroot/usr/include
SYS_INC += -I /usr/local/aarch64/jsoncpp/include
#SYS_INC += -I /usr/local/aarch64/hiredis/include

endif
ifeq ($(ARCH),x86)
SYS_INC := -I /usr/local/ffmpeg/include -I/usr/include/sys -I/usr/include
SYS_INC += -I /usr/local/libjpeg/include
SYS_INC += -I /usr/local/include
SYS_INC += -I /usr/local/include/hiredis
endif
ifeq ($(MEDIAARCH),vpu)
#SYS_INC += -I/usr/local/aarch64/mpp/inc
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/inc
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/mpp/inc
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/osal/inc/
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/utils/
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/mpp/base/inc/
SYS_INC += -I/home/dev/data/wrm/host1/mpp-release/mpp/codec/inc/
endif

USER_INC := -I ./common
USER_INC += -I ./common/mydef
USER_INC += -I ./common/log
USER_INC += -I ./common/utils
USER_INC += -I ./common/utils/sync_task
USER_INC += -I ./common/utils/queue
USER_INC += -I ./common/utils/cache
USER_INC += -I ./common/utils/queue/redis
USER_INC += -I ./media_func
USER_INC += -I ./media_func/decode
USER_INC += -I ./media_func/media_utils
USER_INC += -I ./media_func/media_utils/jpg
USER_INC += -I ./media_func/media_utils/yuv
USER_INC += -I ./init

SRC := $(wildcard ./*.cc)
SRC += $(wildcard ./init/*.cc)
SRC += $(wildcard ./common/*.cc)
SRC += $(wildcard ./common/mydef/*.cc)
SRC += $(wildcard ./common/log/*.cc)
SRC += $(wildcard ./common/utils/*.cc)
SRC += $(wildcard ./common/utils/cache/*.cc)
SRC += $(wildcard ./common/utils/sync_task/*.cc)
SRC += $(wildcard ./common/utils/queue/*.cc)
SRC += $(wildcard ./common/utils/queue/redis/*.cc)
SRC += $(wildcard ./media_func/*.cc)
SRC += $(wildcard ./media_func/decode/*.cc)
SRC += $(wildcard ./media_func/media_utils/*.cc)
SRC += $(wildcard ./media_func/media_utils/jpg/*.cc)
SRC += $(wildcard ./media_func/media_utils/yuv/*.cc)

TARGET_O := $(foreach src,$(SRC), $(patsubst %cc,%o,$(src)) )

ifeq ($(ARCH),arm)
LIB = -L/usr/local/aarch64/sysroot/lib -L/usr/local/aarch64/sysroot/usr/lib -lpthread -lm -lavcodec -lswscale -lavutil -lavdevice -lavfilter -lavformat
#LIB = -L/home/dev/data/wrm/host1/aarch64-buildroot-linux-gnu/sysroot/lib -L/home/dev/data/wrm/host/aarch64-buildroot-linux-gnu/sysroot/usr/lib -lpthread -lm -lavcodec -lswscale -lavutil -lavdevice -lavfilter -lavformat
LIB += -L/usr/local/aarch64/jpeg-8d/lib -ljpeg
LIB += -L/usr/local/aarch64/jsoncpp/lib -ljsoncpp
#LIB += -L/usr/local/aarch64/hiredis/lib -lhiredis
#LIB += -L/home/peng/work/tmp/host/usr/lib
endif
ifeq ($(ARCH),x86)
LIB = -L/usr/lib -L/usr/local/lib -lpthread -lm 
LIB +=-L/usr/lib/x86_64-linux-gnu -ljsoncp1920*1080p
LIB += -L/usr/local/ffmpeg/lib -lavcodec -lswscale -lavutil -lavdevice -lavfilter -lavformat
LIB += -L/usr/local/libjpeg/lib -ljpeg	-L/usr/local/lib -lhiredis -luv
endif
ifeq ($(MEDIAARCH),vpu)
#LIB += -L/usr/local/aarch64/mpp/lib -lrockchip_vpu -lrockchip_mpp
LIB += -L/home/dev/data/wrm/host1/mpp-release/mpp/ -lrockchip_mpp
LIB += -L/home/dev/data/wrm/host1/mpp-release/mpp/legacy/ -lrockchip_vpu

endif

TARGET_BIN=libqjmedia.so

#$(C++)  $(FLAGS) $(COMPILE_FLAG) -c  $(SRC) $(SYS_INC) $(USER_INC) $(LIB) 
#$(C++) -o output/$(TARGET_BIN)  $(notdir $(TARGET_O)) $(LIB)
all:
	@echo "$(shell date "+%Y-%m-%d %H:%M:%S")-INFO:compile begin"
	$(C++)  $(FLAGS) $(COMPILE_FLAG) -o $(TARGET_BIN)  $(SRC) $(SYS_INC) $(USER_INC) $(LIB) 
	@echo "$(shell date "+%Y-%m-%d %H:%M:%S")-INFO:end"
	@./cp.sh

.PHONY:clean
clean:
	@rm $(TARGET_BIN) -rf
	@rm *.d -rf
	@echo "clean complete"
