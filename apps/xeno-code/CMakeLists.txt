qt6_standard_project_setup()

add_executable(xeno-code
    src/main.cpp
)

target_link_libraries(xeno-code
    Qt6::Core
    Qt6::Widgets
    ai-integration
    utils
)

# Enable automatic MOC for Qt
set_target_properties(xeno-code PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Set compile features
target_compile_features(xeno-code PRIVATE cxx_std_20)

# Install target
install(TARGETS xeno-code
    DESTINATION bin
)