cmake_minimum_required(VERSION 3.20)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Dependencies
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Stb REQUIRED)


# Supres boost warnings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()




if(BUILD_NET_GUI)
# Server GUI -----------------------------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_GUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/server/gui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/gui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)

add_executable(server-gui ${SERVER_GUI_SOURCES})

if(WIN32)
    if(MSVC)
        target_link_options(server-gui PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    endif()
endif()


target_include_directories(server-gui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../.." ${Stb_INCLUDE_DIR})
target_link_libraries(server-gui PRIVATE imgui::imgui glfw GLEW::GLEW net)


# Client Gui -----------------------------------------------------------------------------------------------
file(GLOB_RECURSE CLIENT_GUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/client/gui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client/gui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/client/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client/common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)


add_executable(client-gui ${CLIENT_GUI_SOURCES})

if(WIN32)
    if(MSVC)
        target_link_options(client-gui PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    endif()
endif()




target_include_directories(client-gui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../.." ${Stb_INCLUDE_DIR})
target_link_libraries(client-gui PRIVATE imgui::imgui glfw GLEW::GLEW net)

endif()

if(BUILD_NET_CLI)
    # Server CLI -----------------------------------------------------------------------------------------------
    file(GLOB_RECURSE SERVER_CLI_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/server/cli/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/server/cli/*.h"    
        "${CMAKE_CURRENT_SOURCE_DIR}/server/common/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/server/common/*.h"
    )

    add_executable(server-cli ${SERVER_CLI_SOURCES})


    target_include_directories(server-cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../.." ${Stb_INCLUDE_DIR})
    target_link_libraries(server-cli PRIVATE imgui::imgui glfw GLEW::GLEW net)


    # Client Cli -----------------------------------------------------------------------------------------------
    file(GLOB_RECURSE CLIENT_CLI_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/client/cli/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/client/cli/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/client/common/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/client/common/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
    )


    add_executable(client-cli ${CLIENT_CLI_SOURCES})



    target_include_directories(client-cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../.." ${Stb_INCLUDE_DIR})
    target_link_libraries(client-cli PRIVATE imgui::imgui glfw GLEW::GLEW net)
endif()