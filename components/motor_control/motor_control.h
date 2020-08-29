/*
 * Motor control.
 *
 * Allows control of motors individually.
 */

#ifndef MOTOR_CONTROL_H_INCLUDED
#define MOTOR_CONTROL_H_INCLUDED

/*
 * TODO: For convenience and safety, this module should also handle the shutting down of motor power if the motors
 *       are not being guided. This will safeguard against network issues etc. This backoff timeout could be given as
 *       a parameter to the setup function, for example, or be an argument to the motor_set_speed() function.
 */

/* Call this to setup all the necessary libraries and HW to enabled motor control. */
void motor_control_setup();

/*
 * Set `motor` to run at `speed`. `motor` is either 1 or 2, and speed should be from -100 to 100, with negative
 * running the motor backwards and positive forwards. Absolute value gives motor power, with 100 being the maximum.
 */
void motor_set_speed(int motor, float speed);

#endif /* MOTOR_CONTROL_H_INCLUDED */