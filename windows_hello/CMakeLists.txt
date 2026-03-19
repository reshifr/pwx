cmake_minimum_required(VERSION 3.15)
project("windows_hello" CXX)

include(ExternalProject)
include(FetchContent)

# Target Platfrom Configurations
if(NOT TARGET_PLATFORM)
  set(TARGET_PLATFORM "x64" CACHE STRING "Target architecture (x64, Win32, ARM64)")
  message(STATUS "No Platform specified, defaulting to: ${TARGET_PLATFORM}")
else()
  message(STATUS "Target Platform set to: ${TARGET_PLATFORM}")
endif()

# C/C++ Configurations
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Build library: libsodium
# Public key: keys/frank-denis-libsodium.asc
fetchcontent_declare(
  libsodium_source
  URL https://github.com/jedisct1/libsodium/releases/download/1.0.21-RELEASE/libsodium-1.0.21.tar.gz
  URL_HASH SHA256=9e4285c7a419e82dedb0be63a72eea357d6943bc3e28e6735bf600dd4883feaf
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(libsodium_source)
fetchcontent_getproperties(libsodium_source SOURCE_DIR LIBSODIUM_SOURCE_DIR)
set(LIBSODIUM_INCLUDE_DIR "${LIBSODIUM_SOURCE_DIR}/src/libsodium/include")
set(LIBSODIUM_LIB_PATH "${LIBSODIUM_SOURCE_DIR}/bin/x64/Release/v145/static/libsodium.lib")
externalproject_add(
  libsodium
  SOURCE_DIR "${LIBSODIUM_SOURCE_DIR}"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND msbuild "${LIBSODIUM_SOURCE_DIR}/builds/msvc/vs2026/libsodium.sln" /p:Configuration=StaticRelease /p:Platform=${TARGET_PLATFORM}
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 1
)

# Build Library: json
# Public key: keys/niels-lohmann-json.asc
fetchcontent_declare(
  json_source
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(json_source)
fetchcontent_getproperties(json_source SOURCE_DIR JSON_SOURCE_DIR)
set(JSON_INCLUDE_DIR "${JSON_SOURCE_DIR}/include")

# Build Library: windows_hello
# Public key: -
set(WINDOWS_HELLO_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(WINDOWS_HELLO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
set(WINDOWS_HELLO_LIB_PATH "${CMAKE_SOURCE_DIR}/build/Release/${PROJECT_NAME}.lib")

message(STATUS "🫵😂 ${WINDOWS_HELLO_LIB_PATH}")

add_library(
  "${PROJECT_NAME}" STATIC
  "${WINDOWS_HELLO_SOURCE_DIR}/${PROJECT_NAME}.cpp"
)

add_dependencies(
  "${PROJECT_NAME}"
  libsodium
)

target_compile_definitions(
  "${PROJECT_NAME}" PRIVATE
  SECURITY_WIN32
  SODIUM_STATIC
)

target_include_directories(
  "${PROJECT_NAME}" PRIVATE
  "${LIBSODIUM_INCLUDE_DIR}"
  "${JSON_INCLUDE_DIR}"
  "${WINDOWS_HELLO_INCLUDE_DIR}"
)

# target_link_libraries(
#   "${PROJECT_NAME}" PRIVATE
#   "${LIBSODIUM_LIB_PATH}"
# )

# Exe
set(EXECUTABLE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(EXECUTABLE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")

add_executable(
  "main"
  "${EXECUTABLE_SOURCE_DIR}/main.cpp"
)

target_compile_definitions(
  "main" PRIVATE
  RENOL
)

target_include_directories(
  "main" PRIVATE
  "${EXECUTABLE_INCLUDE_DIR}"
)

target_link_libraries(
  "main" PRIVATE
  # User32.Lib
  # Secur32.Lib
  # AdvAPI32.Lib
  webauthn.lib
  # crypt32.lib
  "${LIBSODIUM_LIB_PATH}"
  "${WINDOWS_HELLO_LIB_PATH}"
)
