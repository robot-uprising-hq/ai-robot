#include "motor_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/mcpwm.h" 

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

float motor_1_pwm = 0;
float motor_2_pwm = 0;

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
    mcpwm_config_t pwm_config;
    
    pwm_config.frequency = 1000;    //frequency = 1000Hz
    pwm_config.cmpr_a = 0.0;       // initial duty cycle 0
    pwm_config.cmpr_b = 0.0;       // initial duty cycle 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
}

void motor_control_setup()
{
    gpio_init();
    pwm_gpio_config();
    mcpwm_config();
}

void motor_set_speed(int motor, float speed)
{
    int level_a = 0, level_b = 0;
    int dir_a_pin = 0, dir_b_pin = 0, timer = 0;

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
    } else if (motor == 2) {
        dir_a_pin = MOTOR_2_DIR_A;
        dir_b_pin = MOTOR_2_DIR_B;
        timer = MCPWM_TIMER_1;
    }
    gpio_set_level(dir_a_pin, level_a);
    gpio_set_level(dir_b_pin, level_b);

    mcpwm_set_duty(MCPWM_UNIT_0, timer, MCPWM_GEN_A, fabs(speed));

    printf("Motor %d set to %f speed.\n", motor, speed);
}
