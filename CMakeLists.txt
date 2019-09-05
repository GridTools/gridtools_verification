cmake_minimum_required(VERSION 3.9.0)
# set the installation prefix
# Note: setting this *before* the project imposes a default that the user is
# able to change.
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/" CACHE PATH "installation prefix")

project(gridtools-verification VERSION 0.14.0 LANGUAGES CXX C)

set(gridtools-verification_VERSION_STRING 
    "${gridtools-verification_VERSION_MAJOR}.${gridtools-verification_VERSION_MINOR}.${gridtools-verification_VERSION_PATCH}")
set(SERIALBOX_VERSION_REQUIRED "2.5.3")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------- CMake options
# C++11 compiler support
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

#----------------- find Serialbox2
find_package( Serialbox "${SERIALBOX_VERSION_REQUIRED}" REQUIRED HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../serialbox2/install/cmake"
    "${SERIALBOX_ROOT}/cmake"
    "${SERIALBOX_DIR}/cmake"
    "$ENV{SERIALBOX_ROOT}/cmake")

#----------------- find boost
set( Boost_NO_SYSTEM_PATHS ON )
set( BOOST_INCLUDEDIR "${SERIALBOX_BOOST_INCLUDE_DIRS}" ) 
set( REQUIRED_BOOST_COMPONENTS ${SERIALBOX_REQUIRED_BOOST_COMPONENTS} program_options system)
find_package( Boost ${SERIALBOX_BOOST_VERSION} REQUIRED ${REQUIRED_BOOST_COMPONENTS})

#----------------- build Google Test
add_subdirectory(libs/googletest)
include(FindThreadsCudaFix)
find_threads_cuda_fix()

#----------------- build verification framework
add_subdirectory(src)

#----------------- install verification framework
install( DIRECTORY src/ DESTINATION "include" FILES_MATCHING PATTERN "*.h" )

#----------------- packaging
include(cmake/Packaging.cmake)

add_subdirectory(unittest)
