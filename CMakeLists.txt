cmake_minimum_required(VERSION 3.6)
project(animeloop_cli)

add_subdirectory(${PROJECT_SOURCE_DIR}/cxxopts)
add_subdirectory(${PROJECT_SOURCE_DIR}/jsoncpp)

include_directories(${PROJECT_SOURCE_DIR}/cxxopts/include)
include_directories(${PROJECT_SOURCE_DIR}/jsoncpp/include/json)

find_package( OpenSSL 1.0.0 REQUIRED )


find_package( OpenCV 3.2.0 REQUIRED )


find_package(Boost 1.63.0 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    animeloop-cli/algorithm.cpp
    animeloop-cli/algorithm.hpp
    animeloop-cli/filter.cpp
    animeloop-cli/filter.hpp
    animeloop-cli/loop_video.cpp
    animeloop-cli/loop_video.hpp
    animeloop-cli/main.cpp
    animeloop-cli/models.cpp
    animeloop-cli/models.hpp
    animeloop-cli/utils.cpp
    animeloop-cli/utils.hpp)

add_executable(animeloop_cli ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} ${OpenSSL_LIBS})
target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})
set(Boost_USE_STATIC_LIBS ON)
target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBS})
