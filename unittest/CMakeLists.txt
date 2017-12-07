function(add_unittest_executable name sources)
    add_executable(${name} ${sources} ${GT_VERIFICATION_HEADERS})
    
    target_link_libraries(${name} 
            gridtools_verification
            ${GTEST_BOTH_LIBRARIES}
        )
    target_compile_definitions( ${name} "-DBOOST_SYSTEM_NO_DEPRECATED" )
endfunction(add_unittest_executable)

file(GLOB_RECURSE GT_VERIFICATION_TESTS "test_*.cpp")

add_unittest_executable( test_verification "${GT_VERIFICATION_TESTS}" )
