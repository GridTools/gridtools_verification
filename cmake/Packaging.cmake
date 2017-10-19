## Handle Packaging
include(CMakePackageConfigHelpers)

set(GRIDTOOLS_VERIFICATION_INCLUDE_DIR include)
set(GRIDTOOLS_VERIFICATION_LIBRARY_DIR lib)

## Generate and install GridtoolsVerificationConfig.cmake
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/gridtools_verification-config.cmake.in
    ${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config.cmake
    INSTALL_DESTINATION
        cmake
    PATH_VARS
        SERIALBOX_ROOT
        GRIDTOOLS_VERIFICATION_INCLUDE_DIR
        GRIDTOOLS_VERIFICATION_LIBRARY_DIR
)
install(FILES "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config.cmake" DESTINATION cmake)

# Generate and install GridtoolsVerificationConfigVersion.cmake
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config-version.cmake"
  VERSION ${GRIDTOOLS_VERIFICATION_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion
)
install(FILES "${PROJECT_BINARY_DIR}/cmake/gridtools_verification-config-version.cmake" DESTINATION cmake)
