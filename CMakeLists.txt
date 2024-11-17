cmake_minimum_required(VERSION 3.29)
project(AliceProtocol VERSION 1.0 LANGUAGES C CXX)

add_compile_options(-Wno-deprecated-declarations)

include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-24-0
)
FetchContent_MakeAvailable(asio)

cmake_policy(SET CMP0167 NEW) # Suppress Boost policy warning

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(OpenSSL REQUIRED)

# Add source directories
add_subdirectory(src)

# Add tests
set(TEST_SOURCES
    test/test_main.cpp
    test/test_example.cpp
    test/test_packet.cpp
    test/test_encryption_manager.cpp
)

# Define executables
add_executable(peer_bootstrap_node src/peer_bootstrap_node.cpp)
target_link_libraries(peer_bootstrap_node PRIVATE alice ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

add_executable(peer_satellite_node src/peer_satellite_node.cpp)
target_link_libraries(peer_satellite_node PRIVATE alice ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

add_executable(peer_edge_node src/peer_edge_node.cpp)
target_link_libraries(peer_edge_node PRIVATE alice ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

add_executable(all_tests ${TEST_SOURCES})
target_link_libraries(all_tests PRIVATE alice ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)

enable_testing()
add_test(NAME BoostTests COMMAND all_tests)

# Install rules
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

export(PACKAGE AliceProtocol)