add_executable(test_shared_libraries
    test_shared_libraries.cpp
)

target_link_libraries(test_shared_libraries
    GTest::gtest
    GTest::gtest_main
    ai-integration
    utils
)

target_compile_features(test_shared_libraries PRIVATE cxx_std_20)

# Add test to CTest
add_test(NAME SharedLibrariesTest COMMAND test_shared_libraries)