add_library(crypto_core STATIC
    utils.cpp
)

# This ensures anyone linking to crypto_core can find the headers
target_include_directories(crypto_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)