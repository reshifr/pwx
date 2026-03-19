cmake_minimum_required(VERSION 3.15)
project(
  "winhello"
  VERSION 1.0.0
  DESCRIPTION "Windows Hello Component for PwX"
  HOMEPAGE_URL "https://github.com/reshifr/pwx"
)

include(ExternalProject)
include(FetchContent)

# Target Platform Configurations
if(CMAKE_VS_PLATFORM_NAME STREQUAL "x64")
  set(TARGET_ARCH "/arch:AVX2")
elseif(CMAKE_VS_PLATFORM_NAME STREQUAL "ARM64")
else()
  message(FATAL_ERROR "Target Platform unsupported")
endif()

# C/C++ Configurations
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Build Library: libsodium
# Public Key: frank-denis-libsodium.asc
fetchcontent_declare(
  libsodium_src
  URL https://github.com/jedisct1/libsodium/releases/download/1.0.21-RELEASE/libsodium-1.0.21.tar.gz
  URL_HASH SHA256=9e4285c7a419e82dedb0be63a72eea357d6943bc3e28e6735bf600dd4883feaf
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(libsodium_src)
fetchcontent_getproperties(libsodium_src SOURCE_DIR LIBSODIUM_SRC_DIR)
set(LIBSODIUM_INCLUDE_DIR "${LIBSODIUM_SRC_DIR}/src/libsodium/include")
set(LIBSODIUM_LIB "${LIBSODIUM_SRC_DIR}/bin/x64/Release/v145/static/libsodium.lib")
externalproject_add(
  libsodium
  SOURCE_DIR "${LIBSODIUM_SRC_DIR}"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND msbuild "${LIBSODIUM_SRC_DIR}/builds/msvc/vs2026/libsodium.sln" /p:Configuration=StaticRelease /p:Platform=x64
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 1
)

# Build Library: json
# Public Key: niels-lohmann-json.asc
fetchcontent_declare(
  json_src
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
fetchcontent_makeavailable(json_src)
fetchcontent_getproperties(json_src SOURCE_DIR JSON_SRC_DIR)
set(JSON_INCLUDE_DIR "${JSON_SRC_DIR}/include")

# Build Library: winhello
# Public Key: -
set(WINHELLO_SRC_DIR "${CMAKE_SOURCE_DIR}")
set(WINHELLO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
set(WINHELLO_LIB "${CMAKE_SOURCE_DIR}/build/$<CONFIG>/${PROJECT_NAME}.lib")
add_library("${PROJECT_NAME}" SHARED "${WINHELLO_SRC_DIR}/${PROJECT_NAME}.cpp")
add_dependencies("${PROJECT_NAME}" libsodium)
target_compile_definitions("${PROJECT_NAME}" PRIVATE SODIUM_STATIC)
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Release>:
  /O2
  ${TARGET_ARCH}
  >
)
target_include_directories("${PROJECT_NAME}" PRIVATE "${LIBSODIUM_INCLUDE_DIR}" "${JSON_INCLUDE_DIR}" "${WINHELLO_INCLUDE_DIR}")
target_link_libraries("${PROJECT_NAME}" PRIVATE "webauthn.lib" "${LIBSODIUM_LIB}")

# Build Library: main
# Public Key: -
set(MAIN_SRC_DIR "${CMAKE_SOURCE_DIR}")
set(MAIN_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
add_executable("main" "${MAIN_SRC_DIR}/main.cpp")
add_dependencies("main" "${PROJECT_NAME}")
target_compile_options("main" PRIVATE
  $<$<CONFIG:Release>:
  /O2
  ${TARGET_ARCH}
  >
)
target_include_directories("main" PRIVATE "${MAIN_INCLUDE_DIR}")
target_link_libraries("main" PRIVATE "${PROJECT_NAME}")
