#ifndef ESP32PROTOBUF_REBOOT_H
#define ESP32PROTOBUF_REBOOT_H

void rebootWithMsg(const char* msg, int wait_time_before_reboot = 3000);

#endif