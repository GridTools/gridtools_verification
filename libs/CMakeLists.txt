#---------------------------------------- googletest ------------------------------------------------
set(GTEST_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

set(CMAKE_EXTERNAL_ARGUMENTS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
                             -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                             -Wno-dev
                             -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

ExternalProject_Add(googletest
    URL             "${CMAKE_CURRENT_SOURCE_DIR}/googletest"
    PREFIX          "googletest"
    CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS}
)
message(STATUS "Building googletest")

link_directories(${GTEST_INSTALL_DIR}/lib)

enable_testing()
set( GTEST_INCLUDE_DIRS "${GTEST_INSTALL_DIR}/include" PARENT_SCOPE )
set( GTEST_LIBRARIES ${GTEST_INSTALL_DIR}/lib/libgtest.a ${GTEST_INSTALL_DIR}/lib/libgmock.a PARENT_SCOPE )
set( GTEST_MAIN_LIBRARIES ${GTEST_INSTALL_DIR}/lib/libgtest_main.a ${GTEST_INSTALL_DIR}/lib/libgmock_main.a PARENT_SCOPE )
set( GTEST_BOTH_LIBRARIES ${GTEST_INSTALL_DIR}/lib/libgtest.a ${GTEST_INSTALL_DIR}/lib/libgmock.a ${GTEST_INSTALL_DIR}/lib/libgtest_main.a ${GTEST_INSTALL_DIR}/lib/libgmock_main.a pthread PARENT_SCOPE )

#---------------------------------------- serialbox ------------------------------------------------
set(SERIALBOX_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

set(CMAKE_EXTERNAL_ARGUMENTS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
                             -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                             -Wno-dev
                             -DCMAKE_INSTALL_PREFIX=${SERIALBOX_INSTALL_DIR}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

ExternalProject_Add(serialbox
    URL             "${CMAKE_CURRENT_SOURCE_DIR}/serialbox"
    PREFIX          "serialbox"
    CMAKE_ARGS      ${CMAKE_EXTERNAL_ARGUMENTS}
)
message(STATUS "Building serialbox")

link_directories(${SERIALBOX_INSTALL_DIR}/lib)

set( SERIALBOX_INCLUDE_DIRS "${SERIALBOX_INSTALL_DIR}/include" PARENT_SCOPE )

set(SERIALBOX_LIBRARIES ${SERIALBOX_INSTALL_DIR}/lib/libSerialbox.a
    ${SERIALBOX_INSTALL_DIR}/lib/libUtils.a
    ${SERIALBOX_INSTALL_DIR}/lib/libjson.a
    ${SERIALBOX_INSTALL_DIR}/lib/libsha256.a PARENT_SCOPE)
    
