project(gridtools-verification CXX C)
cmake_minimum_required(VERSION 3.3)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(ExternalProject)

#---------------------------------------- CMake options --------------------------------------------
# C++11 compiler support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT(COMPILER_SUPPORTS_CXX11))
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif(NOT(COMPILER_SUPPORTS_CXX11))
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Do we use terminal colors?
set(GT_VERIFICATION_USE_COLOR "ON" CACHE BOOL "Enable colored terminal output")
if(NOT(GT_VERIFICATION_USE_COLOR))
    add_definitions(-DDYCORE_NO_COLOR)
endif()

# Set build type to Release if nothing was specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
    "Choose the type of build, options are: Debug Release" FORCE)
    message(STATUS "Setting build type to 'Release' as none was specified")
endif(NOT CMAKE_BUILD_TYPE)

# GTest has to use their own TR1 tupples on Mac OSX when compiling with Clang or ICC
if(APPLE AND NOT(CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
endif()

# Adjust compile flags for Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

#---------------------------------------- Boost ----------------------------------------------------
find_package(Boost 1.58 COMPONENTS system program_options REQUIRED)

if(NOT(${Boost_FOUND}))
    message(FATAL_ERROR "Boost not found or insufficient version.")
endif(NOT(${Boost_FOUND}))

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#---------------------------------------- Serialbox ------------------------------------------------
# If SERIALBOX_ROOT was not manually provided, we will search in external/serialbox for Serialbox 
# (we check if there is a CMakeLists.txt).
set(SERIALBOX_ROOT_ENV "$ENV{SERIALBOX_ROOT}")
if(NOT(SERIALBOX_ROOT) AND NOT(SERIALBOX_ROOT_ENV) AND 
   EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../serialbox/CMakeLists.txt")
    # Set the include directory
    set(SERIALBOX_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/serialbox/include")
else()
    find_package(Serialbox QUIET REQUIRED)
endif()

include_directories(SYSTEM ${SERIALBOX_INCLUDE_DIRS})

# Glob the headers (for IDEs)
file(GLOB_RECURSE GT_VERIFICATION_HEADERS 
     ${CMAKE_CURRENT_SOURCE_DIR}/src
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE GT_VERIFICATION_UNITTEST_HEADERS 
     ${CMAKE_CURRENT_SOURCE_DIR}/unittest
     ${CMAKE_CURRENT_SOURCE_DIR}/unittest/*.h)

# TODO find_gtest!
include_directories(SYSTEM ${GTEST_ROOT} )


# build verification framework
include_directories( src )
file(GLOB_RECURSE GT_VERIFICATION_SOURCES "src/*.cpp")

add_library( gt_verification ${GT_VERIFICATION_SOURCES} )
target_link_libraries( gt_verification ${Boost_LIBRARIES} )

#add_subdirectory(unittest)
