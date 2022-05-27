#####arch gcc#####
ARCH = x86
#ARCH = arm

ifeq ($(ARCH),x86)
  C++ = g++
endif
ifeq ($(ARCH),arm)
  C++ =  /home/peng/work/tmp/host/usr/bin/aarch64-linux-g++
endif

#FLAGS= -Wall -O2 -MD -shared -std=c++11 -U_FORTIFY_SOURCE -g
FLAGS := -Wall -O2 -MD -shared -std=c++11 -U_FORTIFY_SOURCE  -fpermissive

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
LIB += -L/usr/local/aarch64/sysroot/lib -L/usr/local/aarch64/sysroot/usr/lib -lpthread -lm -lavcodec -lswscale -lavutil -lavdevice -lavfilter -lavformat
LIB += -L/usr/local/aarch64/jpeg-8d/lib -ljpeg
LIB += -L/usr/local/aarch64/jsoncpp/lib -ljsoncpp
LIB += -L/usr/local/aarch64/hiredis/lib -lhiredis
LIB += -L/home/peng/work/tmp/host/usr/lib 
endif
TARGET_BIN=my-media

all:
	@rm output -rf
	@echo "$(shell date "+%Y-%m-%d %H:%M:%S")-INFO:compile begin"
	@[ -d "./output" ] || mkdir output
	$(C++)  $(FLAGS) $(COMPILE_FLAG) -c  $(SRC) $(SYS_INC) $(USER_INC) $(LIB) 
	$(C++) -o output/$(TARGET_BIN)  $(notdir $(TARGET_O)) $(LIB)
	@mv ./*.o ./output/
	@mv ./*.d ./output/
	@chmod 777 output/$(TARGET_BIN)
	@echo "$(shell date "+%Y-%m-%d %H:%M:%S")-INFO:end"
	./cp.sh
.PHONY:clean
clean:
	@rm output -rf
	@rm jpg/* -rf
	@echo "clean complete"
