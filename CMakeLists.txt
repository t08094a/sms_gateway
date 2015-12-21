cmake_minimum_required(VERSION 2.6)
project(sms_gateway)

add_executable(sms_gateway main.cpp)

install(TARGETS sms_gateway RUNTIME DESTINATION bin)
