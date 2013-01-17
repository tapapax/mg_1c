cmake_minimum_required(VERSION 2.6)

if(WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
endif()

project(mg_1c)

add_definitions(/MT)

set(CMAKE_MODULE_PATH cmake)

find_package(Poco COMPONENTS Foundation Net)
include_directories(${Poco_INCLUDE_DIRS})
link_directories(${Poco_LIBRARY_DIR})

include_directories(include)

set(SRCS dllmain.cpp Export.cpp MessageGetter.cpp SmartComponentBase.cpp stdafx.cpp Component.def)

add_library(mg_1c SHARED ${SRCS})
target_link_libraries(mg_1c ${Poco_LIBRARIES})
