add_executable(test_integration
    test_integration.cpp
)

target_link_libraries(test_integration
    GTest::gtest
    GTest::gtest_main
)

target_compile_features(test_integration PRIVATE cxx_std_20)

# Add test to CTest
add_test(NAME IntegrationTest COMMAND test_integration)