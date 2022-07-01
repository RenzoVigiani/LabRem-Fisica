Arduino library for creating hexdumps from program memory and view these on the serial monitor.

Initially designed for ESP8266, therefore uses `printf()` instead of standard Arduino `print()` and `println()` methods.
`printf()` is more flexible and more powerful but uses more memory which should be less of an issue on modern 32-bit microcontrollers.

For some microprocessor platforms / Arduino cores `printf()` needs to be explitly enabled. For others like ESP32 and ESP8266 this is default enabled.
