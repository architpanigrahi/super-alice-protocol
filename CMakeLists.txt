cmake_minimum_required(VERSION 3.29)
project(AliceProtocol VERSION 1.0 LANGUAGES C CXX)

# Setting Language Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add Library and Source Directories
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)
install(TARGETS alice
        EXPORT aliceTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/alice DESTINATION include)
export(EXPORT aliceTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/aliceTargets.cmake" NAMESPACE alice::)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/AliceProtocolConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)


configure_file(cmake/AliceProtocolConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/AliceProtocolConfig.cmake" @ONLY
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/AliceProtocolConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/AliceProtocolConfigVersion.cmake"
        DESTINATION lib/cmake/AliceProtocol
)

# Export the package for immediate use in the build tree
export(PACKAGE AliceProtocol)

add_executable(alice_simulation src/main.cpp)
target_link_libraries(alice_simulation PRIVATE alice)  # Link to your alice library
target_include_directories(alice_simulation PRIVATE include)  # Include header files

