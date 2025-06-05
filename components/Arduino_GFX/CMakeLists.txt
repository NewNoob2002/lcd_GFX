set (GFX_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(COMPONENT_ADD_INCLUDEDIRS
    ${GFX_ROOT}/src
    ${GFX_ROOT}/src/canvas
    ${GFX_ROOT}/src/databus
    ${GFX_ROOT}/src/display
    ${GFX_ROOT}/src/font
    )
file(GLOB SRCS
    ${GFX_ROOT}/src/*.cpp
    ${GFX_ROOT}/src/canvas/*.cpp
    ${GFX_ROOT}/src/databus/*.cpp
    ${GFX_ROOT}/src/display/*.cpp
     )

set(COMPONENT_SRCS ${SRCS})
set(COMPONENT_REQUIRES
    arduino-esp32
    driver
    esp_lcd
    esp_timer
    )

message(STATUS "GFXLib use components = ${COMPONENT_REQUIRES}")

register_component()