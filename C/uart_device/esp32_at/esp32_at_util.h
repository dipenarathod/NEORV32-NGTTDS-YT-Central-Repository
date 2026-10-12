
// esp32_at_util.h - helpers to build command strings (no printf needed)
#ifndef ESP32_AT_UTIL_H
#define ESP32_AT_UTIL_H
 
#include <stdint.h>

char *esp32_at_cmd_begin(void); // Clear and return pointer to the static command buffer

char* append_int32(char *dest, int32_t value); // Append a signed 32-bit integer to a string buffer, returning the pointer to the string
char* append_comma(char *dest); // Append a comma ',' to a string buffer
char* append_str(char *dest, const char *src); // Append a null-terminated string to a string buffer
char* append_quoted_str(char *dest, const char *src); // Append a double-quoted string, escaping backslash, double quote and comma as ESP-AT requires

#endif