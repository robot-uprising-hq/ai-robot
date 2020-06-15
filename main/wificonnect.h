//
// Created by Sami Lehtinen on 15.6.2020.
//

#ifndef ROBOT_FRONTEND_WIFICONNECT_H
#define ROBOT_FRONTEND_WIFICONNECT_H

#include "esp_system.h"

/*
 * Connect to wifi. Should only be called from main loop once at a time.
 */
void wifi_init_sta(const char *ssid, const char *passwd);

/*
 * Get interface IP(v4) address.
 */
esp_err_t get_ip_addr(char *buf, size_t buf_len);

#endif //ROBOT_FRONTEND_WIFICONNECT_H
