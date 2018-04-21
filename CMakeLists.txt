cmake_minimum_required(VERSION 3.2)

set(PROJECT_NAME Besouro)
set(EXECUTABLE_NAME besouro)

project("${PROJECT_NAME}" CXX)

##------------------------------------------------------------------------------
## Initialization
##

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Verboseness
#SET(CMAKE_SHARED_LINKER_FLAGS "-v")
#SET(CMAKE_MODULE_LINKER_FLAGS "-v")
#SET(CMAKE_EXE_LINKER_FLAGS "-v")
##SET(GCC_LINK_FLAGS "-v")
#add_definitions("-DVERBOSE")

# Current state
if (WIN32)
  message(STATUS "WIN32")
endif ()
if (MSVC)
  message(STATUS "MSVC")
endif ()
if (APPLE)
  message(STATUS "APPLE")
endif ()

message(STATUS "Using C: " ${CMAKE_C_COMPILER_ID})
message(STATUS "Using CXX: " ${CMAKE_CXX_COMPILER_ID})

# Set high warning levels
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wno-long-long -pedantic")
endif ()

# Make $HOME compatible with windows
STRING(REGEX REPLACE "\\\\" "/" ENV_HOME_DIR "$ENV{HOME}")

# Add custom modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/findcmake")

# Set base directories
set(CPP_SRC_DIR "src")
set(LIB_DIR "lib")

# Add manual libs
list(APPEND INCLUDE_DIRS "include")

##------------------------------------------------------------------------------
## Dependencies
##

add_subdirectory("${LIB_DIR}/fmt")
list(APPEND INCLUDE_DIRS "${LIB_DIR}/fmt")
list(APPEND LIBRARIES fmt::fmt-header-only)

##------------------------------------------------------------------------------
## Sources
##

list(APPEND SOURCES
    ${CPP_SRC_DIR}/main.cpp
    ${CPP_SRC_DIR}/debugger.cpp
    ${CPP_SRC_DIR}/breakpoint.cpp
    )

 list(APPEND HEADERS
     ${CPP_SRC_DIR}/debugger.hpp
     ${CPP_SRC_DIR}/breakpoint.hpp
     ${CPP_SRC_DIR}/registers.hpp
     )

list(APPEND SOURCES ${HEADERS})

##------------------------------------------------------------------------------
## Targets
##

# Create project
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${CPP_SRC_DIR}/main.cpp)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_LANGUAGE_STANDARD 17 C_STANDARD 11)

# Organize the source
source_group("Headers" FILES ${HEADERS})

# Link with FIND_PACKAGE
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARIES})

# Add manual includes
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${INCLUDE_DIRS})

