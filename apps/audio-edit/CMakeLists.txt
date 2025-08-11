add_executable(xeno-audio-edit
    src/main.cpp
)

target_link_libraries(xeno-audio-edit
    Qt6::Core
    Qt6::Widgets
    ai-integration
    utils
)

# Enable automatic MOC for Qt
set_target_properties(xeno-audio-edit PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Set compile features
target_compile_features(xeno-audio-edit PRIVATE cxx_std_20)

# Install target
install(TARGETS xeno-audio-edit
    DESTINATION bin
)