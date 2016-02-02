cmake_minimum_required(VERSION 2.8)
project( PiCamCVTest )
SET(COMPILE_DEFINITIONS -Werror)

include_directories(/usr/include ${PIROOT}/opt/vc/include /opt/vc/include/interface/vcos/pthreads /opt/vc/include/interface/vmcs_host/linux )
link_directories( /opt/vc/lib )
add_executable(PiCamCVTest main.cpp camera.cpp cameracontrol.cpp graphics.cpp)

target_link_libraries(PiCamCVTest libmmal_core.so libmmal_util.so libmmal_vc_client.so libvcos.so librt.so libbcm_host.so GLESv2 EGL libopencv_core.so libopencv_imgproc.so)
