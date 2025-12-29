
# Find dependencies
find_package(Catch2 3 REQUIRED)
find_package(OpenSSL REQUIRED)

# Define the single test executable with all test files
add_executable(crypto_unit_tests
    test_classic.cpp
    test_modern.cpp
    test_standard.cpp
    test_performance.cpp
)

# Link all cipher libraries and the shared core
target_link_libraries(crypto_unit_tests PRIVATE
    classic_ciphers
    modern_ciphers
    standard_ciphers
    crypto_core
    Catch2::Catch2WithMain
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Set include directories so tests can find the headers
target_include_directories(crypto_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Handle static linking for MinGW/Windows if enabled
if(ENABLE_STATIC_LINKING AND MINGW)
    target_link_options(crypto_unit_tests PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Enable CTest integration
include(CTest)
include(Catch)
catch_discover_tests(crypto_unit_tests)

