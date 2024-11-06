# Create the library from source files
add_library(alice
        node.cpp
        packet.cpp
        router.cpp
#        error_handler.cpp
)

# Link the include directory for public headers
target_include_directories(alice
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set properties for the library target (e.g., versioning)
set_target_properties(alice PROPERTIES VERSION ${PROJECT_VERSION})