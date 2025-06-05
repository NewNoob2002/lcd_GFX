set (MAIN_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${MAIN_ROOT}/HAL/inc
    ${MAIN_ROOT}/app
    ${MAIN_ROOT}/lv_port
    ${MAIN_ROOT}/Utils/lv_anim_lable
    ${MAIN_ROOT}/Utils/lv_ext_func
    )
file(GLOB SRCS
     ${MAIN_ROOT}/HAL/src/*.cpp
     ${MAIN_ROOT}/*.cpp
     ${MAIN_ROOT}/lv_port/*.cpp
     ${MAIN_ROOT}/app/*.cpp
     ${MAIN_ROOT}/app/*.c
     ${MAIN_ROOT}/Utils/lv_anim_lable/*.c
     ${MAIN_ROOT}/Utils/lv_ext_func/*.cpp
     )

set(COMPONENT_SRCS ${SRCS})

message(STATUS "main complie files = ${SRCS}")

register_component()