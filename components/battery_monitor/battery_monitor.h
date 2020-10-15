//
// Created by Sami Lehtinen on 6.10.2020.
//

#ifndef AI_ROBOT_BATTERY_MONITOR_H
#define AI_ROBOT_BATTERY_MONITOR_H

void battery_monitor_setup();

float battery_monitor_get_voltage();

float battery_monitor_get_startup_voltage();

#endif //AI_ROBOT_BATTERY_MONITOR_H
