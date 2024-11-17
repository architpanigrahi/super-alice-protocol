# Create the library from source files
add_library(alice
        node.cpp
        packet.cpp
        router.cpp
        peer.cpp
        logger.cpp
        device_meta/device_ip_table.cpp
        device_meta/eci_position_calculator.cpp
        device_meta/position_table.cpp
        position_service/position_notifier.cpp
        encryption_manager.cpp
#        error_handler.cpp
)

# Link the include directory for public headers
target_include_directories(alice
        PUBLIC
        ${OPENSSL_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        ${asio_SOURCE_DIR}/asio/include
        $<INSTALL_INTERFACE:include>
)

# Set properties for the library target (e.g., versioning)
set_target_properties(alice PROPERTIES VERSION ${PROJECT_VERSION})