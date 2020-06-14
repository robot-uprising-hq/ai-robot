/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/param.h>
#include <sys/select.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"

static const char* TAG = "robotfrontend";

static void uart1_deinit(void)
{
    // close(uart_fd);
    // uart_fd = -1;
    uart_driver_delete(UART_NUM_0);
}

static void uart1_init(void)
{
    // uart_config_t uart_config = {
    //     .baud_rate = 115200,
    //     .data_bits = UART_DATA_8_BITS,
    //     .parity    = UART_PARITY_DISABLE,
    //     .stop_bits = UART_STOP_BITS_1,
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    //     .source_clk = UART_SCLK_APB,
    // };
    // uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
    // uart_param_config(UART_NUM_1, &uart_config);
    // uart_set_loop_back(UART_NUM_1, true);

    // if ((uart_fd = open("/dev/uart/1", O_RDWR | O_NONBLOCK)) == -1) {
    //     ESP_LOGE(TAG, "Cannot open UART1");
    //     uart1_deinit();
    // }

    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    esp_vfs_dev_uart_use_driver(0);
}

int my_fgets(char *buf, size_t buf_len, int fd)
{
    int read_bytes = 0;

    while (1) {
        int s;
        fd_set rfds;
        struct timeval tv = {
            .tv_sec = 1,
            .tv_usec = 0,
        };

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        s = select(fd + 1, &rfds, NULL, NULL, &tv);

        if (s < 0) {
            ESP_LOGE(TAG, "Select failed: errno %d", errno);
        } else if (s == 0) {
            ESP_LOGD(TAG, "Timeout has been reached and nothing has been received");
        } else {
            ssize_t ret = read(fd, buf + read_bytes, 1);
            if (ret < 0) {
                if (errno != EAGAIN && errno != EINTR) {
                    ESP_LOGE(TAG, "Read failed: errno %d", errno);
                    return -1;
                }
            } else if (ret == 0) {
                ESP_LOGI(TAG, "Got EOF from read");
                return read_bytes;
            } else {
                if (buf[read_bytes] == '\n') {
                    return read_bytes + 1;
                }
                read_bytes += ret;
                if (read_bytes >= buf_len) {
                    return read_bytes;
                }
            }
        }
    }
}

static void echo_task(void *param)
{
        uart1_init();

    while (1)
    {
        printf("Write something:\n");
        char buf[20] = "";
        my_fgets(buf, sizeof(buf) - 1, fileno(stdin));
        buf[sizeof(buf) - 1] = '\0';
        printf("\nYou wrote: %s\n", buf);
    }

    uart1_deinit();
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

    // {
    //     printf("Write something: ");
    //     char buf[20] = "";
    //     my_fgets(buf, sizeof(buf) - 1, fileno(stdin));
    //     buf[sizeof(buf) - 1] = '\0';
    //     printf("\nYou wrote: %s\n", buf);
    // }

    // for (int i = 10; i >= 0; i--) {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
