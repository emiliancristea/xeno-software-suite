add_library(utils
    src/utils.cpp
)

target_include_directories(utils
    PUBLIC include
)

# Set compile features
target_compile_features(utils PUBLIC cxx_std_20)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(utils shell32)
elseif(APPLE)
    target_link_libraries(utils "-framework CoreFoundation")
endif()