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
#FLAGS := -Wall -O2 -MD -shared -std=c++11 -U_FORTIFY_SOURCE  -fpermissive
FLAGS := -Wall -O0 -MD -shared -std=c++11 -U_FORTIFY_SOURCE  -fpermissive

MEDIAARCH := vpu
ifeq ($(MEDIAARCH),cpu)
	COMPILE_FLAG= -DCPUMEDIA
endif
ifeq ($(MEDIAARCH),vpu)
	COMPILE_FLAG= -DVPUMEDIA
endif

PWD=$(shell pwd)

#####h.cc#####
USER_INC += -I ./medialib/media_func
USER_INC += -I ./medialib/init

SRC := $(wildcard ./*.cc)
TARGET_O := $(foreach src,$(SRC), $(patsubst %cc,%o,$(src)) )


LIB = -L./medialib -lqjmedia
ifeq ($(ARCH),arm)
#LIB += -L/usr/local/aarch64/sysroot/lib -L/usr/local/aarch64/sysroot/usr/lib -lpthread -lm -lavcodec -lswscale -lavutil -lavdevice -lavfilter -lavformat
LIB += -L/usr/local/aarch64/sysroot/usr/lib
LIB += -L/usr/local/aarch64/sysroot/lib
LIB += -L/usr/local/aarch64/jpeg-8d/lib -ljpeg
LIB += -L/usr/local/aarch64/jsoncpp/lib -ljsoncpp
#LIB += -L/home/peng/work/tmp/host/usr/lib
endif
TARGET_BIN=my-media
SYS_ROOT_PATH= /home/dev/data/wrm/host/aarch64-buildroot-linux-gnu/sysroot
all:
	$(C++) -Wall -O0 -D_GLIBCXX_USE_CXX11_ABI=0  $(COMPILE_FLAG)  $(SRC) $(SYS_INC) $(USER_INC) $(LIB)  -o $(TARGET_BIN)
	#$(C++)  -Wall -O0 -MD -std=c++11 -U_FORTIFY_SOURCE  -fpermissive -o output/$(TARGET_BIN)  $(notdir $(TARGET_O)) $(LIB)
.PHONY:clean
clean:
	@echo "clean complete"
