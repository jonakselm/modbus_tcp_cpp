cmake_minimum_required(VERSION 3.16)
project(modbus_test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(modbus_server server.cpp)
target_link_libraries(modbus_server PRIVATE modbus)

add_executable(modbus_write write.cpp)
target_link_libraries(modbus_write PRIVATE modbus)

add_executable(modbus_read read.cpp)
target_link_libraries(modbus_read PRIVATE modbus)
