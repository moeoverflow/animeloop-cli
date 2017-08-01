cmake_minimum_required(VERSION 3.5)
project(animeloop-cli)

find_package(PkgConfig REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/cxxopts)

include_directories(${PROJECT_SOURCE_DIR}/cxxopts/include)
include_directories(${PROJECT_SOURCE_DIR}/jsoncpp/dist/json)
include_directories(${PROJECT_SOURCE_DIR}/research)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
    jsoncpp/dist/jsoncpp.cpp
        )

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

# OpenCV
find_package( OpenCV 3.2 REQUIRED )
target_link_libraries( animeloop-cli ${OpenCV_LIBS} )
target_link_libraries( similar-and-same ${OpenCV_LIBS})
target_link_libraries( cuts-switch ${OpenCV_LIBS})
target_link_libraries( color ${OpenCV_LIBS})

# Boost
find_package(Boost COMPONENTS filesystem date_time system program_options REQUIRED)
target_link_libraries( animeloop-cli ${Boost_LIBRARIES} )
target_link_libraries( cuts-switch ${Boost_LIBRARIES} )

# Search OpenSSL
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

target_link_libraries( animeloop-cli ${OPENSSL_LIBRARIES})
target_link_libraries( cuts-switch ${OPENSSL_LIBRARIES})