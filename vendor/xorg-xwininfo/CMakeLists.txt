project (xwinfolib C CXX)

# find_package(PkgConfig REQUIRED)

add_library(${PROJECT_NAME} STATIC
  xwininfo.cpp
  dsimple.c
  clientwin.c
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE xcb
)
