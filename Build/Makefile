SDK_PATH:=$~/home/ahliao/AR_SDK/ARDrone_SDK_2_0_1/ARDroneLib/
PC_TARGET=yes
USE_LINUX=yes

ifdef MYKONOS
   include $(ARDRONE_CUSTOM_CONFIG)
   include $(ARDRONE_BUILD_CONFIG)
else
   include $(SDK_PATH)/Soft/Build/custom.makefile
   include $(SDK_PATH)/Soft/Build/config.makefile
endif

ifeq "$(RELEASE_BUILD)" "yes"
   ARDRONE_TARGET_DIR=$(shell pwd)/Release
else
   ARDRONE_TARGET_DIR=$(shell pwd)/Debug
endif

TARGET=linux_video_demo

SRC_DIR:=$(shell pwd)/../Sources

# Define application source files
GENERIC_BINARIES_SOURCE_DIR:=$(SRC_DIR)

GENERIC_BINARIES_COMMON_SOURCE_FILES+=\
Video/pre_stage.c\
Video/post_stage.c\
Video/display_stage.c\
Video/qr_reader.c

GENERIC_BINARIES_COMMON_SOURCE_FILES+=\
Video/apriltags/apriltag.c\
Video/apriltags/image_u8.c\
Video/apriltags/tag36h11.c\
Video/apriltags/zarray.c\
Video/apriltags/g2d.c\
Video/apriltags/graymodel.c\
Video/apriltags/homography.c\
Video/apriltags/image_f32.c\
Video/apriltags/image_u32.c\
Video/apriltags/matd.c\
Video/apriltags/segment2.c\
Video/apriltags/tag36h10.c\
Video/apriltags/unionfind.c\
Video/apriltags/workerpool.c\
Video/apriltags/zhash.c\

GENERIC_INCLUDES+=					\
	$(SRC_DIR) \
	$(LIB_DIR) \
	$(SDK_PATH)/Soft/Common \
	$(SDK_PATH)/Soft/Lib 

GENERIC_TARGET_BINARIES_PREFIX=

GENERIC_TARGET_BINARIES_DIR=$(ARDRONE_TARGET_DIR)

GENERIC_BINARIES_SOURCE_ENTRYPOINTS+=			\
   ardrone_testing_tool.c

GENERIC_INCLUDES:=$(addprefix -I,$(GENERIC_INCLUDES))
GENERIC_INCLUDES += `pkg-config --cflags opencv` -std=gnu99

GENERIC_LIB_PATHS=-L$(GENERIC_TARGET_BINARIES_DIR)
GENERIC_LIBS=-lpc_ardrone -lrt -lgtk-x11-2.0 `pkg-config --libs opencv` -lcairo 


SDK_FLAGS+="USE_APP=yes"
SDK_FLAGS+="APP_ID=linux_video_demo"

export GENERIC_CFLAGS
export GENERIC_LIBS
export GENERIC_LIB_PATHS
export GENERIC_INCLUDES
export GENERIC_BINARIES_SOURCE_DIR
export GENERIC_BINARIES_COMMON_SOURCE_FILES
export GENERIC_TARGET_BINARIES_PREFIX
export GENERIC_TARGET_BINARIES_DIR
export GENERIC_BINARIES_SOURCE_ENTRYPOINTS

# Bug fix ...
export GENERIC_LIBRARY_SOURCE_DIR=$(GENERIC_BINARIES_SOURCE_DIR)


.PHONY: $(TARGET) build_libs

all: build_libs $(TARGET)

$(TARGET):
	@$(MAKE) -C $(SDK_PATH)/VP_SDK/Build $(TMP_SDK_FLAGS) $(SDK_FLAGS) $(MAKECMDGOALS) USE_LINUX=yes
	mv $(ARDRONE_TARGET_DIR)/ardrone_testing_tool $(TARGET)
	mv $(TARGET) $(ARDRONE_TARGET_DIR)/

$(MAKECMDGOALS): build_libs
	@$(MAKE) -C $(SDK_PATH)/VP_SDK/Build $(TMP_SDK_FLAGS) $(SDK_FLAGS) $(MAKECMDGOALS) USE_LINUX=yes

build_libs:
	@$(MAKE) -C $(SDK_PATH)/Soft/Build $(TMP_SDK_FLAGS) $(SDK_FLAGS) $(MAKECMDGOALS) USE_LINUX=yes

