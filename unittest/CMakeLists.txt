
SET( UNITTEST_VERIFICATION_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/SerializationUnittest.cpp)
add_subdirectory( benchmark )
add_subdirectory( core )
add_subdirectory( verification )
add_unittest_executable( UnittestVerification "${UNITTEST_VERIFICATION_SRCS}" )