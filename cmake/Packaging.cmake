# this registers the build-tree with a global CMake-registry
export(PACKAGE gridtools_verification)

## Handle Packaging
include(CMakePackageConfigHelpers)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/FindThreadsCudaFix.cmake" DESTINATION cmake)
set(GTest_DIR ${CMAKE_INSTALL_PREFIX}/lib64/cmake)

## For install tree
# Generate and install GridtoolsVerificationConfig.cmake
set(PACKAGE_gridtools_verification_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/cmake)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/gridtools_verification-config.cmake.in
    ${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config.cmake
    INSTALL_DESTINATION
        cmake
    PATH_VARS
        Serialbox_DIR
        GTest_DIR
        PACKAGE_gridtools_verification_CMAKE_DIR
)
install(FILES "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config.cmake" DESTINATION cmake)

# Generate and install GridtoolsVerificationConfigVersion.cmake
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config-version.cmake"
  VERSION ${gridtools-verification_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion
)
install(FILES "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config-version.cmake" DESTINATION cmake)
install(EXPORT gridtools_verificationTargets DESTINATION cmake NAMESPACE gridtools_verification::)

## For build tree
set(PACKAGE_gridtools_verification_CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/gridtools_verification-config.cmake.in
    ${PROJECT_BINARY_DIR}/gridtools_verification-config.cmake
    INSTALL_DESTINATION
        ${PROJECT_BINARY_DIR}
    PATH_VARS
        Serialbox_DIR
        GTest_DIR
        PACKAGE_gridtools_verification_CMAKE_DIR
)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/gridtools_verification-config-version.cmake"
  VERSION ${gridtools-verification_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion
)
export(TARGETS gridtools_verification gtest_main gmock_main gtest gmock FILE ${PROJECT_BINARY_DIR}/gridtools_verificationTargets.cmake NAMESPACE gridtools_verification::)
