cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(cmake)

project(Network)
setupProject(LIB_STATIC src)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

project(TestServer)
setupProject(EXE tests/TestServer.cpp)
target_link_libraries(${PROJECT_NAME} Network)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

project(TestClient)
setupProject(EXE tests/TestClient.cpp)
target_link_libraries(${PROJECT_NAME} Network)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

