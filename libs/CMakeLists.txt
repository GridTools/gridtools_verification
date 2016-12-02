#---------------------------------------- googletest ------------------------------------------------
set(GTEST_INSTALL_DIR "${CMAKE_BINARY_DIR}/libs/googletest/install")

set(CMAKE_EXTERNAL_ARGUMENTS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
                             -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                             -Wno-dev
                             -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

# Build googletest
ExternalProject_Add(googletest
    URL             "${CMAKE_CURRENT_SOURCE_DIR}/googletest"
    PREFIX          "googletest"
    CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS}
)
message(STATUS "Building googletest")

link_directories(${GTEST_INSTALL_DIR}/lib)

enable_testing()
set( GTEST_INCLUDE_DIRS "${GTEST_INSTALL_DIR}/include" PARENT_SCOPE )
set( GTEST_LIBRARIES "gtest" PARENT_SCOPE )
set( GTEST_MAIN_LIBRARIES "gtest-main" PARENT_SCOPE )
set( GTEST_BOTH_LIBRARIES "${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES}" PARENT_SCOPE )

