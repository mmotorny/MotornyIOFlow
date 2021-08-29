# Copyright (c) 2021, Maksym Motorny. All rights reserved.
# Use of this source code is governed by a 3-clause BSD license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 3.21)

project(MotornyIOFlow)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/boost)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glog)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/XPSDK)

# ------------------------------------------------------------------------------
add_library(MotornyIOFlow_CommonOptions INTERFACE)

target_compile_features(MotornyIOFlow_CommonOptions INTERFACE
    cxx_std_23
)

if (MSVC)
  target_compile_options(MotornyIOFlow_CommonOptions INTERFACE
      /W4
      /WX
  )

  target_link_options(MotornyIOFlow_CommonOptions INTERFACE
      /INCREMENTAL:NO
  )
endif()

target_compile_definitions(MotornyIOFlow_CommonOptions INTERFACE 
    XPLM200
    XPLM210
    XPLM300
    XPLM301
    XPLM302
)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  target_compile_definitions(MotornyIOFlow_CommonOptions INTERFACE 
      MOTORNY_IOFLOW_PLATFORM_WINDOWS
      UNICODE
  )
endif()

# ------------------------------------------------------------------------------
add_library(MotornyIOFlow SHARED 
    src/Plugin.cpp
)

set_target_properties(MotornyIOFlow PROPERTIES
    SUFFIX .xpl
)

target_link_libraries(MotornyIOFlow
    Boost::locale
    glog
    MotornyIOFlow_CommonOptions
    XPLM
)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  install(TARGETS MotornyIOFlow
      RUNTIME 
      DESTINATION win_x64
      COMPONENT "MotornyIOFlow"
  )
endif()
