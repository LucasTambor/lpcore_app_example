# LP Core Example Application

This project demonstrates a simple interaction between the High-Performance (HP) Core and the Low-Power (LP) Core on the ESP32-C6 DevKit.

**HP Core Behavior:**

The HP Core continuously rotates the colors of an RGB LED.

When the Boot button is pressed, it triggers an interrupt that:

Sends an interrupt to the LP Core.

Puts the HP Core into deep sleep mode, causing the RGB LED to stop rotating.

**LP Core Behavior:**

On boot, the LP Core sends a "Hello World!" message to the console with the identifier: `esp32c6_devkitc/esp32c6/lpcore`.

It then waits for an interrupt from the HP Core (triggered by the Boot button press).

When the interrupt is received, the LP Core waits for 5 seconds and then wakes up the HP Core.

The LP Core only resets when the device is powered up.

## Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the ``lpcore_app_example`` and all Zephyr modules will be cloned. Run the following
command:

```shell
# initialize the workspace (main branch)
west init -m https://github.com/LucasTambor/lpcore_app_example --mr main my-workspace
# update Zephyr modules
cd my-workspace
west update
```

## Building and running

To build the application, run the following command:

```shell
cd lpcore_app_example
west build -p -b esp32c6_devkitc/esp32c6/hpcore app --sysbuild
```

Once you have built the application, run the following command to flash it:

```shell
west flash && west espressif monitor
```

## LP Core Console

Connect an UART to USB adapter to the LP Core's LP UART pins (TX: GPIO 5, RX: GPIO 4, GND: GND) and open a terminal emulator.
