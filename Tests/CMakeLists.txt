add_executable(TestHikCamera TestHikCamera.cpp)
target_link_libraries(TestHikCamera PRIVATE Camera)

add_executable(TestBaslerCamera TestBaslerCamera.cpp)
target_link_libraries(TestBaslerCamera PRIVATE Camera)

