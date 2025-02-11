#include "esp_log.h"
#include "esp_mqtt_client_wrapper.h"
#include "wifi_sta.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

void app_main() {
  static const char *MAIN_TAG = "main_tag";
  wifi_init_sta();
  if (isConnected()) {
    ESP_LOGI(MAIN_TAG, "WiFi Is Connected!!");

    // task need a pass struct as static or global variable
    static mqtt_params_t mqtt_publish_params = {
        .topic = "test/mqtt_pub",
        .message = "Hello",
        .qos = 1,       // At least once
        .interval = 10, // interval for 10 second
    };

    mqtt_tcp_init();
    xTaskCreate(mqtt_topic_publisher, "publish_task", 1024 * 2,
                &mqtt_publish_params, 1, NULL);
  } else {
    ESP_LOGI(MAIN_TAG, "WiFi Is Not Connected!!");
  }
}
