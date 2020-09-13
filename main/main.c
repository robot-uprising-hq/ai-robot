/*
 * Robot Uprising: Micro-Invaders 2020
 *
 * Robot frontend. This connects the robot to Wifi and to the robot
 * backend, allowing the robot to be remotely controlled.
 *
 * At minimum, need to get some rudimentary status information from
 * the robot and be able to control left and right track motors.
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include <ctype.h>
#include "esp_wifi.h"
#include "readline.h"
#include "wifiscan.h"
#include "wificonnect.h"
#include "udpserver.h"
#include "motor_control.h"

static const char* TAG = "robotfrontend";

/*
 * Uninstall UART communications driver.
 */
static void uart0_deinit(void)
{
    uart_driver_delete(UART_NUM_0);
}

/*
 * Install UART communications driver.
 */
static void uart0_init(void)
{
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    esp_vfs_dev_uart_use_driver(0);
}

/*
 * Right-trim whitespace from given buffer.
 */
static void rtrim(char *buf, size_t buf_len)
{
    for (int ii = 0; ii < buf_len; ii++) {
        char *p = &buf[buf_len - 1 - ii];
        if (isspace(*p)) {
            *p = '\0';
        } else if (*p == '\0') {
            continue;
        } else {
            // Anything else, stop trim.
            break;
        }
    }
}

static esp_err_t set_config(nvs_handle_t handle, const char *key, const char *val)
{
    esp_err_t err = nvs_set_str(handle, key, val);
    if (err != ESP_OK) {
        printf("Failed to write key: %s\n", esp_err_to_name(err));
    } else {
        err = nvs_commit(handle);
        if (err != ESP_OK) {
            printf("Commit failed after writing key: %s\n", esp_err_to_name(err));
        }
        printf("Wrote %s.\n", key);
    }
    return err;
}

static esp_err_t set_config_int(nvs_handle_t handle, const char *key, int val)
{
    esp_err_t err = nvs_set_i32(handle, key, val);
    if (err != ESP_OK) {
        printf("Failed to write key: %s\n", esp_err_to_name(err));
    } else {
        err = nvs_commit(handle);
        if (err != ESP_OK) {
            printf("Commit failed after writing key: %s\n", esp_err_to_name(err));
        }
        printf("Wrote %s.\n", key);
    }
    return err;
}

void motor_action_cb(int left_motor_action, int right_motor_action, int timeout)
{
    printf("Motor request: %d %d %d\n", left_motor_action, right_motor_action, timeout);
    motor_set_speed(1, right_motor_action, timeout);
    motor_set_speed(2, left_motor_action, timeout);
}

static void udp_server_task(void *pvParameters)
{
    udp_server(motor_action_cb);
    vTaskDelete(NULL);
}

static bool start_wifi(nvs_handle_t handle)
{
    esp_err_t err;
    char passwd[60] = "";
    size_t passwd_length = sizeof(passwd);
    char ssid[32] = "";
    size_t ssid_length = sizeof(ssid);

    err = nvs_get_str(handle, "wifi-passwd", passwd, &passwd_length);
    if (err == ESP_OK) {
        err = nvs_get_str(handle, "wifi-ssid", ssid, &ssid_length);
        if (err == ESP_OK) {
            printf("Attempting Wifi connection to %s...\n", ssid);
            wifi_init_sta(ssid, passwd);
            return true;
        }
    }
    if (err != ESP_OK) {
        printf("Wifi not configured.\n");
    }
    return false;
}

static bool start_udp_server(TaskHandle_t *udp_server_handle)
{
    BaseType_t ret = xTaskCreate(udp_server_task, "udp_server", 4 * 1024, NULL, 5, udp_server_handle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Error creating task: %d", ret);
        return false;
    }
    return true;
}

