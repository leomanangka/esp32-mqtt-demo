#include "esp_mqtt_client_wrapper.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MQTT TAG */
static const char *MQTT_TAG = "MQTT";
static bool MQTT_CONNECTED_FLAG = false;
static esp_mqtt_client_handle_t client = NULL;

static void log_error_if_nonzero(const char *msg, int error_code) {
  if (error_code != 0) {
    ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", msg, error_code);
  }
}

/* EVENT HANDLER FOR MQTT*/
static void mqtt_event_handler(void *args, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, id=%" PRIi32 "",
           event_base, event_id);
  esp_mqtt_event_handle_t event = event_data;
  switch ((esp_mqtt_event_id_t)event_id) {
  case MQTT_EVENT_CONNECTED:
    MQTT_CONNECTED_FLAG = true;
    ESP_LOGI(MQTT_TAG, "MQTT Connect Successfully");
    break;
  case MQTT_EVENT_DISCONNECTED:
    MQTT_CONNECTED_FLAG = false;
    ESP_LOGI(MQTT_TAG, "MQTT Disconnected");
    break;
  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(MQTT_TAG, "MQTT Subscribe Topic Successfully, msg=%d",
             event->msg_id);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(MQTT_TAG, "MQTT Unsubscribe Topic, msg=%d", event->msg_id);
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(MQTT_TAG, "MQTT Publish Topic Successfully, msg=%d",
             event->msg_id);
    break;
  case MQTT_EVENT_DATA:
    ESP_LOGI(MQTT_TAG, "MQTT EVENT DATA");
    ESP_LOGI(MQTT_TAG, "TOPIC: %.*s\r\n", event->topic_len, event->topic);
    ESP_LOGI(MQTT_TAG, "DATA: %.*s\r\n", event->data_len, event->data);
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(MQTT_TAG, "MQTT EVENT ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
      log_error_if_nonzero("reported from esp-tls",
                           event->error_handle->esp_tls_last_esp_err);
      log_error_if_nonzero("reported from tls stack",
                           event->error_handle->esp_tls_stack_err);
      log_error_if_nonzero("reported as transport's socket errno",
                           event->error_handle->esp_transport_sock_errno);
      ESP_LOGI(MQTT_TAG, "Last errno string (%s)",
               strerror(event->error_handle->esp_transport_sock_errno));
    }
    break;
  default:
    ESP_LOGI(MQTT_TAG, "Other event id: %d", event->event_id);
    break;
  }
}

void mqtt_tcp_init(void) {
  esp_mqtt_client_config_t mqtt_cfg = {
      .broker.address.uri = CONFIG_MQTT_BROKER_URL,
      .broker.address.port = CONFIG_MQTT_BROKER_PORT,
      .credentials.username = CONFIG_MQTT_AUTH_USERNAME,
      .credentials.authentication.password = CONFIG_MQTT_AUTH_PASSWD,
  };
  client = esp_mqtt_client_init(&mqtt_cfg);
  /* The last argument may be used to pass data to the event handler */
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
                                 NULL);
  esp_mqtt_client_start(client);
}

void mqtt_topic_publisher(void *parameters) {
  mqtt_params_t *mqtt_parameters = (mqtt_params_t *)parameters;
  int publish_interval = 1000;
  if (mqtt_parameters->interval != 0) {
    publish_interval = mqtt_parameters->interval * publish_interval;
  }
  while (true) {
    if (MQTT_CONNECTED_FLAG) {
      esp_mqtt_client_publish(client, mqtt_parameters->topic,
                              mqtt_parameters->message, 0, mqtt_parameters->qos,
                              0);
      vTaskDelay(publish_interval / portTICK_PERIOD_MS);
    }
  }
}
