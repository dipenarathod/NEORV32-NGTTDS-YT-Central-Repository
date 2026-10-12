// esp32_at_util.c - helpers to build command strings
#include "esp32_at_util.h"
#include <string.h>

#define AT_CMD_MAX 256
static char cmd[AT_CMD_MAX]; // Common command buffer


// Clear and return pointer to the static command buffer
char *esp32_at_cmd_begin(void)
{ 
    cmd[0] = '\0';
    return cmd;
}

// Append a signed 32-bit integer to a string buffer, returning the pointer to the string
char* append_int32(char *dest, int32_t value) {

    int len = strlen(dest); // Length of the existing string in dest to find the end for appending

    uint32_t uval; //unsigned version of the value to handle negative numbers
    if (value < 0) {
        dest[len++] = '-';
        uval = 0U - (uint32_t)value;
    } else {
        uval = (uint32_t)value;
    }

    char num_buf[11]; //buffer to store character representation of the number (max 10 digits for 32-bit int + null terminator)
    int i = 0;

    if (uval == 0) { // Special case for zero
        num_buf[i++] = '0';
    } else {
        while (uval > 0) {
            num_buf[i++] = (char)('0' + (uval % 10)); // Convert int digit to char and store it in buffer (in reverse order)
            uval /= 10; //Delete the last digit from uval
        }
    }

    // Reverse num_buf into cmd
    for (int j = i - 1; j >= 0; j--) {
        dest[len++] = num_buf[j];
    }

    dest[len] = '\0'; // Null-terminate the command string
    return dest;
}

// Append a comma ',' to a string buffer
char* append_comma(char *dest) {
    int len = strlen(dest);
    dest[len++] = ',';
    dest[len] = '\0';
    return dest;
}

// Append a null-terminated string to a string buffer
char* append_str(char *dest, const char *src) {
    int len = strlen(dest);
    while (*src != '\0') {
        dest[len++] = *src++;
    }
    dest[len] = '\0';
    return dest;
}

// Append a double-quoted string, escaping backslash, double quote and comma as ESP-AT requires
char* append_quoted_str(char *dest, const char *src) {
    int len = strlen(dest);
    dest[len++] = '"'; // Opening quote
    while (*src != '\0') {
        if (*src == '\\' || *src == '"' || *src == ',') {
            dest[len++] = '\\'; // Escape character goes before the special character
        }
        dest[len++] = *src++;
    }
    dest[len++] = '"'; // Closing quote
    dest[len] = '\0';
    return dest;
}