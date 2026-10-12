#include "esp32_at_basic.h"
#include "esp32_at_port.h"
#include "esp32_at_util.h"
#include <string.h>

// Trigger hardware reset command and wait for boot completion message
esp32_at_status_t esp32_at_reset(void)
{ 
    esp32_at_flush_rx();
    esp32_at_tx("AT+RST\r\n");
    return esp32_at_wait_for("ready\r\n", NULL, 0, ESP32_AT_RESET_TIMEOUT_MS);
}

// Reset device configurations back to default settings
esp32_at_status_t esp32_at_restore_factory(void)
{
    esp32_at_status_t status = esp32_at_send_cmd("AT+RESTORE", NULL, 0, ESP32_AT_DEFAULT_TIMEOUT_MS);
    if (status == ESP32_AT_OK) {
        return esp32_at_wait_for("ready\r\n", NULL, 0, ESP32_AT_RESET_TIMEOUT_MS);
    }
    return status;
}

// Request ESP32 enter low-power deep sleep mode and wait until it wakes up and reboots
esp32_at_status_t esp32_at_deep_sleep(uint32_t sleep_ms)
{ 
    char *cmd = esp32_at_cmd_begin();
    
    // Explicit manual construction avoiding heavy printf overhead
    strcpy(cmd, "AT+GSLP=");
    
    append_int32(cmd, sleep_ms);

    //snprintf(cmd, sizeof(cmd), "AT+GSLP=%u", (unsigned int)sleep_ms);
    esp32_at_status_t status = esp32_at_send_cmd(cmd, NULL, 0, ESP32_AT_DEFAULT_TIMEOUT_MS);
    if (status != ESP32_AT_OK) {
        return status;
    }
    // The ESP32 sleeps for sleep_ms, then reboots and prints its boot log ending with "ready"
    // Echo is back on after this reboot, so the caller must send ATE0 again if needed
    return esp32_at_wait_for("ready\r\n", NULL, 0, sleep_ms + ESP32_AT_RESET_TIMEOUT_MS);
}