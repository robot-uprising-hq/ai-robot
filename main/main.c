/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "console.h"

//static const char* TAG = "robotfrontend";

// Uninstall UART communications driver.
static void uart0_deinit(void)
{
    uart_driver_delete(UART_NUM_0);
}

// Install UART communications driver.
static void uart0_init(void)
{
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    esp_vfs_dev_uart_use_driver(0);
}

static void echo_task(void *param)
{
    uart0_init();

    while (1)
    {
        printf("Write something:\n");
        char buf[20] = "";
        my_fgets(buf, sizeof(buf) - 1, fileno(stdin));
        buf[sizeof(buf) - 1] = '\0';
        printf("\nYou wrote: %s\n", buf);
    }

    uart0_deinit();
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("Hello world!\n");


    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    xTaskCreate(echo_task, "echo_task", 4*1024, NULL, 5, NULL);
}
