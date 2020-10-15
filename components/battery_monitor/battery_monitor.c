#include "battery_monitor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/adc.h>
#include "esp_adc_cal.h"

#include "stdio.h"

#define BATTERY_MONITOR_CHANNEL 4

esp_adc_cal_characteristics_t adc_characteristic;
float startup_voltage = 0;

void battery_monitor_setup()
{
    printf("Battery monitor init\n");

    adc2_config_channel_atten(BATTERY_MONITOR_CHANNEL, ADC_ATTEN_11db );
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_characteristic);

    // We store startup voltage, as the current voltage may fluctuate after taking wifi online, due to ADC channel
    // disturbance in the PCB.
    startup_voltage = battery_monitor_get_voltage();
}

float battery_monitor_get_startup_voltage()
{
    return startup_voltage;
}

float battery_monitor_get_voltage()
{
    int raw_adc;
    float v_batt = 0;

    adc2_get_raw(BATTERY_MONITOR_CHANNEL, ADC_WIDTH_BIT_12, &raw_adc);
    raw_adc = esp_adc_cal_raw_to_voltage(raw_adc, &adc_characteristic);

    v_batt = ((float) raw_adc)*(3.2/1000); // The voltage divider divides by 3.2 and raw_adc is in mV

    return v_batt;
}