project (phrosty-core CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)

add_library(${PROJECT_NAME} STATIC
  logger.cpp
  state.cpp
  notify.cpp
)

# target_compile_definitions(${PROJECT_NAME}
# )

target_include_directories(${PROJECT_NAME}
  PRIVATE ${LIBNOTIFY_INCLUDE_DIRS}
)

target_link_directories(${PROJECT_NAME}
  PRIVATE ${LIBNOTIFY_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC ${PRIV_SHARED_DEPS}_spdlog
  PRIVATE ${LIBNOTIFY_LIBRARIES}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_precompile_headers(${PROJECT_NAME} PRIVATE
    ./pch.h
  )
endif()

