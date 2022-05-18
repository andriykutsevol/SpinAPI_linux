cmake_minimum_required(VERSION 3.6)

project(spinapi)

set(VER_STRING_API "User Compiled" CACHE STRING "API Version String")
set(LIBRARY_NAME "spinapi" CACHE STRING "API Name")
set(ARCHITECTURE "x64" CACHE STRING "Platform architecture")

if(WIN32)
  set(SPINAPI_SRC  
    caps.c debug.c driver-usb-windows.c if.c io.c spinapi.c usb.c util.c)
elseif(LINUX)
  set(SPINAPI_SRC
    caps.c debug.c spinapi.c driver-direct-linux.c driver-usb-linux.c if.c usb.c util.c)
endif()

add_definitions(-DVER_STRING_API="${VER_STRING_API}")

include_directories(ftd2xx)
if(WIN32)
  include_directories(ddk_headers)

  set(LIBRARIES kernel32.lib setupapi.lib)

  if(${ARCHITECTURE} STREQUAL "x64") 
    link_directories(ftd2xx/Windows/amd64)
    set(LIBRARIES libdriver-windows ftd2xx.lib ${LIBRARIES})
  else()
    link_directories(ftd2xx/Windows/i386)
    set(LIBRARIES libdriver-windows32 ftd2xx.lib ${LIBRARIES})
  endif()

  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DDLL_EXPORTS)
elseif(LINUX)
  set(LIBRARIES ftd2xx m dl rt pthread)
  
  if(${ARCHITECTURE} STREQUAL "x64")
    link_directories(ftd2xx/Linux/amd64)
  else()
    link_directories(ftd2xx/Linux/i386)
  endif()
  
  #TODO: ADD DEFINITIONS FOR EXPORTS
endif()

add_library(${LIBRARY_NAME} SHARED ${SPINAPI_SRC})
install(TARGETS ${LIBRARY_NAME} 
        LIBRARY DESTINATION lib/ 
        ARCHIVE DESTINATION lib/ 
        RUNTIME DESTINATION lib/
        CONFIGURATIONS Release)

install(FILES spinapi.h pulseblaster.h dds.h radioprocessor.h spinpts.h
        DESTINATION include/)

install(DIRECTORY ./ DESTINATION src/)

if(WIN32 OR LINUX)
  target_link_libraries(${LIBRARY_NAME} ${LIBRARIES})
endif()

