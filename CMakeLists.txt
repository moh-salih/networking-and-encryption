cmake_minimum_required(VERSION 3.25)

# Project setup
project(secureSocket VERSION 1.0.0 LANGUAGES CXX)

# Compiler configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(ENABLE_CATCH_TESTS "Build Catch2-based tests" ON)
option(BUILD_CRYPTO_APP "Build Crypto Qt Application" ON)
option(BUILD_NET_CLI "Build CLI Server/Client Application" ON)
option(BUILD_NET_GUI "Build GUI Server/Client Application" ON)


# Build Subdirectories
add_subdirectory(src/crypto)
add_subdirectory(src/net)
add_subdirectory(app)


# Testing configuration
if (ENABLE_CATCH_TESTS)
    enable_testing()
    add_subdirectory(tests/catch2)
endif()

