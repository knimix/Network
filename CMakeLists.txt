cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
project(Network)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE SOURCE_FILES src/*.h src/*.cpp src/*.c)

add_library(${PROJECT_NAME} ${SOURCE_FILES} tests/TestServer.cpp src/Server/Server.cpp src/Server/Server.h src/Client/Client.cpp src/Client/Client.h)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "lib${PROJECT_NAME}")

set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/lib${PROJECT_NAME}.a" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/)
else()
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/build/lib${PROJECT_NAME}.a" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/)
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/export/${PROJECT_NAME}/include/${PROJECT_NAME}" FILES_MATCHING PATTERN "*.h")

project(TestServer)
set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES src/*.h src/*.cpp src/*.c)

add_executable(TestServer ${SOURCE_FILES} tests/TestServer.cpp)

if (WIN32)
    target_link_libraries(TestServer wsock32 ws2_32)
endif ()

project(TestClient)
set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES src/*.h src/*.cpp src/*.c)

add_executable(TestClient ${SOURCE_FILES} tests/TestClient.cpp)

if (WIN32)
    target_link_libraries(TestClient wsock32 ws2_32)
endif ()


