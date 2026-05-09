#include "lcd1602_i2c.h"
#include "../i2c_helper/i2c_helper.h"
#include <neorv32.h>
#include <stddef.h>
#include <stdint.h>

/*
Refer to this HD44780 explaination for all I2C commands:
https://dawes.wordpress.com/2010/01/05/hd44780-instruction-set/
https://www.electronicsforu.com/technology-trends/learn-electronics/16x2-lcd-pinout-diagram
Referred to the following repos to write this library:
https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.h
Used ChatGPT's help
*/

// Private module state
static uint8_t lcd_addr = 0;
static uint8_t lcd_backlight_val = LCD_BACKLIGHT;
static uint8_t lcd_displaycontrol = LCD_CMD_DISPLAY_ON;
static uint8_t lcd_displaymode = LCD_CMD_ENTRY_INC_NO_SHIFT;
static uint8_t lcd_numlines = 2;
static uint8_t lcd_initialized = 0;

static void lcd_delay_ms(uint32_t ms)
{
    neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), ms);
}

// Write one output byte to the LCD I2C interface
static int lcd_write_io(uint8_t data)
{
    uint8_t tx = (uint8_t)(data | lcd_backlight_val);
    return i2c_write(lcd_addr, &tx, 1);
}

// Pulse EN high then low so the LCD latches the nibble
static int lcd_pulse_enable(uint8_t data)
{
    if (lcd_write_io((uint8_t)(data | LCD_EN)) != 0)
    {
        return -1;
    }

    lcd_delay_ms(1);

    if (lcd_write_io((uint8_t)(data & (uint8_t)~LCD_EN)) != 0)
    {
        return -2;
    }

    lcd_delay_ms(1);
    return 0;
}

static int lcd_write4bits(uint8_t value)
{
    if (lcd_write_io(value) != 0)
    {
        return -1;
    }

    return lcd_pulse_enable(value);
}

static int lcd_send(uint8_t value, uint8_t mode)
{
    uint8_t high_nib = (uint8_t)(value & 0xF0);
    uint8_t low_nib = (uint8_t)((value << 4) & 0xF0);

    if (lcd_write4bits((uint8_t)(high_nib | mode)) != 0)
    {
        return -1;
    }

    if (lcd_write4bits((uint8_t)(low_nib | mode)) != 0)
    {
        return -2;
    }

    return 0;
}

static int lcd_command(uint8_t value)
{
    return lcd_send(value, 0);
}

static int lcd_detect_address(void)
{
    if (i2c_ping(LCD1602_ADDR) == 0)
    {
        lcd_addr = LCD1602_ADDR;
        return 0;
    }

    return -1;
}

int lcd_init(void)
{
    if (lcd_detect_address() != 0)
    {
        return -1;
    }

    lcd_displaycontrol = LCD_CMD_DISPLAY_ON;
    lcd_displaymode = LCD_CMD_ENTRY_INC_NO_SHIFT;
    lcd_numlines = 2;

    // Wait for LCD power-up
    lcd_delay_ms(50);

    // Initialize interface output state
    if (lcd_write_io(lcd_backlight_val) != 0)
    {
        return -2;
    }

    lcd_delay_ms(5);

    
    //LCD 4 - bit initialization sequence : Start in 8-bit wakeup mode, then switch to 4-bit mode. 
    if (lcd_write4bits(0x03 << 4) != 0)
    {
        return -3;
    }
    lcd_delay_ms(5);

    if (lcd_write4bits(0x03 << 4) != 0)
    {
        return -4;
    }
    lcd_delay_ms(5);

    if (lcd_write4bits(0x03 << 4) != 0)
    {
        return -5;
    }
    lcd_delay_ms(2);

    if (lcd_write4bits(0x02 << 4) != 0)
    {
        return -6;
    }
    lcd_delay_ms(2);

    // Function set: 4-bit, 2-line
    if (lcd_command(LCD_CMD_FUNCTION_4BIT_2LINE) != 0)
    {
        return -7;
    }

    // Display off while configuring
    if (lcd_command(LCD_CMD_DISPLAY_OFF) != 0)
    {
        return -8;
    }

    // Clear display
    if (lcd_command(LCD_CMD_CLEAR) != 0)
    {
        return -9;
    }
    lcd_delay_ms(2);

    // Entry mode: increment cursor, no display shift
    if (lcd_command(LCD_CMD_ENTRY_INC_NO_SHIFT) != 0)
    {
        return -10;
    }

    // Return home
    if (lcd_command(LCD_CMD_HOME) != 0)
    {
        return -11;
    }
    lcd_delay_ms(2);

    // Display on, cursor off, blink off
    if (lcd_command(LCD_CMD_DISPLAY_ON) != 0)
    {
        return -12;
    }

    lcd_initialized = 1;
    return 0;
}

