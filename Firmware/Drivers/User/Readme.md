This directory contains the user-drivers:

- adc: ADC Driver. Used to read battery voltage
- key: Key driver. Reads the keys
- led: LED driver. Displays GPS-Fix, Transmit-in-progress, battery voltage
- radio: radio transmitter driver. Implements PLB protocol and sends data
- uart: Uart driver. Used in gps, usb, ble
- watchdog: watchdog driver. Configures watchdog
- forceFeedback: force-feedback-driver