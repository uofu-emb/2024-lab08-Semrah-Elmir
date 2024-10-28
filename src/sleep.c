#include <stdio.h>
#include <pico/stdlib.h>

#define OUT_PIN 1
#define DELAY_MS 500

int main(void)
{
    int toggle = 0;
    stdio_init_all();
    gpio_init(OUT_PIN);
    gpio_set_dir(OUT_PIN, GPIO_OUT);
    gpio_put(OUT_PIN, toggle);

    while (true) {
        sleep_ms(100); // Work
        toggle = !toggle;
        gpio_put(OUT_PIN, toggle);
        sleep_ms(DELAY_MS);
    }
}