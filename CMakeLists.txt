cmake_minimum_required(VERSION 3.5)
project(animeloop-cli)

find_package(PkgConfig REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/cxxopts)

include_directories(${PROJECT_SOURCE_DIR}/cxxopts/include)
include_directories(${PROJECT_SOURCE_DIR}/research)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CLI_SOURCE_FILES
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
    animeloop-cli/utils.hpp
    animeloop-cli/thread_pool.h)

add_executable(animeloop-cli ${CLI_SOURCE_FILES})

set(SIMILAR_SAME_SOURCE_FILES
    animeloop-cli/algorithm.cpp
    animeloop-cli/algorithm.hpp
    research/similar_and_same.cpp)

add_executable(similar-and-same ${SIMILAR_SAME_SOURCE_FILES})

set(CUTS_SWITCH_SOURCE_FILES
        animeloop-cli/algorithm.cpp
        animeloop-cli/algorithm.hpp
        animeloop-cli/utils.cpp
        animeloop-cli/utils.hpp
        research/cuts_switch.cpp)

add_executable(cuts-switch ${CUTS_SWITCH_SOURCE_FILES})

set(COLOR_SOURCE_FILES
        research/color.cpp)

add_executable(color ${COLOR_SOURCE_FILES})

# Jsoncpp
pkg_check_modules(JSONCPP jsoncpp)
target_link_libraries(animeloop-cli ${JSONCPP_LIBRARIES})
### temp fix for Ubuntu ###
include_directories(animeloop-cli /usr/include/jsoncpp)

# OpenCV
find_package( OpenCV 3.2 REQUIRED )
target_link_libraries( animeloop-cli ${OpenCV_LIBS} )
target_link_libraries( similar-and-same ${OpenCV_LIBS})
target_link_libraries( cuts-switch ${OpenCV_LIBS})
target_link_libraries( color ${OpenCV_LIBS})

# Boost
find_package(Boost COMPONENTS filesystem date_time REQUIRED)
target_link_libraries( animeloop-cli ${Boost_LIBRARIES} )
target_link_libraries( cuts-switch ${Boost_LIBRARIES} )