add_executable(xeno-launcher
    src/main.cpp
)

target_link_libraries(xeno-launcher
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ai-integration
    utils
)

# Enable automatic MOC for Qt
set_target_properties(xeno-launcher PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Set compile features
target_compile_features(xeno-launcher PRIVATE cxx_std_20)

# Install target
install(TARGETS xeno-launcher
    DESTINATION bin
)