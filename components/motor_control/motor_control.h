/*
 * Motor control.
 *
 * Allows control of motors individually.
 */

#ifndef MOTOR_CONTROL_H_INCLUDED
#define MOTOR_CONTROL_H_INCLUDED

#include "esp_system.h"

/*
 * Motor timeouts
 * ==============
 *
 * Motor actions run forever and need to be explicitly turned off by setting the motor speed to zero.
 * For convenience and safety, this module also allows setting a timeout value, which will shut down the motor power
 * if the motors are not being guided. This will safeguard against network issues etc.
 *
 * A=action request
 * *=timeout occurred
 * -=motor running
 * .=motor not running
 * S=stop request (action request with motor speeds of 0)
 *
 * every character is 100 ms wide
 *
 * A----A----*.A---A----*.A---A--S.....
 *      ^1   ^2         ^3
 *
 * In the example, the timeout is set to 500 ms. Timeout is reset on 1, as a new action request comes before the
 * timoeut expires. In times 2 and 3 the timeout is activated, and the motor stopped.
 *
 * You can set a global timeout, a so called hard_timeout, which will be the limit for all motor actions. Individual
 * motor speed commands can have timeouts as well, allowing shorter timeouts to be used locally. Typically the global
 * hard timeout would be a fairly large value, like 2 seconds, with the individual timeout being much shorter, like
 * 400 milliseconds.
 *
 */

typedef struct {
    /* Hard timeout for the actions. A single command to set the motor speed will expire after `hard_timeout`
       milliseconds at the latest.  Setting this to zero disables the hard timeout. */
    int hard_timeout;
    /* PWM frequency. Controls how the mcpwm module is initialized internally. */
    int pwm_frequency;
} MotorControlConfig;

#define MotorControlConfig_zero {CONFIG_MC_DEFAULT_HARD_TIMEOUT, CONFIG_MC_DEFAULT_PWM_FREQ};

/*
 * Call this to setup all the necessary libraries and HW to enable motor control. If you are setting this, please use
 * MotorControlConfig_zero to assign defaults.
 *
 * Example code:
 *
 *   MotorControlConfig config = MotorControlConfig_zero;
 *   config.pwm_frequency = 50;
 *   motor_control_setup(&config);
 *
 */
esp_err_t motor_control_setup(MotorControlConfig *config);

/* Allow setting `hard_timeout` later, after calling setup. */
void motor_control_set_hard_timeout(int hard_timeout);

/*
 * Set `motor` to run at `speed`. `motor` is either 1 or 2, and speed should be from -100 to 100, with negative
 * running the motor backwards and positive forwards. Absolute value gives motor power, with 100 being the maximum.
 *
 * If `timeout` is not zero, it will be used as the timeout if it is less than the current `hard_timeout`.
 *
 * Setting motor power to zero will stop the motor and clear the timeout.
 */
void motor_set_speed(int motor, float speed, int timeout);

/*
 * Shutdown the motor control system.
 */
void motor_control_deinit();

#endif /* MOTOR_CONTROL_H_INCLUDED */