cmake_minimum_required(VERSION 3.15)
project(Network CXX)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h src/*.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(Network ${SOURCE_FILES} ${HEADER_FILES})

if (CONAN_BUILD)
    find_package(util REQUIRED)
    target_link_libraries(Network util::util)
else ()
    target_compile_definitions(Network BUILD_LIBRARY)
    target_link_libraries(Network Util)
endif ()

if (WIN32)
    target_link_libraries(Network wsock32 ws2_32)
endif ()

target_include_directories(Network PUBLIC include)

install(TARGETS Network ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION "${CMAKE_INSTALL_PREFIX}/include" FILES_MATCHING PATTERN "*.h")
