cmake_minimum_required(VERSION 3.10)
project(CMake_Projects)

set(CMAKE_CXX_STANDARD 17)
set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib") # OpenCV 설치 경로 맞춰주기

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("C:/Users/user/Libraries/eigen-5.0.0")

file(GLOB SRC_FILES src/*.cpp)

add_executable(MyCVApp ${SRC_FILES})
target_link_libraries(MyCVApp ${OpenCV_LIBS})