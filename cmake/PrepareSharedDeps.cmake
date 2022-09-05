include(ExternalProject)

# glad
add_library(${PRIV_SHARED_DEPS}_glad INTERFACE)

ExternalProject_Add(
  shared_glad
  URL "https://github.com/Dav1dde/glad/archive/refs/tags/v0.1.34.tar.gz"
  TIMEOUT 600
  URL_MD5 eea8f198923672b4be7a3c81e22076ac

  SOURCE_DIR ${CMAKE_BINARY_DIR}/.ext/glad/
  INSTALL_DIR ${PRIV_SHARED_DEPS_ROOT}/glad

  CONFIGURE_COMMAND cd <SOURCE_DIR> && mkdir -p build && mkdir -p <INSTALL_DIR> && cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DGLAD_INSTALL=ON -DGLAD_ALL_EXTENSIONS=OFF -DGLAD_REPRODUCIBLE=OFF -DGLAD_PROFILE="core" -DGLAD_API="gl=4.2" -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -GNinja
  BUILD_COMMAND cd <SOURCE_DIR>/build && ninja -j4
  INSTALL_COMMAND COMMAND cd <SOURCE_DIR>/build && ninja install
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PRIV_CREDITS_ROOT}/glad/
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/LICENSE ${PRIV_CREDITS_ROOT}/glad/
)
add_dependencies(${PRIV_SHARED_DEPS}_glad shared_glad)
target_include_directories(${PRIV_SHARED_DEPS}_glad
  INTERFACE ${PRIV_SHARED_DEPS_ROOT}/glad/include
)
target_link_directories(${PRIV_SHARED_DEPS}_glad
  INTERFACE ${PRIV_SHARED_DEPS_ROOT}/glad/lib
)
target_link_libraries(${PRIV_SHARED_DEPS}_glad
  INTERFACE glad
  INTERFACE dl # [XXX] dl should be placed after glad (for some old linkers)
)

# spdlog
add_library(${PRIV_SHARED_DEPS}_spdlog INTERFACE)

ExternalProject_Add(
  shared_spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.8.0.tar.gz"
  TIMEOUT 600
  URL_MD5 cbd179161d1ed185bd9f3f242c424fd7

  SOURCE_DIR ${CMAKE_BINARY_DIR}/.ext/spdlog/
  INSTALL_DIR ${PRIV_SHARED_DEPS_ROOT}/spdlog

  CONFIGURE_COMMAND cd <SOURCE_DIR> && mkdir -p build && mkdir -p <INSTALL_DIR> && cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DSPDLOG_ENABLE_PCH=ON -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -GNinja
  BUILD_COMMAND cd <SOURCE_DIR>/build && ninja -j4
  INSTALL_COMMAND COMMAND cd <SOURCE_DIR>/build && ninja install
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PRIV_CREDITS_ROOT}/spdlog/
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/LICENSE ${PRIV_CREDITS_ROOT}/spdlog/
)
add_dependencies(${PRIV_SHARED_DEPS}_spdlog shared_spdlog)
target_compile_definitions(${PRIV_SHARED_DEPS}_spdlog
  INTERFACE "SPDLOG_COMPILED_LIB"
)
set_target_properties(${PRIV_SHARED_DEPS}_spdlog PROPERTIES
  POSITION_INDEPENDENT_CODE ON # force adding -fPIC
)
target_include_directories(${PRIV_SHARED_DEPS}_spdlog
  INTERFACE ${PRIV_SHARED_DEPS_ROOT}/spdlog/include
)
target_link_directories(${PRIV_SHARED_DEPS}_spdlog
  INTERFACE ${PRIV_SHARED_DEPS_ROOT}/spdlog/lib
)
target_link_libraries(${PRIV_SHARED_DEPS}_spdlog
  INTERFACE spdlog
)

# cli11 
add_library(${PRIV_SHARED_DEPS}_cli11 INTERFACE)

ExternalProject_Add(
  shared_cli11
  URL "https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.1.2.tar.gz"
  TIMEOUT 600
  URL_MD5 4261b86b4327b4ea29ce5ee61af83e33

  SOURCE_DIR ${CMAKE_BINARY_DIR}/.ext/cli11/
  INSTALL_DIR ${PRIV_SHARED_DEPS_ROOT}/cli11

  CONFIGURE_COMMAND cd <SOURCE_DIR> && mkdir -p build && mkdir -p <INSTALL_DIR> && cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DCLI11_SINGLE_FILE=OFF -DCLI11_SANITIZERS=OFF -DCLI11_BUILD_DOCS=OFF -DCLI11_BUILD_TESTS=OFF -DCLI11_BUILD_EXAMPLES=OFF -DCLI11_FORCE_LIBCXX=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -GNinja
  BUILD_COMMAND cd <SOURCE_DIR>/build && ninja -j4
  INSTALL_COMMAND COMMAND cd <SOURCE_DIR>/build && ninja install
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PRIV_CREDITS_ROOT}/cli11/
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/LICENSE ${PRIV_CREDITS_ROOT}/cli11/
)
add_dependencies(${PRIV_SHARED_DEPS}_cli11 shared_cli11)
target_include_directories(${PRIV_SHARED_DEPS}_cli11
  INTERFACE ${PRIV_SHARED_DEPS_ROOT}/cli11/include
)
# target_link_libraries(${PRIV_SHARED_DEPS}_cli11
#   INTERFACE cli11
# )
