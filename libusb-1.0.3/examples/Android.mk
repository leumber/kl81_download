#LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_SRC_FILES:= lsusb.c
#LOCAL_MODULE := lsusb
#LOCAL_MODULE_TAGS = eng 
#LOCAL_PRELINK_MODULE := false
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
#LOCAL_SHARED_LIBRARIES := libc libusb
#include $(BUILD_EXECUTABLE)
