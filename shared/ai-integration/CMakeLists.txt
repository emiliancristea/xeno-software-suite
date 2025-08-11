add_library(ai-integration
    src/ai_integration.cpp
)

target_include_directories(ai-integration
    PUBLIC include
)

# Always link nlohmann-json as it's available on system
target_compile_definitions(ai-integration PRIVATE HAVE_NLOHMANN_JSON)

# Link dependencies conditionally
if(httplib_FOUND)
    target_link_libraries(ai-integration httplib::httplib)
    target_compile_definitions(ai-integration PRIVATE HAVE_HTTPLIB)
elseif(HTTPLIB_FOUND)
    target_link_libraries(ai-integration ${HTTPLIB_LIBRARIES})
    target_include_directories(ai-integration PRIVATE ${HTTPLIB_INCLUDE_DIRS})
    target_compile_definitions(ai-integration PRIVATE HAVE_HTTPLIB)
endif()

# Set compile features
target_compile_features(ai-integration PUBLIC cxx_std_20)