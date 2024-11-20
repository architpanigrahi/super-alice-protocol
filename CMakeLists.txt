cmake_minimum_required(VERSION 3.16)
project(AliceProtocol VERSION 1.0 LANGUAGES C CXX)

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

message(STATUS "Detecting operating system...")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Operating System: Linux")
    set(OPENSSL_ROOT_DIR /usr/bin/openssl)
    set(OPENSSL_INCLUDE_DIR /usr/include/openssl)
    set(OPENSSL_LIBRARIES ${OPENSSL_ROOT_DIR}/lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Operating System: macOS")
    set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)
    set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
    set(OPENSSL_LIBRARIES ${OPENSSL_ROOT_DIR}/lib)
else()
    message(FATAL_ERROR "Unsupported Operating System: ${CMAKE_SYSTEM_NAME}")
endif()



include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

find_package(OpenSSL REQUIRED)



include_directories(${asio_SOURCE_DIR}/asio/include)
add_subdirectory(src)
include_directories(include test)

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


add_executable(peer_bootstrap_node src/peer_bootstrap_node.cpp)
target_link_libraries(peer_bootstrap_node PRIVATE alice pthread ${Boost_LIBRARIES} OpenSSL::Crypto)  
add_executable(peer_satellite_node src/peer_satellite_node.cpp)
target_link_libraries(peer_satellite_node PRIVATE alice pthread ${Boost_LIBRARIES} OpenSSL::Crypto)
add_executable(peer_edge_node src/peer_edge_node.cpp)
target_link_libraries(peer_edge_node PRIVATE alice pthread ${Boost_LIBRARIES} OpenSSL::Crypto)
option(BUILD_TESTS "Build the tests" OFF)
if (BUILD_TESTS)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
set(TEST_SOURCES
    test/test_main.cpp
    src/peer.cpp
    src/peer_satellite_node.cpp
    test/test_example.cpp
    test/test_packet.cpp
    test/test_encryption_manager.cpp
    test/test_iptable.cpp
    test/test_discovery.cpp
)
add_executable(all_tests ${TEST_SOURCES})
target_compile_definitions(all_tests PRIVATE BUILD_TESTS)
target_link_libraries(all_tests PRIVATE alice ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
enable_testing()
add_test(NAME BoostTests COMMAND all_tests)
endif()