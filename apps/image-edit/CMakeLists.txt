add_executable(xeno-image-edit
    src/main.cpp
)

target_link_libraries(xeno-image-edit
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_photo
    ai-integration
    utils
)

# Enable automatic MOC for Qt
set_target_properties(xeno-image-edit PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Set compile features
target_compile_features(xeno-image-edit PRIVATE cxx_std_20)

# Install target
install(TARGETS xeno-image-edit
    DESTINATION bin
)