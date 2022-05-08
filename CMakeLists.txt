cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
project(Network)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES src/*.h src/*.cpp src/*.c)

add_library(${PROJECT_NAME} ${SOURCE_FILES} tests/main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/${PROJECT_NAME}.a" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/)
else()
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/${PROJECT_NAME}.so" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/)
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/include/${PROJECT_NAME}" FILES_MATCHING PATTERN "*.h")

project(Test)
set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES src/*.h src/*.cpp src/*.c)

add_executable(Test ${SOURCE_FILES} tests/main.cpp)

target_link_libraries(Test wsock32 ws2_32)


