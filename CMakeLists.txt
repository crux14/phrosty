cmake_minimum_required(VERSION 3.20)
project (phrosty_proj C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
  message("-- Using Debug mode")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize-recover=all")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -rdynamic")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  message("-- Using Release mode")
else()
  message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE found! Possible values are:\n'Debug', 'Release'")
endif()

set(PHROSTY_CONFIG_TARGET_NAME "phrosty_priv_config_target")

set(PRIV_SHARED_DEPS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/shared_temp/)
set(PRIV_SHARED_DEPS "priv_shared_deps")
set(PRIV_CREDITS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/shared_credits/)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrepareSharedDeps.cmake)

add_definitions(
  " -Wall -Wextra"
  " -std=c++17"
  " -fno-rtti"
  " -fdiagnostics-color=always"
  " -fdiagnostics-show-template-tree"
)

add_subdirectory("./vendor/xorg-xwininfo")

add_subdirectory("./src/core")

add_subdirectory("./src/renderer")

add_subdirectory("./src/ui")
set_target_properties(phrosty PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(phrosty ${PHROSTY_CONFIG_TARGET_NAME})
add_custom_target(${PHROSTY_CONFIG_TARGET_NAME} ALL
  VERBATIM
  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/include/version.h ${CMAKE_CURRENT_SOURCE_DIR}/VERSION
  COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/version.sh ${CMAKE_CURRENT_SOURCE_DIR}/VERSION ${CMAKE_CURRENT_BINARY_DIR}/include
)

install(TARGETS phrosty
  RUNTIME DESTINATION "bin"
)
install(DIRECTORY ${PRIV_CREDITS_ROOT}
  DESTINATION "share/phrosty/credits"
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/credits/
  DESTINATION "share/phrosty/credits"
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/desktop/phrosty.desktop
  DESTINATION "share/applications"
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/desktop/Phrosty.png
  DESTINATION "share/icons"
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/desktop/Phrosty_ICON8_LINK.html
  DESTINATION "share/icons"
)
