//
// Created by fadi on 2022-03-13.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <time.h>
#include <stdint-gcc.h>

int open_joystick(char *device_name) {
    int fd = -1;

    if (device_name == NULL) {
        return fd;
    }

    fd = open(device_name, O_RDONLY | O_NONBLOCK);

    if (fd < 0) {
        printf("Could not locate joystick device %s\n", device_name);
        exit(1);
    }
    return fd;
}

void print_device_info(int fd) {
    int axes = 0, buttons = 0;
    char name[128];

    ioctl(fd, JSIOCGAXES, &axes);
    ioctl(fd, JSIOCGBUTTONS, &buttons);
    ioctl(fd, JSIOCGNAME(sizeof(name)), &name);

    printf("%s\n  %d Axes %d Buttons\n", name, axes, buttons);
}

void process_event(struct js_event jse, int fd) {
//    printf("number: %d\n", jse.number);
//    printf("value: %d\n", jse.value);
//    printf("type: %d\n", jse.type);
    struct timespec ts;
    uint16_t delay = 50;
    ts.tv_sec = 0;
    ts.tv_nsec = delay * 10000000;


    if (jse.type == 2) {
        if (jse.number == 7) {
            if (jse.value < 0) {
                printf("UP\n");
                nanosleep(&ts, NULL);
                read(fd, &jse, sizeof(jse));
                if (jse.value != 0) {
                    while (1) {
                        read(fd, &jse, sizeof(jse));
                        if (jse.value == 0) {
                            break;
                        }
                        printf("UP\n");
                    }
                }
            } else if (jse.value > 0) {
                printf("Down\n");
            }
        } else if (jse.number == 6) {
            if (jse.value < 0) {
                printf("Left\n");
            } else if (jse.value > 0) {
                printf("Right\n");
            }
        }
    }
    if (jse.type == 1 && jse.value > 0) {
        printf("%d\n", jse.number);
    }
}

int main() {
    int fd;
    struct js_event jse;

    fd = open_joystick("/dev/input/js1");
    print_device_info(fd);

    while (1) {
        while (read(fd, &jse, sizeof(jse)) > 0) {
            process_event(jse, fd);
        }
    }
    return 0;
}