file(GLOB_RECURSE MODERN_CIPHERS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(modern_ciphers
    ${MODERN_CIPHERS_SOURCES}
)


target_link_libraries(modern_ciphers PUBLIC crypto_core)
target_include_directories(modern_ciphers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)

