project (phrosty C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui X11Extras REQUIRED)
qt5_add_resources(RESOURCES "resource.qrc")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
  app.cpp
  cli.cpp
  window.cpp
  font.cpp
  monitor.cpp
  store.cpp
  action_bar.cpp
  slider.cpp
  xutils.cpp
  ${RESOURCES}
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE "QT_NO_KEYWORDS" # for avoiding issues with other libraries like Python.h, Boost
)

target_include_directories(${PROJECT_NAME}
  PRIVATE "../"
  PRIVATE ${CMAKE_BINARY_DIR}/../vendor/xorg-xwininfo/include
  PRIVATE ${CMAKE_BINARY_DIR}/include
)

target_link_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_BINARY_DIR}/../vendor/xorg-xwininfo/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Threads::Threads
  PRIVATE Qt5::Widgets
  PRIVATE Qt5::Gui
  PRIVATE Qt5::X11Extras
  PRIVATE X11
  PRIVATE phrosty-core
  PRIVATE phrosty-renderer
  PRIVATE xwinfolib
  PUBLIC ${PRIV_SHARED_DEPS}_cli11
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_precompile_headers(${PROJECT_NAME} PRIVATE
    ./pch.h
  )
endif()

