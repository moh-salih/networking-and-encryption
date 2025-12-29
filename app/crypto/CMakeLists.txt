set(CMAKE_AUTOMOC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)


qt_standard_project_setup()

qt_add_executable(crypto-gui WIN32
    main.cpp
    Window.cpp
    panels.cpp
)
qt_add_resources(crypto-gui
    PREFIX "/"
    BASE "res"
    FILES
        # Images
        "res/images/ktu_amblem_1a.png"
        "res/images/ktu_amblem_1b.png"
        "res/images/ktu_amblem_1c.png"

        # Styles
        "res/styles/style.qss"
)



target_link_libraries(crypto-gui PRIVATE
    Qt6::Widgets
    classic_ciphers
    modern_ciphers
    standard_ciphers
    crypto_core
)

target_include_directories(crypto-gui PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)



