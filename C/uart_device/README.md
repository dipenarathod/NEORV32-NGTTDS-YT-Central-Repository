## UART Libraries for NEORV32

Written for March 7, 2026 build of NEORV32.

### ESP32 AT Library for NEORV32 (esp32_at)
This library provides a simple interface to use AT commands with an ESP32 using UART.

**Board:**: ESP32 WROOM Devkit
**ESP AT Version:**: 4.1.1.0
**Baud Rate**: 115200

#### Connection between UART 1 Pins on ESP32 and NEORV32:
`GPIO16 (RX2)` -` UART1 TX` pin specified in the FPGA constraints file\
`GPIO17 (TX2)` - `UART1 RX` pin specified in the FPGA constraints file\
`GPIO15 (CTS)` - Not used\
`GPIO14 (RTS)` - Not used

`GPIO16` - labelled as `RX2` on my ESP32 board\
`GPIO17` - labelled as `TX2` on my ESP32 board

#### Relevant Links
**Connect serial port adapter to UART 1 of ESP32**: https://docs.espressif.com/projects/esp-at/en/latest/esp32/Get_Started/Hardware_connection.html
Note: I connected UART1 of NEORV32 to these pins.

**Flash ESP AT Firmware and check if its working**: https://docs.espressif.com/projects/esp-at/en/latest/esp32/Get_Started/Downloading_guide.html#flash-factory-linux

**Note:** You can use the USB port (UART0) of the devkit for flashing the AT firmware.

#### Porting the ESP-AT Library to Other MCUs
You must start with [neorv32_at_port.c](./esp32_at/esp32_at_port.c)
Only this file uses NEORV32-specific methods. You can modify this file as needed.

### ESP32 AT Library Tests for NEORV32 (esp32_at_test)
Example tests for various use cases using the ESP-AT commands.
In the folder makefile, look for line: 
```bash
APP_SRC += test_http_get.c # Replace with whatever test you want to run
```

Change the test file with whatever you want to try. For example:
```bash
APP_SRC += test_tcp_client.c
```

#### How to run
1) Generate the neorv32_exe.bin file for bootloader upload.
```bash
make clean_all
make exe
```
2) Upload the neorv32_exe.bin file to NEORV32 using any utility like GTKTerm.



