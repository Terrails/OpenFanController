#include "board.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "fan_control.h"
#include "host_communication.h"
#include "usb_cdc.h"

#define LOG_NAME "MAIN"
#include "logger.h"

bool blink_connecting_callback(repeating_timer_t *rt);
bool blink_active_callback(repeating_timer_t *rt);

int main() {
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

    repeating_timer_t blink_connecting_timer = {0};
    bool blink_connecting = false;

    repeating_timer_t blink_active_timer = {0};
    bool blink_active = false;

    while (1) {
        usb_cdc_tick();

        bool serial_connected = gpio_get(USB_DET_PIN) && usb_cdc_connected();
        if (!serial_connected) { // Start blinking if not connected
            if (!blink_connecting) {
                LOG_INFO("USB Disconnected");
                add_repeating_timer_ms(-1000, blink_connecting_callback, NULL, &blink_connecting_timer);
                blink_connecting = true;
            }

            if (blink_active) {
                cancel_repeating_timer(&blink_active_timer);
                gpio_put(LED_PIN, 0);
                blink_active = false;
            }

            continue;
        }

        if (blink_connecting) {
            LOG_INFO("USB Connected");
            cancel_repeating_timer(&blink_connecting_timer);
            gpio_put(LED_PIN, 0);
            blink_connecting = false;
        }

        if (!blink_active) {
            add_repeating_timer_ms(-30000, blink_active_callback, NULL, &blink_active_timer);
            blink_active = true;
        }

        fan_periodic_tick();
        host_comm_tick();
    }
}

bool blink_connecting_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN, !static_cast<bool>(gpio_get(LED_PIN)));
    return true;
}

bool blink_active_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN, 1);
    // Turn off for a quick blink
    add_alarm_in_ms(500, [](alarm_id_t id, void *user_data) -> int64_t {
        gpio_put(LED_PIN, 0);
        return 0;
    }, NULL, true);
    return true;
}
