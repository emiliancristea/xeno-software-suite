add_library(ai-integration
    src/ai_integration.cpp
)

target_include_directories(ai-integration
    PUBLIC include
)

target_link_libraries(ai-integration
    nlohmann_json::nlohmann_json
    httplib::httplib
)

# Set compile features
target_compile_features(ai-integration PUBLIC cxx_std_20)