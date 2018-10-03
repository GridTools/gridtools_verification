set(GT_VERIFICATION_SOURCES
    "core/color.cpp"
    "core/color.h"
    "core/command_line.cpp"
    "core/command_line.h"
    "core/error.h"
    "core/include_boost_format.h"
    "core/logger.cpp"
    "core/logger.h"
    "core/serialization.h"
    "core/type_erased_field.h"
    "core/utility.cpp"
    "core/utility.h"
    "verification/boundary_extent.h"
    "verification/error_metric_interface.h"
    "verification/error_metric.h"
    "verification/field_collection.cpp"
    "verification/field_collection.h"
    "verification/main.h"
    "verification/unittest_environment.cpp"
    "verification/unittest_environment.h"
    "verification/verification_reporter.cpp"
    "verification/verification_reporter.h"
    "verification/verification_result.h"
    "verification/verification_specification.cpp"
    "verification/verification_specification.h"
    "verification/verification.h"
    "common.h"
    "core.h"
    "verification_exception.h"
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
