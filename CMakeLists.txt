cmake_minimum_required(VERSION 3.29)
project(AliceProtocol VERSION 1.0 LANGUAGES C CXX)

# Setting Language Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Boost without using the FindBoost module (avoids CMP0167)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Add Library and Source Directories
add_subdirectory(src)
include_directories(include test)

# Install targets
install(TARGETS alice
        EXPORT aliceTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/alice DESTINATION include)
export(EXPORT aliceTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/aliceTargets.cmake" NAMESPACE alice::)

# Configure package versioning
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

# Main executable
# add_executable(alice_simulation src/main.cpp)
# target_link_libraries(alice_simulation PRIVATE ${Boost_LIBRARIES} alice)  # Use PRIVATE for keyword syntax
# target_include_directories(alice_simulation PRIVATE include)  # Include header files

# Test sources
set(TEST_SOURCES
    test/test_main.cpp
    test/test_example.cpp
    test/test_packet.cpp
)

# Test executable
add_executable(all_tests ${TEST_SOURCES})
target_link_libraries(all_tests PRIVATE alice ${Boost_LIBRARIES})  # Use PRIVATE for keyword syntax

# Enable and add tests
enable_testing()
add_test(NAME BoostTests COMMAND all_tests)
