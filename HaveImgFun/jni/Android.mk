LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES := on
OPENCV_CAMERA_MODULES  := off
include /cygdrive/d/OpenCV-2.4.5-android-sdk/OpenCV-2.4.5-android-sdk/sdk/native/jni/OpenCV.mk

include $(CLEAR_VARS)
#OPENCV_LIB_TYPE:=STATIC
LOCAL_MODULE    := sift_prebuilt
LOCAL_SRC_FILES := libnonfree.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
#OPENCV_LIB_TYPE:=STATIC
LOCAL_MODULE    := opencv_java_prebuilt
LOCAL_SRC_FILES := libopencv_java.so
include $(PREBUILT_SHARED_LIBRARY)
  

#OPENCV_LIB_TYPE:=DYNAMIC  
#include ../includeOpenCV.mk

##ifeq ("$(wildcard $(OPENCV_MK_PATH))","")  
###try to load OpenCV.mk from default install location  
##include $(TOOLCHAIN_PREBUILT_ROOT)/user/share/OpenCV/OpenCV.mk  
##else  
##include $(OPENCV_MK_PATH)  
##endif  

include $(CLEAR_VARS)
#OPENCV_LIB_TYPE:=STATIC
LOCAL_C_INCLUDES += /cygdrive/d/OpenCV-2.4.5-android-sdk/OpenCV-2.4.5-android-sdk/sdk/native/jni/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_MODULE    := ImgFun
LOCAL_LDLIBS     += -llog -ldl -lm
LOCAL_SHARED_LIBRARIES := sift_prebuilt opencv_java_prebuilt
LOCAL_SRC_FILES := SiftMatching.cpp SurfMatching.cpp BriskMatching.cpp FreakMatching.cpp OrbMatching.cpp ImgFun.cpp

include $(BUILD_SHARED_LIBRARY) 

