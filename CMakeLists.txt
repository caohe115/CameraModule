cmake_minimum_required(VERSION 3.13)

project(Camera CXX)

set(Camera_SRCS
  ./Src/BaslerCamera.cpp  
  ./Src/HikCamera.cpp
  ./Src/CameraSystem.cpp
  )

set(Hik_include_dir "/opt/MVS/include/")
set(PLATFORM_SPECIFIC_LIBS "-lpthread")
file(GLOB Hik_libs "/opt/MVS/lib/64/*.so" ABSOLUTE)

set(Basler_include_dir "/opt/pylon5/include/")
file(GLOB Basler_libs "/opt/pylon5/lib64/*.so" ABSOLUTE)

set(Baumer_include_dir "/opt/baumer-gapi-sdk/include/bgapi2_genicam/")
file(GLOB Baumer_libs "/opt/baumer-gapi-sdk/lib/*.so" ABSOLUTE)

set(OpenCV_DIR /home/rvbust/Rvbust/Install/OpenCV/lib/cmake/opencv4)
set(OpenCV_include_dir /home/rvbust/Rvbust/Install/OpenCV/include/opencv4/)

find_package(OpenCV REQUIRED NO_DEFAULT_PATH)

set(WITH_BASLER ON)
set(WITH_HIK ON)

add_library(Camera ${Camera_SRCS})
if (WITH_BASLER)
target_include_directories(Camera PUBLIC ./Include/  ${Basler_include_dir} ${OpenCV_include_dir})
target_link_libraries(Camera PRIVATE ${Basler_libs} ${PLATFORM_SPECIFIC_LIBS} ${OpenCV_LIBS})
endif()
if (WITH_HIK)
target_include_directories(Camera PUBLIC ./Include/  ${Hik_include_dir} ${OpenCV_include_dir})
target_link_libraries(Camera PRIVATE ${Hik_libs} ${PLATFORM_SPECIFIC_LIBS} ${OpenCV_LIBS})
endif()

add_subdirectory(Tests)





