qt6_standard_project_setup()

add_executable(xeno-video-edit
    src/main.cpp
)

target_link_libraries(xeno-video-edit
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    ai-integration
    utils
)

# Enable automatic MOC for Qt
set_target_properties(xeno-video-edit PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Set compile features
target_compile_features(xeno-video-edit PRIVATE cxx_std_20)

# Install target
install(TARGETS xeno-video-edit
    DESTINATION bin
)