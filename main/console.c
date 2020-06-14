//
// Created by Sami Lehtinen on 15.6.2020.
//

#include "console.h"
#include <sys/param.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/errno.h>
#include "esp_log.h"

static const char* TAG = "robotfrontend";

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
