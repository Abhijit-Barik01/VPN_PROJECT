cmake_minimum_required(VERSION 3.20)
project(VPN_System VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OPENSSL_INCLUDE_DIR})

# Sources
file(GLOB_RECURSE CRYPTO_SOURCES "src/crypto/*.cpp")
file(GLOB_RECURSE TUN_SOURCES "src/tun/*.cpp")
file(GLOB_RECURSE PROTOCOL_SOURCES "src/protocol/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp")

# Common Library
add_library(vpn_common STATIC
    ${CRYPTO_SOURCES}
    ${TUN_SOURCES}
    ${PROTOCOL_SOURCES}
    ${UTILS_SOURCES}
)
target_link_libraries(vpn_common PRIVATE OpenSSL::SSL OpenSSL::Crypto Threads::Threads ws2_32)

# Server Executable
add_executable(vpn_server src/server/main.cpp)
target_link_libraries(vpn_server PRIVATE vpn_common)

# Client Executable
add_executable(vpn_client src/client/main.cpp)
target_link_libraries(vpn_client PRIVATE vpn_common)

# Copy wintun.dll to bin directory (Placeholder command, user needs to provide DLL)
# add_custom_command(TARGET vpn_client POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${CMAKE_SOURCE_DIR}/vendor/wintun.dll"
#     "$<TARGET_FILE_DIR:vpn_client>"
# )
