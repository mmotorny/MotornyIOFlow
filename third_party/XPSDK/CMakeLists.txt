# Copyright (c) 2021, Maksym Motorny. All rights reserved.
# Use of this source code is governed by a 3-clause BSD license that can be
# found in the LICENSE file.

include(FetchContent)

# ------------------------------------------------------------------------------
FetchContent_Declare(XPSDK303
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/XPSDK/XPSDK303
  URL https://developer.x-plane.com/wp-content/plugins/code-sample-generation/sample_templates/XPSDK303.zip
)

FetchContent_MakeAvailable(XPSDK303)

# ------------------------------------------------------------------------------
add_library(XPLM STATIC IMPORTED GLOBAL)

set_target_properties(XPLM PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${xpsdk303_SOURCE_DIR}/CHeaders/XPLM
)
if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  set_target_properties(XPLM PROPERTIES
      IMPORTED_LOCATION ${xpsdk303_SOURCE_DIR}/Libraries/Win/XPLM_64.lib
      INTERFACE_COMPILE_DEFINITIONS IBM
  )
endif()
