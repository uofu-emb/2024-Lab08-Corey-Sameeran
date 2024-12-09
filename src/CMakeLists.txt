############## RX ###############

# This is the main binary. List your C files here.
add_executable(can_rx
    can_rx.c
    )

pico_set_program_name(can_rx "test_rx")
pico_set_program_version(can_rx "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(can_rx 0)
pico_enable_stdio_usb(can_rx 1)

# Add the libraries to the build
target_link_libraries(can_rx PRIVATE
  can2040
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(can_rx PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(can_rx)

add_custom_target(flash_rx picotool load -f $<TARGET_FILE:can_rx> DEPENDS can_rx)

############## BABBLING ###############

add_executable(can_babbling
    can_babbling.c
)

pico_set_program_name(can_babbling "test_babbling")
pico_set_program_version(can_babbling "0.1")

pico_enable_stdio_uart(can_babbling 0)
pico_enable_stdio_usb(can_babbling 1)

target_link_libraries(can_babbling PRIVATE
  can2040
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

if(PICO_CYW43_SUPPORTED)
    target_link_libraries(can_babbling PRIVATE
        pico_cyw43_arch_none
    )
endif()

pico_add_extra_outputs(can_babbling)

add_custom_target(flash_babbling picotool load -f $<TARGET_FILE:can_tx> DEPENDS can_babbling)

############## TX ###############

add_executable(can_tx
    can_tx.c
)

pico_set_program_name(can_tx "test_tx")
pico_set_program_version(can_tx "0.1")

pico_enable_stdio_uart(can_tx 0)
pico_enable_stdio_usb(can_tx 1)

target_link_libraries(can_tx PRIVATE
  can2040
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

if(PICO_CYW43_SUPPORTED)
    target_link_libraries(can_tx PRIVATE
        pico_cyw43_arch_none
    )
endif()

pico_add_extra_outputs(can_tx)

add_custom_target(flash_tx picotool load -f $<TARGET_FILE:can_tx> DEPENDS can_tx)
