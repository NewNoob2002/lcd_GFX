set (RESOURCE_MANAGER_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${RESOURCE_MANAGER_ROOT}/inc
    )
file(GLOB SRCS
    ${RESOURCE_MANAGER_ROOT}/src/*.cpp
     )

set(COMPONENT_SRCS ${SRCS})
set(COMPONENT_REQUIRES
    arduino-esp32
    driver
    lvgl
    )

message(STATUS "ResourceManager use components = ${COMPONENT_REQUIRES}")

register_component()