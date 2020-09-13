#include "led_control.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "led_control";

#define LED_WIFI_PIN        17
#define LED_SERVER_PIN      5
#define LED_ACTIVITY_PIN    18

#define LED_PIN_SEL ((1ULL<<LED_WIFI_PIN) | (1ULL<<LED_SERVER_PIN) | (1ULL<<LED_ACTIVITY_PIN))

static void lc_gpio_init()
{
    printf("LED control GPIO init\n");
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = LED_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);
}

struct Led {
    int pin;
    esp_timer_handle_t timer;
} leds[3] = {{LED_WIFI_PIN, 0}, {LED_SERVER_PIN, 0}, {LED_ACTIVITY_PIN, 0}};

void led_control_turn_off_callback(void *arg);

esp_err_t led_control_setup()
{
    lc_gpio_init();

    esp_err_t err;
    esp_timer_create_args_t timer_config = {};
    timer_config.callback = led_control_turn_off_callback;
    timer_config.arg = (void *)LED_WIFI;
    timer_config.dispatch_method = ESP_TIMER_TASK;
    timer_config.name = "led_timer_wifi";
    err = esp_timer_create(&timer_config, &(leds[LED_WIFI].timer));
    if (err != ESP_OK) {
        return err;
    }

    timer_config.arg = (void *)LED_SERVER;
    timer_config.name = "led_timer_server";
    err = esp_timer_create(&timer_config, &(leds[LED_SERVER].timer));
    if (err != ESP_OK) {
        esp_timer_delete(leds[LED_WIFI].timer);
        return err;
    }

    timer_config.arg = (void *)LED_ACTIVITY;
    timer_config.name = "led_timer_activity";
    err = esp_timer_create(&timer_config, &(leds[LED_ACTIVITY].timer));
    if (err != ESP_OK) {
        esp_timer_delete(leds[LED_WIFI].timer);
        esp_timer_delete(leds[LED_SERVER].timer);
        return err;
    }
    return ESP_OK;
}

void led_control_turn_off_callback(void *arg)
{
    LedControlLed led = (LedControlLed)arg;
    if (led < LED_WIFI || led > LED_ACTIVITY) {
        ESP_LOGE(TAG, "Invalid value for led %d", led);
        return;
    }

    led_control_deactivate(led);
}

void
led_control_activate(LedControlLed led, int timeout)
{
    esp_timer_stop(leds[led].timer);
    ESP_LOGI(TAG, "Activating led %d, pin %d, timeout %d.", led, leds[led].pin, timeout);
    gpio_set_level(leds[led].pin, 1);
    if (timeout > 0) {
        esp_timer_start_once(leds[led].timer, timeout * 1000);
    }
}

void
led_control_deactivate(LedControlLed led)
{
    esp_timer_stop(leds[led].timer);
    gpio_set_level(leds[led].pin, 0);
    ESP_LOGI(TAG, "Deactivating led %d, pin %d.", led, leds[led].pin);
}

void led_control_deinit()
{
    // Allocated resources should be freed here, and initialized resources de-initialized.
    for (int ii = 0; ii < sizeof(leds)/sizeof(leds[0]); ii++) {
        esp_timer_stop(leds[ii].timer);
        esp_timer_delete(leds[ii].timer);
    }
}