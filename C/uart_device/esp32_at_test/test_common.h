#ifndef TEST_COMMON_H
#define TEST_COMMON_H

int test_setup(void);     // UART0 + ESP32 init, returns 0 on success
int test_wifi_join(void); // Station mode + join the AP from test_config.h, returns 0 on success

#endif