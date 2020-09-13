#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "esp_system.h"

esp_err_t led_control_setup();

typedef enum
{
    LED_WIFI = 0,
    LED_SERVER,
    LED_ACTIVITY
} LedControlLed;

void
led_control_activate(LedControlLed led, int timeout);

void
led_control_deactivate(LedControlLed led);


void led_control_deinit();

#endif /* LED_CONTROL_H */
