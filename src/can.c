#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SEND_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define SEND_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

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
    canbus_setup();

    while(1) {
        //Creat a message struct and receive message from the queue. Print message.
        struct can2040_msg msg;
        xQueueReceive(recieved_msgs, &msg, portMAX_DELAY); 
        printf("Recieved message: %s\n", msg.data);
    }
}

void send_thread(void *params){

    while(1){

        //Create a message struct and put hello into the data. Priority of 200. (Low)
        struct can2040_msg send_msg;
        send_msg.id = 0x200;
        send_msg.dlc = 5;
        send_msg.data[0] = 'H';
        send_msg.data[1] = 'e';
        send_msg.data[2] = 'l';
        send_msg.data[3] = 'l';
        send_msg.data[4] = 'o';

        //Send the message and check status
        int status = can2040_transmit(&cbus, &send_msg);

        //Check if the message was sent successfully
        // if(status == 0){
        //     printf("Message sent\n");
        // } else if (status < 0) {
        //     printf("No space on CAN message queue.\n");
        // }

        //Delay so we can see whats happening.
        vTaskDelay(50);
    }
}

int main(void)
{
    //Initialize and wait for 5 seconds.
    stdio_init_all();
    //sleep_ms(5000);
    printf("Starting Pico\n");

    //Create queue. Setup the CAN bus. Create threads. Start scheduler.
    recieved_msgs = xQueueCreate(100, sizeof(struct can2040_msg));
    TaskHandle_t main_task, send_task;
    xTaskCreate(main_thread, "MainThread",
            MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main_task);
    xTaskCreate(send_thread, "SendThread",
            SEND_TASK_STACK_SIZE, NULL, SEND_TASK_PRIORITY, &send_task);
    vTaskStartScheduler();

    return(0);
}