cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/CMakeModules/")

# Set options for this project
set(PROJECT_NAME "sms_gateway" CXX)
project(${PROJECT_NAME})

message(STATUS "============================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "============================")

# set the installation prefix to a custom one for testing purpose
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed" CACHE PATH "default install path" FORCE )
    message(STATUS "Install prefix (overriden): ${CMAKE_INSTALL_PREFIX}")
endif()

set(PROJECT_SOURCE_DECLARATION_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DEFINITION_DIRECTORY ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_BUILD_TYPE Debug)
message( STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}" )

# Note: http://unlogic.co.uk/2012/08/13/cmake-and-gcov/
# lcov and gcovr must be installed
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -W -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage") # -Wshadow 
SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

add_definitions(-std=c++11)

enable_testing()

# Load settings for the external project "Boost".
# The components "system" and "filesystem" from "Boost" version 1.55.0 or
# greater are required.
find_package( Boost 1.55.0 REQUIRED COMPONENTS log regex date_time filesystem thread )
if (Boost_FOUND)
    # From the offical documentation:
    # Add include directories to the build. [...] If the SYSTEM option is given,
    # the compiler will be told the directories are meant as system include
    # directories on some platforms (signalling this setting might achieve effects
    # such as the compiler skipping warnings [...])."
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})

    message(STATUS "The Boost version: ${Boost_VERSION}")
    message(STATUS "The Boost include directory: ${Boost_INCLUDE_DIR}")
    message(STATUS "The Boost libraries: ${Boost_LIBRARIES})")
    
    # From the offical documentation:
    # "Specify directories in which the linker will look for libraries. [...] Note
    # that this command is rarely necessary. Library locations returned by
    # find_package() and find_library() are absolute paths. Pass these absolute
    # library file paths directly to the target_link_libraries() command. CMake
    # will ensure the linker finds them."
    link_directories (${Boost_LIBRARY_DIRS})
endif ()

# Load external "gSoap"
find_package(GSOAP 2.8 REQUIRED)
message(STATUS "The GSOAP version: ${GSOAP_VERSION}")
message(STATUS "The GSOAP include directory: ${GSOAP_INCLUDE_DIR}")
#message(STATUS "The GSOAP libraries: ${GSOAP_LIBRARIES}")
message(STATUS "The GSOAP SSL libraries: ${GSOAP_SSL_LIBRARIES}")

#Create the directory that will host files generated by GSOAP
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated/SmsTrade)

#some files are generated by gsoap
set_source_files_properties( ${CMAKE_BINARY_DIR}/generated/SmsTrade/SmstradeBinding.nsmap          PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_BINARY_DIR}/generated/SmsTrade/soapC.cpp                      PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_BINARY_DIR}/generated/SmsTrade/soapSmstradeBindingProxy.cpp   PROPERTIES GENERATED TRUE ) 

# collect all *.cpp files to SRC_FILES
# Add application files and gsoap dependencies
file(GLOB SRC_FILES ${CMAKE_BINARY_DIR}/generated/SmsTrade/*.nsmap)
aux_source_directory(${CMAKE_BINARY_DIR}/generated/SmsTrade SRC_FILES) # find all source files in ./build/generated/
aux_source_directory(src SRC_FILES)                                    # find all source files in ./src/

# collect all *.h files to SRC_FILES
file(GLOB HEADER_FILES include/*.h ${CMAKE_BINARY_DIR}/generated/SmsTrade/*.h)

message(STATUS "Source files: ${SRC_FILES}")
message(STATUS "Header files: ${HEADER_FILES}")

include_directories (${PROJECT_SOURCE_DECLARATION_DIRECTORY} ${CMAKE_BINARY_DIR}/generated/SmsTrade ${GSOAP_INCLUDE_DIR})
include_directories(tests)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include)

add_subdirectory(tests)


#Create a cmake target that generate gsoap files
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/generated/SmsTrade/SmsTradeQuery.h
    COMMAND ${GSOAP_WSDL2H} -o ${CMAKE_BINARY_DIR}/generated/SmsTrade/SmsTradeQuery.h https://gateway.smstrade.de/soap/index.php?WSDL
    COMMENT "CREATING BINDING FILES: SmsTradeQuery.h"
)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/generated/SmsTrade/soapSmstradeBindingProxy.cpp ${CMAKE_BINARY_DIR}/generated/SmsTrade/soapC.cpp
    COMMAND ${GSOAP_SOAPCPP2} -j -CLx -d ${CMAKE_BINARY_DIR}/generated/SmsTrade -I/usr/share/gsoap/import/ ${CMAKE_BINARY_DIR}/generated/SmsTrade/SmsTradeQuery.h 
    DEPENDS ${CMAKE_BINARY_DIR}/generated/SmsTrade/SmsTradeQuery.h
    COMMENT "CREATING STUBS AND GLUE CODE"
)

add_custom_target(SMSTRADE_GSOAP_TARGET DEPENDS ${CMAKE_BINARY_DIR}/generated/SmsTrade/soapSmstradeBindingProxy.cpp)

#Make sure that the soap client is compiled only after gsoap has been processed
add_dependencies(${PROJECT_NAME} SMSTRADE_GSOAP_TARGET)

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES} ${GSOAP_SSL_LIBRARIES}) # ${GSOAP_LIBRARIES} 
