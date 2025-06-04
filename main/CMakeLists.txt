set (MAIN_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${MAIN_ROOT}/HAL/inc
    ${MAIN_ROOT}/ui
    ${MAIN_ROOT}/lv_port
    )
file(GLOB SRCS
     ${MAIN_ROOT}/HAL/src/*.cpp
     ${MAIN_ROOT}/*.cpp
     ${MAIN_ROOT}/lv_port/*.cpp
     ${MAIN_ROOT}/ui/*.cpp
     ${MAIN_ROOT}/ui/*.c
     )

set(COMPONENT_SRCS ${SRCS})

message(STATUS "main use components = ${SRCS}")

register_component()