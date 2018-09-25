set(GT_VERIFICATION_TESTS
        "core/test_utility.cpp"
        "verification/test_error_metric.cpp"
        "verification/test_verification.cpp"
        "helper_dycore.h"
        "test_serialization.cpp"
        )

add_executable(test_verification "${GT_VERIFICATION_TESTS}")
target_link_libraries(test_verification gridtools_verification)

if( GRIDTOOLS_ROOT )
    # GridTools is only need for some tests
    message( STATUS "GridTools is available: ${GRIDTOOLS_ROOT}/include" )
    target_include_directories(test_verification PRIVATE ${GRIDTOOLS_ROOT}/include)
    target_compile_definitions(test_verification PRIVATE HAS_GRIDTOOLS)
endif()
