set (MAIN_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${MAIN_ROOT}/HAL/include
    )
file(GLOB SRCS
     ${MAIN_ROOT}/HAL/src/*.cpp
     ${MAIN_ROOT}/*.cpp
     )

set(COMPONENT_SRCS ${SRCS})

message(STATUS "main use components = ${SRCS}")

register_component()