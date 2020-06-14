//
// Created by Sami Lehtinen on 15.6.2020.
//

#ifndef ROBOT_FRONTEND_CONSOLE_H
#define ROBOT_FRONTEND_CONSOLE_H

#include <sys/unistd.h>

// Get characters until a newline is encountered or the buffer is filled.
int my_fgets(char *buf, size_t buf_len, int fd);

#endif //ROBOT_FRONTEND_CONSOLE_H
