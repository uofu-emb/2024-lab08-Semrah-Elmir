#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

static struct can2040 cbus;

QueueHandle_t recieved_msgs;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    xQueueSendToBack(recieved_msgs, msg, portMAX_DELAY);
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 16, gpio_tx = 17;

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

void main_thread(void *params){
    struct can2040_msg msg;
    xQueueReceive(recieved_msgs, &msg, portMAX_DELAY); 
    printf("Recieved message: %s\n", msg.data);
}

void send_thread(void *params){

    /*
    struct can2040_msg {
    uint32_t id;
    uint32_t dlc;
    union {
        uint8_t data[8];
        uint32_t data32[2];
    };
};
    */

    struct can2040_msg send_msg;
    send_msg.id = 0x200;
    send_msg.dlc = 5;
    send_msg.data[0] = 'H';
    send_msg.data[1] = 'e';
    send_msg.data[2] = 'l';
    send_msg.data[3] = 'l';
    send_msg.data[4] = 'o';

    int status = can2040_transmit(&cbus, &send_msg);

    if(status){
        printf("Message sent\n");
    } else {
        printf("Message not sent\n");
    }

}

int main(void)
{
    stdio_init_all();
    recieved_msgs = xQueueCreate(100, sizeof(struct can2040_msg));
    canbus_setup();
    TaskHandle_t main_task;
    xTaskCreate(main_thread, "MainThread",
            MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main_task);
    vTaskStartScheduler();

}