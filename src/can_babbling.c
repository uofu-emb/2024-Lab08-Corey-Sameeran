#include "can2040.h"
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define BABBLING 1

static struct can2040 cbus;
QueueHandle_t msgs;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    xQueueSendToBack(msgs, msg, portMAX_DELAY);
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void tx_task(__unused void *params)
{
    struct can2040_msg msg;
    #ifdef BABBLING
        msg.id = 0x1; // Lower message id = higher priority
    #else
        msg.id = 0x2;
    #endif
    msg.dlc = 1;
    msg.data[0] = 0x01;

    while (1) {
        if (can2040_transmit(&cbus, &msg))
        {
            printf("Transmission failed.\n");
        }
        else
        {
            printf("Transmission sent.\n");
        }
        
        #ifndef BABBLING
            sleep_ms(1000);
        #endif
    }
}

void rx_task(__unused void *params)
{
    struct can2040_msg data;
    while (1) {
        xQueueReceive(msgs, &data, portMAX_DELAY);
        printf("Got message\n");
    }
}

int main(void)
{
    stdio_init_all();
    canbus_setup();
    TaskHandle_t tx_task_handle, rx_task_handle;
    xTaskCreate(tx_task, "TxThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &tx_task_handle);
    xTaskCreate(rx_task, "RxThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &rx_task_handle);
    vTaskStartScheduler();
    return 0;
}