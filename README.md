# ESP32-C3 Low Level UART Bridge

The ESP32-C3 UART Bridge is an ESP-IDF project. This makes the ESP32-C3 act as USB to serial UART interface like FT232R or CP210x.
The communication is transferred in both directions between the Computer and the target MCU through the ESP32-C3 UART Bridge.

> NOTE: Firmware tested on ESP32-C3. But it may work in other ESP32 family.

See diagram below:

```
 COMPUTER            ESP32-C3             MCU
┌────────┐     ┌────────┬────────┐     ┌────────┐
│        │     │        │        │     │        │
│  USB   │◄───►│  CDC   │  UART  │◄───►│  UART  │
│        │     │        │        │     │        │
└────────┘     └────────┴────────┘     └────────┘
```

This code is modified to use low level USB I/O instead of Serial JTAG Read Driver. See [USB Serial JTAG Read Bug](https://github.com/electronicayciencia/esp32-misc/tree/master/usb_serial_jtag_read)


### Hardware Required

The example can be run on ESP32-C3 based development board connected to a computer with a single USB cable for flashing and bridging. The target should have 3.3V logic.


### Setup the Hardware

Connect the target MCU serial interface to the ESP32-C3 as follows.

Note: **UART0** is also console output. So connect the target device after the Bridge has already started.

-------------------------------------------------------------------------
| ESP32-C3 chip         | Kconfig       | ESP32-C3 Pin | Target MCU Pin |
| ----------------------|---------------|--------------|----------------|
| UART Port             | UART_PORT_NUM |     0        |                |
| Transmit Data (TxD)   | UART_TXD      | GPIO21       | RxD            |
| Receive Data (RxD)    | UART_RXD      | GPIO20       | TxD            |


See connection diagram below:

```
 ESP32-C3           MCU
┌─────────┐       ┌────────┐
│ GPIO Tx ├──────►│  RxD   │
│         │ 3.3v  │        │
│         │ logic │        │
│ GPIO Rx │◄──────┤  TxD   │
└─────────┘       └────────┘
```

To prevent debug messages to reach target device, use UART1. For example:

-------------------------------------------------------------------------
| ESP32-C3 chip         | Kconfig       | ESP32-C3 Pin | Target MCU Pin |
| ----------------------|---------------|--------------|----------------|
| UART Port             | UART_PORT_NUM |     1        |                |
| Transmit Data (TxD)   | UART_TXD      | GPIO4        | RxD            |
| Receive Data (RxD)    | UART_RXD      | GPIO5        | TxD            |


## How to Compile the Project

[ESP-IDF](https://github.com/espressif/esp-idf) v5.0.1 or newer can be used to compile the project. Please read the
documentation of ESP-IDF for setting up the environment.

```
idf.py set-target esp32c3
idf.py menuconfig
idf.py build
idf.py flash
```

Options for menuconfig:

- Component config → ESP System Settings → Channel for console output: **Default: UART0**
- Component config → ESP System Settings → Channel for console secondary output: **No secondary console**
- The project-specific settings are in the "ESP32 Uart Bridge Configuration" sub-menu.

Use the command below to configure project using Kconfig menu as showed in the table above.
The default Kconfig values can be changed such as: `UBRIDGE_UART_BAUD_RATE`, `UBRIDGE_UART_PARITY`,  `UBRIDGE_UART_PORT_NUM` (Refer to Kconfig file).

```
idf.py menuconfig
```

The default baud rate is `115200` and port number `0` (GPIO Rx 20, Tx 21).

Note ESP32-C3's CDC ACM class does not support set line mode. So the baud rate is fixed in compile time.

