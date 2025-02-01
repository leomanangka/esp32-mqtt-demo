#include "esp_log.h"
#include "wifi_sta.h"

void app_main() {
  static const char *MAIN_TAG = "main_tag";
  wifi_init_sta();
  if (isConnected()) {
    ESP_LOGI(MAIN_TAG, "WiFi Is Connected!!");
  } else {
    ESP_LOGI(MAIN_TAG, "WiFi Is Not Connected!!");
  }
}
