// Test 6: MQTT. On the PC run a Mosquitto broker that accepts LAN clients, then:
//   mosquitto_sub -h <PC_IP> -t neorv32/out        (see what the board publishes)
//   mosquitto_pub -h <PC_IP> -t neorv32/in -m hi   (the board prints the received message)
// Mosquitto 2.x only listens on localhost by default. Add to mosquitto.conf:
//   listener 1883
//   allow_anonymous true
#include <neorv32.h>
#include <string.h>
#include "esp32_at.h"
#include "test_common.h"
#include "test_config.h"

static bool mqtt_lost = false;

// Callback function to handle incoming MQTT messages and disconnection events.
// line =  The received URC received from the ESP32 AT interface.
static void on_line(const char *line)
{
    neorv32_uart0_printf("[URC] %s\n", line); // Incoming messages look like +MQTTSUBRECV:0,"topic",len,data
    if (strncmp(line, "+MQTTDISCONNECTED", 17) == 0) mqtt_lost = true;
}


// Connect to the MQTT broker using the ESP32 AT interface.
// This function sends the necessary AT commands to establish a connection with the MQTT broker and subscribe to a topic.
static esp32_at_status_t mqtt_connect(void)
{
    // AT+MQTTCONN=<link>,"host",<port>,<auto reconnect: 0 = we do it ourselves>
    char *c = esp32_at_cmd_begin();
    strcpy(c, "AT+MQTTCONN=0,");
    append_quoted_str(c, PC_IP);
    append_comma(c);
    append_int32(c, PC_MQTT_PORT);
    append_str(c, ",0");
    esp32_at_status_t st = esp32_at_send_cmd(c, NULL, 0, 10000);
    if (st != ESP32_AT_OK) {
        return st;
    }
    return esp32_at_send_cmd("AT+MQTTSUB=0,\"neorv32/in\",0", NULL, 0, 5000);
}

// Publish a message to the MQTT broker using the ESP32 AT interface.
// This function sends the necessary AT commands to publish a message to a specified topic.
// count = The counter value to be included in the message payload.
static esp32_at_status_t mqtt_publish(uint32_t count)
{
    char msg[24];
    strcpy(msg, "hello ");
    append_int32(msg, (int32_t)count);

    // AT+MQTTPUB=0,"topic","payload",qos,retain
    char *c = esp32_at_cmd_begin();
    strcpy(c, "AT+MQTTPUB=0,");
    append_quoted_str(c, "neorv32/out");
    append_comma(c);
    append_quoted_str(c, msg);
    append_str(c, ",0,0");
    return esp32_at_send_cmd(c, NULL, 0, 5000);
}

int main(void)
{
    if (test_setup() != 0) return 1;
    if (test_wifi_join() != 0) return 1;

    esp32_at_set_urc_handler(on_line);

    // AT+MQTTUSERCFG=<link>,<scheme: 1 = MQTT over TCP>,"client id","user","pass",0,0,""
    esp32_at_status_t st = esp32_at_send_cmd("AT+MQTTUSERCFG=0,1,\"neorv32\",\"\",\"\",0,0,\"\"", NULL, 0, 2000);
    neorv32_uart0_printf("MQTTUSERCFG: %s\n", esp32_at_status_str(st));

    st = mqtt_connect();
    neorv32_uart0_printf("MQTT connect + subscribe: %s\n", esp32_at_status_str(st));
    mqtt_lost = (st != ESP32_AT_OK); // Retry below if the first attempt failed

    uint32_t tick = 0, count = 0;
    while (1) {
        esp32_at_poll();

        if (mqtt_lost) {
            esp32_at_delay_ms(2000);
            st = mqtt_connect();
            neorv32_uart0_printf("reconnect: %s\n", esp32_at_status_str(st));
            mqtt_lost = (st != ESP32_AT_OK);
        } else if (++tick % 50 == 0) { // Every 5 s
            st = mqtt_publish(count++);
            neorv32_uart0_printf("publish: %s\n", esp32_at_status_str(st));
        }
        esp32_at_delay_ms(100);
    }
}