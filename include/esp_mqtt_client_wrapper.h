#ifndef ESP_MQTT_CLIENT_WRAPPER_H
#define ESP_MQTT_CLIENT_WRAPPER_H

typedef struct {
  char *topic;
  char *message;
  int qos;
  int interval;
} mqtt_params_t;

void mqtt_tcp_init(void);
void mqtt_topic_publisher(void *parameters);

#endif
