# This is the main binary. List your C files here.
add_executable(lab09
    can.c
    )

pico_set_program_name(lab09 "lab09")
pico_set_program_version(lab09 "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(lab09 0)
pico_enable_stdio_usb(lab09 1)

# Add the libraries to the build
target_link_libraries(lab09 PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(lab09 PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(lab09)

add_custom_target(flash picotool load -f $<TARGET_FILE:lab09> DEPENDS lab09)





##
## CanRecieveOnly
##

# This is the main binary. List your C files here.
add_executable(CanRecieveOnly
    canrecieveonly.c
    )

pico_set_program_name(CanRecieveOnly "CanRecieveOnly")
pico_set_program_version(CanRecieveOnly "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(CanRecieveOnly 0)
pico_enable_stdio_usb(CanRecieveOnly 1)

# Add the libraries to the build
target_link_libraries(CanRecieveOnly PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
  can2040
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(CanRecieveOnly PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(CanRecieveOnly)

add_custom_target(flash_recieveonly picotool load -f $<TARGET_FILE:CanRecieveOnly> DEPENDS CanRecieveOnly)