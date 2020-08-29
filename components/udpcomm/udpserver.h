//
// Created by Sami Lehtinen on 15.6.2020.
//

#ifndef ROBOT_FRONTEND_UDPSERVER_H
#define ROBOT_FRONTEND_UDPSERVER_H

typedef void (*MotorActionCallback)(int left_motor_speed, int right_motor_speed);

void udp_server(MotorActionCallback motor_action_cb);

#endif //ROBOT_FRONTEND_UDPSERVER_H
