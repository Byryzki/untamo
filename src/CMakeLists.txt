# Add executable. Default name is the project name, version 0.1

add_executable(untamo
    main.c
    noods.c
    oled.c
    )

pico_set_program_name(untamo "untamo")
pico_set_program_version(untamo "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(untamo 0)
pico_enable_stdio_usb(untamo 1)

# Add the standard library to the build
target_link_libraries(untamo
        pico_stdlib
        OLED_libs
        pulse_lib
        )

# Add the standard include files to the build
target_include_directories(untamo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PROJECT_SOURCE_DIR}/libs/oled
        ${PROJECT_SOURCE_DIR}/libs/pulse
)

target_link_libraries(untamo 
        hardware_spi
        hardware_i2c
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_adc
        hardware_gpio
)

pico_add_extra_outputs(untamo)