int lcd_write_char(char c)
{
    if (lcd_initialized == 0)
    {
        return -1;
    }

    if (lcd_send((uint8_t)c, LCD_RS) != 0)
    {
        return -2;
    }

    return 0;
}

int lcd_set_cursor(uint8_t col, uint8_t row)
{
    if (lcd_initialized == 0)
    {
        return -1;
    }

    if (row > 1)
    {
        return -2;
    }

    if (col > 15)
    {
        return -3;
    }

    if (row == 0)
    {
        return lcd_command((uint8_t)(LCD_CMD_LINE1 + col));
    }
    else
    {
        return lcd_command((uint8_t)(LCD_CMD_LINE2 + col));
    }
}

int lcd_write_string(const char *s)
{
    if (lcd_initialized == 0)
    {
        return -1;
    }

    if (s == NULL)
    {
        return -2;
    }

    while (*s != '\0')
    {
        if (lcd_write_char(*s) != 0)
        {
            return -3;
        }
        s++;
    }

    return 0;
}

int lcd_display_on(void)
{
    lcd_displaycontrol |= 0x04;
    return lcd_command(lcd_displaycontrol);
}

int lcd_display_off(void)
{
    lcd_displaycontrol &= (uint8_t)~0x04;
    return lcd_command(lcd_displaycontrol);
}

int lcd_cursor_on(void)
{
    lcd_displaycontrol |= 0x02;
    return lcd_command(lcd_displaycontrol);
}

int lcd_cursor_off(void)
{
    lcd_displaycontrol &= (uint8_t)~0x02;
    return lcd_command(lcd_displaycontrol);
}

int lcd_blink_on(void)
{
    lcd_displaycontrol |= 0x01;
    return lcd_command(lcd_displaycontrol);
}

int lcd_blink_off(void)
{
    lcd_displaycontrol &= (uint8_t)~0x01;
    return lcd_command(lcd_displaycontrol);
}

int lcd_cursor_left(void)
{
    return lcd_command(LCD_CMD_CURSOR_LEFT);
}

int lcd_cursor_right(void)
{
    return lcd_command(LCD_CMD_CURSOR_RIGHT);
}

int lcd_scroll_left(void)
{
    return lcd_command(LCD_CMD_SCROLL_LEFT);
}

int lcd_scroll_right(void)
{
    return lcd_command(LCD_CMD_SCROLL_RIGHT);
}

int lcd_left_to_right(void)
{
    lcd_displaymode |= 0x02;
    return lcd_command(lcd_displaymode);
}

int lcd_right_to_left(void)
{
    lcd_displaymode &= (uint8_t)~0x02;
    return lcd_command(lcd_displaymode);
}

int lcd_autoscroll_on(void)
{
    lcd_displaymode |= 0x01;
    return lcd_command(lcd_displaymode);
}

int lcd_autoscroll_off(void)
{
    lcd_displaymode &= (uint8_t)~0x01;
    return lcd_command(lcd_displaymode);
}

int lcd_clear(void)
{
    if (lcd_command(LCD_CMD_CLEAR) != 0)
    {
        return -1;
    }
    lcd_delay_ms(2);
    return 0;
}

int lcd_home(void)
{
    if (lcd_command(LCD_CMD_HOME) != 0)
    {
        return -1;
    }
    lcd_delay_ms(2);
    return 0;
}

int lcd_backlight_on(void)
{
    lcd_backlight_val = LCD_BACKLIGHT;
    return lcd_write_io(0x00);
}

int lcd_backlight_off(void)
{
    lcd_backlight_val = LCD_NOBACKLIGHT;
    return lcd_write_io(0x00);
}