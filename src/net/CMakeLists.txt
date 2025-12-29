
file(GLOB_RECURSE NET_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(net STATIC ${NET_SOURCES})


find_package(Boost REQUIRED COMPONENTS system thread asio filesystem)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED) 


target_include_directories(net PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${Boost_INCLUDE_DIRS} # Boost headers must be visible to targets linking this library
)


if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()



target_link_libraries(net
    PRIVATE
        Boost::system
        Boost::thread
        Boost::asio
        Boost::filesystem
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)



