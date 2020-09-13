#include "motor_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/mcpwm.h" 

#include "esp_log.h"
#include "esp_timer.h"

#include "math.h"
#include "stdio.h"

// Pin definitions
#define MOTOR_1_DIR_A   23
#define MOTOR_1_DIR_B   21
#define MOTOR_1_PWM     19

#define MOTOR_2_DIR_A   33
#define MOTOR_2_DIR_B   25
#define MOTOR_2_PWM     32

#define MOTOR_DIR_PIN_SEL ((1ULL<<MOTOR_1_DIR_A) | (1ULL<<MOTOR_1_DIR_B) | (1ULL<<MOTOR_2_DIR_A) | (1ULL<<MOTOR_2_DIR_B))

/* Global config. */
MotorControlConfig motor_control_config = MotorControlConfig_zero;

esp_timer_handle_t motor1_timeout_timer;
esp_timer_handle_t motor2_timeout_timer;

static const char *TAG = "motor_control";

void gpio_init()
{
    printf("Motor control GPIO init\n");
    gpio_config_t io_conf;
    
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = MOTOR_DIR_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    
    gpio_config(&io_conf);
}

void pwm_gpio_config()
{
    printf("MCPWM GPIO init\n");
    mcpwm_pin_config_t pin_config = {
        .mcpwm0a_out_num = MOTOR_1_PWM,
        .mcpwm1a_out_num = MOTOR_2_PWM,
    };
    
    mcpwm_set_pin(MCPWM_UNIT_0, &pin_config);
}

void mcpwm_config()
{
    printf("MCPWM init\n");
    printf("MCPWM pwm freq: %d\n", motor_control_config.pwm_frequency);
    printf("MCPWM hard timeout: %d\n", motor_control_config.hard_timeout);
    mcpwm_config_t pwm_config;
    
    pwm_config.frequency = motor_control_config.pwm_frequency;    //frequency in Hz
    pwm_config.cmpr_a = 0.0;       // initial duty cycle 0
    pwm_config.cmpr_b = 0.0;       // initial duty cycle 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
}

/* Forward declaration. */
void motor_control_stop_motor_callback(void *arg);

esp_err_t motor_control_setup(MotorControlConfig *config)
{
    if (config != NULL) {
        motor_control_config = *config;
    }
    gpio_init();
    pwm_gpio_config();
    mcpwm_config();

    esp_err_t err;
    esp_timer_create_args_t timer_config = {};
    timer_config.callback = motor_control_stop_motor_callback;
    timer_config.arg = (void *)1;
    timer_config.dispatch_method = ESP_TIMER_TASK;
    timer_config.name = "motor_time_1";
    err = esp_timer_create(&timer_config, &motor1_timeout_timer);
    if (err != ESP_OK) {
        return err;
    }
    timer_config.arg = (void *)2;
    timer_config.name = "motor_time_2";

    err = esp_timer_create(&timer_config, &motor2_timeout_timer);
    if (err != ESP_OK) {
        esp_timer_delete(motor1_timeout_timer);
        return err;
    }
    return ESP_OK;
}

void motor_control_deinit()
{
    // Allocated resources should be freed here, and initialized resources deinitialized.
    esp_timer_stop(motor1_timeout_timer);
    esp_timer_delete(motor1_timeout_timer);
    esp_timer_stop(motor2_timeout_timer);
    esp_timer_delete(motor2_timeout_timer);
}

void motor_control_set_hard_timeout(int hard_timeout)
{
    motor_control_config.hard_timeout = hard_timeout;
}

void motor_set_speed(int motor, float speed, int timeout)
{
    int level_a = 0, level_b = 0;
    int dir_a_pin = 0, dir_b_pin = 0, timer = 0;
    esp_timer_handle_t *stop_timer_handle = NULL;

    if (speed == 0) {
        level_a = 0;
        level_b = 0;
    } else if (speed < 0) {
        level_a = 1;
        level_b = 0;
    } else {
        level_a = 0;
        level_b = 1;
    }

    if (motor == 1) {
        dir_a_pin = MOTOR_1_DIR_A;
        dir_b_pin = MOTOR_1_DIR_B;
        timer = MCPWM_TIMER_0;
        stop_timer_handle = &motor1_timeout_timer;
    } else if (motor == 2) {
        dir_a_pin = MOTOR_2_DIR_A;
        dir_b_pin = MOTOR_2_DIR_B;
        timer = MCPWM_TIMER_1;
        stop_timer_handle = &motor2_timeout_timer;
    }
    gpio_set_level(dir_a_pin, level_a);
    gpio_set_level(dir_b_pin, level_b);

    esp_timer_stop(*stop_timer_handle);

    mcpwm_set_duty(MCPWM_UNIT_0, timer, MCPWM_GEN_A, fabs(speed));

    int calc_timeout = timeout;
    /* Cap the timeout to the hard timeout. */
    if (motor_control_config.hard_timeout > 0) {
        if (calc_timeout > motor_control_config.hard_timeout) {
            calc_timeout = motor_control_config.hard_timeout;
        }
    }
    if (calc_timeout > 0 && fabs(speed) > 0) {
        esp_err_t err;
        err = esp_timer_start_once(*stop_timer_handle, calc_timeout * 1000);
        ESP_ERROR_CHECK(err); // this should never fail.
    }

    printf("Motor %d set to %f speed.\n", motor, speed);
}

void motor_control_stop_motor_callback(void *arg)
{
    int timer = -1;
    int motor = (int)arg;
    if (motor == 1) {
        timer = MCPWM_TIMER_0;
    } else if (motor == 2) {
        timer = MCPWM_TIMER_1;
    }
    if (timer != -1) {
        ESP_LOGI(TAG, "Stopped motor %d.", motor);
        mcpwm_set_duty(MCPWM_UNIT_0, timer, MCPWM_GEN_A, 0);
    } else {
        ESP_LOGE(TAG, "Invalid motor value %d.", motor);
    }
}
