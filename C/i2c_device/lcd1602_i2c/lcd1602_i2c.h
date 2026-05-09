#ifndef LCD1602_I2C_H
#define LCD1602_I2C_H

#include <stdint.h>

#define LCD1602_ADDR 0x27
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_HOME 0x02

#define LCD_CMD_ENTRY_DEC_NO_SHIFT 0x04
#define LCD_CMD_ENTRY_DEC_SHIFT 0x05
#define LCD_CMD_ENTRY_INC_NO_SHIFT 0x06
#define LCD_CMD_ENTRY_INC_SHIFT 0x07

#define LCD_CMD_DISPLAY_OFF 0x08
#define LCD_CMD_DISPLAY_ON 0x0C
#define LCD_CMD_CURSOR_ON 0x0E
#define LCD_CMD_BLINK_ON 0x0F

#define LCD_CMD_CURSOR_LEFT 0x10
#define LCD_CMD_CURSOR_RIGHT 0x14
#define LCD_CMD_SCROLL_LEFT 0x18
#define LCD_CMD_SCROLL_RIGHT 0x1C

#define LCD_CMD_FUNCTION_4BIT_1LINE 0x20
#define LCD_CMD_FUNCTION_4BIT_2LINE 0x28
#define LCD_CMD_FUNCTION_8BIT_1LINE 0x30
#define LCD_CMD_FUNCTION_8BIT_2LINE 0x38

#define LCD_CMD_SET_CGRAM_ADDR 0x40
#define LCD_CMD_SET_DDRAM_ADDR 0x80

#define LCD_CMD_LINE1 0x80
#define LCD_CMD_LINE2 0xC0

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define LCD_EN 0x04
#define LCD_RW 0x02
#define LCD_RS 0x01

// Initialize the 16x2 LCD over I2C.
int lcd_init(void);

// Basic LCD control.
int lcd_clear(void);
int lcd_home(void);
int lcd_set_cursor(uint8_t col, uint8_t row);

// Write data to the LCD.
int lcd_write_char(char c);
int lcd_write_string(const char *s);

// Backlight control.
int lcd_backlight_on(void);
int lcd_backlight_off(void);

// Display control.
int lcd_display_on(void);
int lcd_display_off(void);

// Cursor control.
int lcd_cursor_on(void);
int lcd_cursor_off(void);

// Cursor and display shifting.
int lcd_cursor_left(void);
int lcd_cursor_right(void);
int lcd_scroll_left(void);
int lcd_scroll_right(void);

// Text direction and auto-scroll.
int lcd_left_to_right(void);
int lcd_right_to_left(void);
int lcd_autoscroll_on(void);
int lcd_autoscroll_off(void);

// Blink control.
int lcd_blink_on(void);
int lcd_blink_off(void);

#endif