void do_motor_test()
{
    printf("Starting motor test!\n");

//    printf("***** Motor1 fwd, 75\n");
//    motor_set_speed(1, 75);
//    vTaskDelay(1000/portTICK_RATE_MS);
//    motor_set_speed(1, 0);
//    vTaskDelay(1000/portTICK_RATE_MS);
//
//    printf("***** Motor1 bwd, 75\n");
//
//    motor_set_speed(1, -75);
//    vTaskDelay(1000/portTICK_RATE_MS);
//    motor_set_speed(1, 0);
//    vTaskDelay(1000/portTICK_RATE_MS);
//
//    printf("***** Motor2 fwd, 75\n");
//    motor_set_speed(2, 75);
//    vTaskDelay(1000/portTICK_RATE_MS);
//    motor_set_speed(2, 0);
//    vTaskDelay(1000/portTICK_RATE_MS);
//
//    printf("***** Motor2 bwd, 75\n");
//    motor_set_speed(2, -75);
//    vTaskDelay(1000/portTICK_RATE_MS);
//    motor_set_speed(2, 0);
//    vTaskDelay(1000/portTICK_RATE_MS);

    printf("***** Motor1 fwd 50, Motor2 bwd, 100\n");
    motor_set_speed(1, 50, 0);
    motor_set_speed(2, -100, 0);
    vTaskDelay(10000/portTICK_RATE_MS);
    motor_set_speed(1, 0, 0);
    motor_set_speed(2, 0, 0);
    vTaskDelay(1000/portTICK_RATE_MS);

    printf("***** Motor1 bwd 100, Motor2 fwd, 50\n");
    motor_set_speed(1, -100, 0);
    motor_set_speed(2, 50, 0);
    vTaskDelay(10000/portTICK_RATE_MS);
    motor_set_speed(1, 0, 0);
    motor_set_speed(2, 0, 0);
    vTaskDelay(1000/portTICK_RATE_MS);

    printf("***** Motor1&2 fwd, 100\n");
    motor_set_speed(1, 100, 0);
    motor_set_speed(2, 100, 0);
    vTaskDelay(5000/portTICK_RATE_MS);
    motor_set_speed(1, 0, 0);
    motor_set_speed(2, 0, 0);

    printf("***** Motor1&2 bwd, 100\n");
    motor_set_speed(1, -100, 0);
    motor_set_speed(2, -100, 0);
    vTaskDelay(5000/portTICK_RATE_MS);
    motor_set_speed(1, 0, 0);
    motor_set_speed(2, 0, 0);
}

/*
 * Return `true` if buf is prefixed with `string`, `false` otherwise. `string` needs to be nul-terminated. If `rest` is
 * not null, it will be set to point to the character right after the prefix.
 */
static bool has_prefix(const char *buf, size_t buf_len, const char *string, const char **rest)
{
    if (buf_len < strlen(string)) {
        return false;
    }

    bool ret = strncmp(buf, string, strlen(string)) == 0;

    if (ret && rest != NULL) {
        *rest = &buf[strlen(string)];
    }
    return ret;
}

static void dump_key_int(nvs_handle_t handle, const char *key)
{
    int int_value = 0;
    esp_err_t err;

    err = nvs_get_i32(handle, key, &int_value);
    if (err != ESP_OK) {
        printf("Failed to get %s: %s\n", key, esp_err_to_name(err));
    } else {
        printf("%s: %d\n", key, int_value);
    }
}

