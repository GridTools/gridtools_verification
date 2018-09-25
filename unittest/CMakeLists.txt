function(add_unittest_executable name sources)
    add_executable(${name} ${sources})
    target_link_libraries(${name} 
            gridtools_verification
        )
endfunction(add_unittest_executable)

set(GT_VERIFICATION_TESTS
        "core/test_utility.cpp"
        "verification/test_error_metric.cpp"
        "verification/test_verification.cpp"
        "helper_dycore.h"
        "test_serialization.cpp"
        )

if( GRIDTOOLS_ROOT )
  message( STATUS "GridTools is available: ${GRIDTOOLS_ROOT}/include" )
  include_directories( ${GRIDTOOLS_ROOT}/include ) 
  add_definitions(-DHAS_GRIDTOOLS)
else()
  message( FATAL_ERROR "No GridTools found!" )
endif()

add_unittest_executable( test_verification "${GT_VERIFICATION_TESTS}" )
