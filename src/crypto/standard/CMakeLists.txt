file(GLOB_RECURSE standard_CIPHERS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(standard_ciphers
    ${standard_CIPHERS_SOURCES}
)


find_package(OpenSSL REQUIRED)


target_link_libraries(standard_ciphers PRIVATE
    crypto_core
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(standard_ciphers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)

