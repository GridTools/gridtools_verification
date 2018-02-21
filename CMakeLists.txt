cmake_minimum_required(VERSION 3.3)
# set the installation prefix
# Note: setting this *before* the project imposes a default that the user is
# able to change.
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/" CACHE PATH "installation prefix")

project(gridtools-verification CXX C)


include(ExternalProject)

set(GRIDTOOLS_VERIFICATION_VERSION_STRING "0.4")
set(SERIALBOX_VERSION_REQUIRED "2.2.1")

#----------------- CMake options
# C++11 compiler support
## Set C++ standard to C++11
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

# Adjust compile flags for Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")


#----------------- find Serialbox2
find_package( Serialbox "${SERIALBOX_VERSION_REQUIRED}" REQUIRED HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../serialbox2/install/cmake"
    "${SERIALBOX_ROOT}/cmake"
    "${SERIALBOX_DIR}/cmake"
    "$ENV{SERIALBOX_ROOT}/cmake")
include_directories(${SERIALBOX_INCLUDE_DIRS})

#----------------- find boost
set( Boost_NO_SYSTEM_PATHS ON )
set( BOOST_INCLUDEDIR "${SERIALBOX_BOOST_INCLUDE_DIRS}" ) 
set( REQUIRED_BOOST_COMPONENTS ${SERIALBOX_REQUIRED_BOOST_COMPONENTS} program_options system)
find_package( Boost ${SERIALBOX_BOOST_VERSION} REQUIRED ${REQUIRED_BOOST_COMPONENTS})

#----------------- build bundled libs
add_subdirectory( libs )
include_directories(${GTEST_INCLUDE_DIRS})

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
add_dependencies( gridtools_verification googletest )
target_link_libraries( gridtools_verification ${Boost_LIBRARIES} )
target_link_libraries( gridtools_verification ${SERIALBOX_CXX_LIBRARIES} ${GTEST_BOTH_LIBRARIES} )
target_link_libraries( gridtools_verification pthread )

#----------------- install verification framework
install( TARGETS gridtools_verification DESTINATION "lib" EXPORT gridtools_verification )
install( DIRECTORY src/ DESTINATION "include/gridtools_verification" FILES_MATCHING PATTERN "*.h" )
install( EXPORT gridtools_verification DESTINATION lib/gridtools_verification )

#----------------- packaging
include(cmake/Packaging.cmake)

add_subdirectory(unittest)
