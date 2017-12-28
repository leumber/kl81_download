LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)  
#LOCAL_MODULE := zngsmartcard      
#LOCAL_SRC_FILES := libzngsmartcard.so   
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include   
#include $(PREBUILT_SHARED_LIBRARY)    

# ============ build libkl81boot ==========================================
include $(CLEAR_VARS)
LOCAL_LDLIBS		:= -ldl -llog
LOCAL_SHARED_LIBRARIES := libc libcrypto libdl libusb libccid libpcsclite libsqlite3 
LOCAL_PRELINK_MODULE	:= false

LOCAL_SRC_FILES := \
					kl81_bootdown.c \
					usb_bulk.c \
					bootloader_flasher.c \
					command.c \
					command_packet.c \
					usb_hid_packetizer.c \
					bin_file_validate.c \
					crypto/sha256.c \
					crypto/des_op.c \
					crypto/des.c \
						
LOCAL_LDLIBS := $(LOCAL_PATH)/libusb.so
LOCAL_C_INCLUDES	:= 	$(LOCAL_PATH)/include \
						$(LOCAL_PATH)/crypto

						
LOCAL_CFLAGS			:= -Werror $(common_cflags)
LOCAL_MODULE_TAGS       := eng
LOCAL_MODULE			:= libkl81boot

include $(BUILD_SHARED_LIBRARY)	
# ============ build kl81boot test main ==========================================
include $(CLEAR_VARS)
LOCAL_MODULE    := kl81_bootdown
LOCAL_SRC_FILES := \
					kl81_bootdown.c \
					usb_bulk.c \
					bootloader_flasher.c \
					command.c \
					command_packet.c \
					usb_hid_packetizer.c \
					bin_file_validate.c \
					crypto/sha256.c \
					crypto/des_op.c \
					crypto/des.c \
					main.c \
					
#LOCAL_SHARED_LIBRARIES£º= libusb
LOCAL_LDLIBS := $(LOCAL_PATH)/libusb.so
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
					$(LOCAL_PATH)/crypto
					
LOCAL_LDLIBS +=  -L$(SYSROOT)/system/lib -llog -lz -lm  
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE
include $(BUILD_EXECUTABLE)

