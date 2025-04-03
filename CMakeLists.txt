cmake_minimum_required(VERSION 3.16)
project(modbus_tcp_cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(modbus_tcp_cpp source/MBServer.cpp)
target_link_libraries(modbus_tcp_cpp PRIVATE modbus)
target_include_directories(modbus_tcp_cpp PRIVATE include)

IF (MB_BUILD_EXAMPLES)
	add_executable(modbus_server source/server.cpp) 
	target_link_libraries(modbus_server PRIVATE modbus_tcp_cpp)
	target_include_directories(modbus_server PRIVATE include)

	add_executable(modbus_write source/write.cpp)
	target_link_libraries(modbus_write PRIVATE modbus)
	target_include_directories(modbus_write PRIVATE include)

	add_executable(modbus_read source/read.cpp)
	target_link_libraries(modbus_read PRIVATE modbus)
	target_include_directories(modbus_read PRIVATE include)
ENDIF()
