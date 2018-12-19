set(GT_VERIFICATION_SOURCES
    "gridtools_verification/core/color.cpp"
    "gridtools_verification/core/color.h"
    "gridtools_verification/core/command_line.cpp"
    "gridtools_verification/core/command_line.h"
    "gridtools_verification/core/error.h"
    "gridtools_verification/core/include_boost_format.h"
    "gridtools_verification/core/logger.cpp"
    "gridtools_verification/core/logger.h"
    "gridtools_verification/core/serialization.h"
    "gridtools_verification/core/type_erased_field.h"
    "gridtools_verification/core/utility.cpp"
    "gridtools_verification/core/utility.h"
    "gridtools_verification/verification/boundary_extent.h"
    "gridtools_verification/verification/error_metric_interface.h"
    "gridtools_verification/verification/error_metric.h"
    "gridtools_verification/verification/field_collection.h"
    "gridtools_verification/verification/main.h"
    "gridtools_verification/verification/unittest_environment.cpp"
    "gridtools_verification/verification/unittest_environment.h"
    "gridtools_verification/verification/verification_reporter.cpp"
    "gridtools_verification/verification/verification_reporter.h"
    "gridtools_verification/verification/verification_result.h"
    "gridtools_verification/verification/verification_specification.cpp"
    "gridtools_verification/verification/verification_specification.h"
    "gridtools_verification/verification/verification.h"
    "gridtools_verification/common.h"
    "gridtools_verification/core.h"
    "gridtools_verification/verification_exception.h"
    )

add_library( gridtools_verification ${GT_VERIFICATION_SOURCES} )
target_include_directories( gridtools_verification 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
    
target_link_libraries( gridtools_verification PRIVATE Boost::program_options)
target_link_libraries( gridtools_verification PRIVATE Boost::system)

target_link_libraries( gridtools_verification PUBLIC gtest_main gmock_main )
target_link_libraries( gridtools_verification PUBLIC Serialbox::SerialboxStatic )

install( TARGETS gridtools_verification DESTINATION "lib" EXPORT gridtools_verificationTargets )
