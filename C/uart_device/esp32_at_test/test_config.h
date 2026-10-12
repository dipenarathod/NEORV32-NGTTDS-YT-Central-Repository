// test_config.h - your setup. Do not commit this file / do not share it with real credentials.
#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#define WIFI_SSID      "RATHODS HOME Sub"
#define WIFI_PASS      "7738747628"

#define PC_IP          "192.168.0.99" // PC running "nc -lvp 5000" and "python3 -m http.server 8000"
#define PC_TCP_PORT    5000
#define PC_HTTP_PORT   8000
#define PC_MQTT_PORT   1883 // PC running a Mosquitto broker

#endif