add_library(classic_ciphers
    Affine.cpp
    Caesar.cpp
    Playfair.cpp
    Vigenere.cpp
    Hill.cpp
)

target_link_libraries(classic_ciphers PUBLIC crypto_core)
target_include_directories(classic_ciphers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
