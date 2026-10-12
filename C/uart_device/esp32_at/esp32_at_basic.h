#ifndef ESP32_AT_BASIC_H
#define ESP32_AT_BASIC_H

#include <stdint.h>
#include "esp32_at_core.h"

esp32_at_status_t esp32_at_reset(void); // Trigger hardware reset command and wait for boot completion message
esp32_at_status_t esp32_at_restore_factory(void); // Reset device configurations back to default settings
esp32_at_status_t esp32_at_deep_sleep(uint32_t sleep_ms); // Request ESP32 enter low-power deep sleep mode and wait until it wakes up and reboots



#endif  