set (PAGES_MANAGER_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${PAGES_MANAGER_ROOT}/inc
    )
file(GLOB SRCS
    ${PAGES_MANAGER_ROOT}/src/*.cpp
     )

set(COMPONENT_SRCS ${SRCS})
set(COMPONENT_REQUIRES
    arduino-esp32
    driver
    lvgl
    )

message(STATUS "PagesManager use components = ${COMPONENT_REQUIRES}")

register_component()