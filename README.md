# LP Core Example Application

This is a simple example application that demonstrates how to use the ESP32C6's LP Core on Zephyr

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
