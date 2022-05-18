cmake_minimum_required(VERSION 3.6)

project(examples)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX ".")
endif() 
  
set(SPINAPI_PATH "../" CACHE STRING "Path to SpinAPI install directory.")

include_directories(${SPINAPI_PATH}/include)
include_directories(Shared)
link_directories(${SPINAPI_PATH}/lib)

if(${ARCHITECTURE} STREQUAL "x64") 
  set(LIBRARY_NAME "spinapi64" CACHE STRING "API Name")
else()
  set(LIBRARY_NAME "spinapi" CACHE STRING "API Name")
endif()

option(COMPILE_GENERAL "Compile general example programs." ON)
option(COMPILE_PB24 "Compile PulseBlaster24 example programs." OFF)
option(COMPILE_PBDDSI "Compile PulseBlasterDDS-I example programs." OFF)
option(COMPILE_PBDDSII "Compile PulseBlasterDDS-II example programs." OFF)
option(COMPILE_PBDDSIII "Compile PulseBlasterDDS-III example programs." OFF)
option(COMPILE_PBESR "Compile PulseBlasterESR example programs." OFF)
option(COMPILE_PBESRPRO "Compile PulseBlasterESR-PRO example programs." OFF)
option(COMPILE_PBESRPROII "Compile PulseBlasterESR-PRO-II example programs." OFF)
option(COMPILE_PBQUAD "Compile PulseBlaster QuadCore example programs." OFF)
option(COMPILE_RP "Compile RadioProcessor example programs." OFF)
option(COMPILE_PTS "Compile PTS example programs." OFF)

if(COMPILE_GENERAL)
add_subdirectory(General)
endif()

if(COMPILE_PB24)
add_subdirectory(PulseBlaster24)
endif()

if(COMPILE_PBDDSI)
add_subdirectory(PulseBlasterDDS-I)
endif()

if(COMPILE_PBDDSII)
add_subdirectory(PulseBlasterDDS-II)
endif()

if(COMPILE_PBDDSIII)
add_subdirectory(PulseBlasterDDS-III)
endif()

if(COMPILE_PBESR)
add_subdirectory(PulseBlasterESR)
endif()

if(COMPILE_PBESRPRO)
add_subdirectory(PulseBlasterESR-PRO)
endif()

if(COMPILE_PBESRPROII)
add_subdirectory(PulseBlasterESR-PRO-II)
endif()

if(COMPILE_PBQUAD)
add_subdirectory(PulseBlaster-QuadCore)
endif()

if(COMPILE_RP)
add_subdirectory(RadioProcessor)
endif()

if(COMPILE_PTS)
add_subdirectory(USB-PTS)
endif()

install(FILES Shared/spinapi_examples_common.h
        DESTINATION examples/Shared/)
