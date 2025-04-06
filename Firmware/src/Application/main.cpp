#include "board.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "fan_control.h"
#include "host_communication.h"
#include "usb_cdc.h"

#define LOG_NAME "MAIN"
#include "logger.h"

bool led_state = false;

bool systick_callback(repeating_timer_t *rt);

int main()
{
    set_sys_clock_khz(250000, false);
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(USB_DET_PIN);
    gpio_set_dir(USB_DET_PIN, GPIO_IN);

    i2c_init(PICO_I2C_INSTANCE, 400 * 1000);
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);

    fan_control_init();
    host_comm_init();

    usbd_serial_init();
    tusb_init();

    LOG_INFO("Up and running");

    repeating_timer_t timer = {0}; // zero out all fields
    bool blinking = false;

    while (1) {
        usb_cdc_tick();

        bool serial_connected = gpio_get(USB_DET_PIN) && usb_cdc_connected();
        if (!serial_connected) { // Start blinking if not connected
            if (!blinking) {
                LOG_INFO("USB Disconnected");
                add_repeating_timer_ms(-1000, systick_callback, NULL, &timer);
                blinking = true;
            }
            continue;
        }

        if (blinking) {
            LOG_INFO("USB Connected");
            cancel_repeating_timer(&timer);
            gpio_put(LED_PIN, 0);
            blinking = false;
        }

        fan_periodic_tick();
        host_comm_tick();
    }
}

bool systick_callback(repeating_timer_t *rt)
{
    gpio_put(LED_PIN, led_state);
    led_state = !led_state;
    return true; // keep repeating
}
