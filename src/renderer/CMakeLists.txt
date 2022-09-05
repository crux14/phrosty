project (phrosty-renderer C CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLM REQUIRED glm)

add_library(${PROJECT_NAME} STATIC
  renderer.cpp
  loader_glx.cpp
  x11trace_texture.cpp
  x11trace_pass.cpp
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE ${GLM_DEFINITIONS}
  PRIVATE "GLM_ENABLE_EXPERIMENTAL"
)

target_include_directories(${PROJECT_NAME}
  PRIVATE "../"
  PRIVATE ${GLM_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME}
  PRIVATE ${GLM_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Threads::Threads
  PRIVATE X11
  PRIVATE Xrender
  PRIVATE Xcomposite
  PRIVATE GL
  PRIVATE ${GLM_LIBRARIES}
  PRIVATE phrosty-core
  PUBLIC ${PRIV_SHARED_DEPS}_glad
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_precompile_headers(${PROJECT_NAME} PRIVATE
    ./pch.h
  )
endif()

