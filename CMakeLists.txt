cmake_minimum_required(VERSION 3.3)
# set the installation prefix
# Note: setting this *before* the project imposes a default that the user is
# able to change.
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/" CACHE PATH "installation prefix")

project(gridtools-verification CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include(ExternalProject)

#----------------- CMake options
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

# Adjust compile flags for Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

#----------------- Boost
find_package(Boost 1.58 COMPONENTS system program_options REQUIRED)

if(NOT(${Boost_FOUND}))
    message(FATAL_ERROR "Boost not found or insufficient version.")
endif(NOT(${Boost_FOUND}))

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#----------------- build bundled libs
add_subdirectory( libs )
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${SERIALBOX_INCLUDE_DIRS})


#----------------- Glob the headers (for IDEs)
file(GLOB_RECURSE GT_VERIFICATION_HEADERS 
     ${CMAKE_CURRENT_SOURCE_DIR}/src
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE GT_VERIFICATION_UNITTEST_HEADERS 
     ${CMAKE_CURRENT_SOURCE_DIR}/unittest
     ${CMAKE_CURRENT_SOURCE_DIR}/unittest/*.h)

#----------------- build verification framework
include_directories( src )
file(GLOB_RECURSE GT_VERIFICATION_SOURCES "src/*.cpp")

if( GRIDTOOLS_ROOT )
  message( STATUS "GridTools is available: ${GRIDTOOLS_ROOT}/include" )
  include_directories( ${GRIDTOOLS_ROOT}/include )
  add_definitions(-DHAS_GRIDTOOLS)
endif()

add_library( gridtools_verification ${GT_VERIFICATION_SOURCES} )
add_dependencies( gridtools_verification googletest serialbox )
target_link_libraries( gridtools_verification ${Boost_LIBRARIES} )
target_link_libraries( gridtools_verification ${SERIALBOX_LIBRARIES} ${GTEST_BOTH_LIBRARIES} )
target_link_libraries( gridtools_verification pthread )

#----------------- install verification framework
install( TARGETS gridtools_verification DESTINATION "lib" EXPORT gridtools_verification )
install( DIRECTORY src/ DESTINATION "include/gridtools_verification" FILES_MATCHING PATTERN "*.h" )
install( EXPORT gridtools_verification DESTINATION lib/gridtools_verification )

add_subdirectory(unittest)
