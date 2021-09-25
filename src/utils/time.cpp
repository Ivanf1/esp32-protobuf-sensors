#include "utils/time.h"

#include <Arduino.h>
#include <time.h>
#include "esp_log.h"

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    ESP_LOGE(TAG, "Failed to obtain time\n");
    return 0;
  }

  time(&now);
  
  return now;
}