const char *help_text =
"Command loop help\n"\
"\n"\
"Typing commands does not echo, turn on local echo on your terminal \n"\
"if you need it.\n"\
"\n"\
"help                Show this help.\n"\
"scan                Do a quick Wifi AP scan.\n"\
"query               Show configuration.\n"\
"query all           Show all configuration, including passwords.\n"\
"set ssid SSID       Set WiFi AP to use to SSID.\n"\
"set passwd PASSWD   Set Wifi password to PASSWD.\n"\
"set mc-hard-timeout MS\n"\
"                    Set motor hard timeout to MS milliseconds. (see motor_control.h)\n"\
"set mc-pwm-frequency FREQ\n"\
"                    Set motor PWM frequency to FREQ. (see motor_control.h)\n"\
"udpsrvstart         Start UDP server.\n"\
"wifistart           (Attempt to) start WiFi.\n"\
"wifistop            Disconnect from WiFi.\n"\
"motortest           Run simple test of the motors.\n"\
"quit                Exit command-loop (mainly for debugging).\n"
"\n";
static void command_loop_task(void *param)
{
    uart0_init();

    esp_err_t err;
    nvs_handle_t handle = 0;
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open storage: %s",  esp_err_to_name(err));
    }

    TaskHandle_t udp_server_handle = NULL;
    bool wifi_started = false;


    /* If configuration is ok, try to start services automatically. */
    wifi_started = start_wifi(handle);
    if (wifi_started) {
        start_udp_server(&udp_server_handle);
    }

    {
        MotorControlConfig config = MotorControlConfig_zero;
        int int_value;
        err = nvs_get_i32(handle, "mc-hard-timeout", &int_value);
        if (err == ESP_OK) {
            config.hard_timeout = int_value;
        }
        err = nvs_get_i32(handle, "mc-pwm-freq", &int_value);
        if (err == ESP_OK) {
            config.pwm_frequency = int_value;
        }
        ESP_ERROR_CHECK(motor_control_setup(&config));
    }

    while (1)
    {
        // TODO: Add MAC address of the WiFi as well.
        char ipaddr[32] = "";
        const char *rest;

        get_ip_addr(ipaddr, sizeof(ipaddr));
        printf("%s%sEnter command:\n", ipaddr, *ipaddr ? " " : "");

        char buf[60] = "";
        size_t buf_len = sizeof(buf) - 1;
        readline(buf, sizeof(buf) - 1, fileno(stdin));
        buf[sizeof(buf) - 1] = '\0';
        printf("\nYou wrote: %s\n", buf);

        if (has_prefix(buf, buf_len, "scan", NULL)) {
            wifi_scan();
        } else if (strncmp(buf, "query", 5) == 0) {
            printf("** Status:\n\n");
            printf("Wifi %srunning\n", wifi_started ? "" : "not ");
            printf("UDP server %srunning\n", udp_server_handle != NULL ? "" : "not ");
            printf("\n** Config:\n\n");
            char string[40] = "";
            size_t string_length = sizeof(string);
            err = nvs_get_str(handle, "wifi-ssid", string, &string_length);
            if (err != ESP_OK) {
                printf("Failed to get key: %s\n", esp_err_to_name(err));
            } else {
                printf("wifi-ssid: \"%s\"\n", string);
            }

            string_length = sizeof(string);
            err = nvs_get_str(handle, "wifi-passwd", string, &string_length);
            if (err != ESP_OK) {
                printf("Failed to get key: %s\n", esp_err_to_name(err));
            } else {
                if (strncmp(buf, "query all", 9) == 0) {
                    printf("wifi-passwd: \"%s\"\n", string);
                } else {
                    printf("wifi-passwd: <is set>\n");
                }
            }
            dump_key_int(handle, "mc-hard-timeout");
            dump_key_int(handle, "mc-pwm-freq");
        } else if (strncmp(buf, "set ssid", 8) == 0) {
            rtrim(buf, sizeof(buf));
            set_config(handle, "wifi-ssid", &buf[9]);
        } else if (strncmp(buf, "set passwd", 10) == 0) {
            rtrim(buf, sizeof(buf));
            set_config(handle, "wifi-passwd", &buf[11]);
        } else if (has_prefix(buf, buf_len, "set mc-hard-timeout", &rest)) {
            rtrim(buf, sizeof(buf));
            set_config_int(handle, "mc-hard-timeout", atoi(rest));
        } else if (has_prefix(buf, buf_len, "set mc-pwm-freq", &rest)) {
            rtrim(buf, sizeof(buf));
            int new_value = atoi(rest);
            set_config_int(handle, "mc-pwm-freq", new_value);
            motor_control_set_hard_timeout(new_value);
            printf("*** note: changing PWM frequency requires a reset of the robot to take effect.\n");
        } else if (strncmp(buf, "help", 4) == 0) {
            printf(help_text);
        } else if (strncmp(buf, "wifistart", 9) == 0) {
            if (!wifi_started) {
                if (start_wifi(handle)) {
                    wifi_started = true;
                }
            } else {
                printf("Wifi already started.\n");
            }
        } else if (strncmp(buf, "udpsrvstart", 11) == 0) {
            if (!udp_server_handle) {
                start_udp_server(&udp_server_handle);
            } else {
                printf("UDP server already running.\n");
            }
        } else if (strncmp(buf, "wifistop", 8) == 0) {

            err = esp_wifi_stop();
            if (err != ESP_OK) {
                printf("Failed to stop wifi: %s\n", esp_err_to_name(err));
            }
            wifi_started = false;
        } else if (strncmp(buf, "motortest", 9) == 0) {
            do_motor_test();
        } else if (strncmp(buf, "quit", 4) == 0) {
            printf("Quitting for testing deinit code...\n");
            break;
        } else {
            printf("Unknown command: %s", buf);
        }
    }

    motor_control_deinit();
    nvs_close(handle);
    uart0_deinit();
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("*** Robot Uprising: Micro-Invaders ***\n");

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

    // Needed for wifi code.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    xTaskCreate(command_loop_task, "command_loop", 4*1024, NULL, 5, NULL);
}
