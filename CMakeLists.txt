cmake_minimum_required(VERSION 3.15)
project(Network CXX)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h src/*.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(Network ${SOURCE_FILES} ${HEADER_FILES})

if (WIN32)
    target_link_libraries(Network wsock32 ws2_32)
endif ()

target_include_directories(Network PUBLIC include)
set_target_properties(Network PROPERTIES PUBLIC_HEADER "include/Network/Network.h")
install(TARGETS Network)

