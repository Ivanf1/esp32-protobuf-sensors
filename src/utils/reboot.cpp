#include "utils/reboot.h"

#include <Arduino.h>
#include "esp_log.h"

void rebootWithMsg(const char* msg, int wait_time_before_reboot) {
  ESP_LOGE(TAG, "%s\n Rebooting in %d ms", msg, wait_time_before_reboot);
  delay(wait_time_before_reboot);
  ESP.restart();
}