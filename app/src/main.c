/*
 * Copyright (c) 2025 Espressif Systems (Shanghai) Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/sys/poweroff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/poweroff.h>
#include <zephyr/drivers/hwinfo.h>
#include <ulp_lp_core.h>
#include <esp_sleep.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define STRIP_NODE		DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)

#define SW0_NODE	DT_ALIAS(sw0)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0x0f, 0x00, 0x00), /* red */
	RGB(0x00, 0x0f, 0x00), /* green */
	RGB(0x00, 0x00, 0x0f), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printf("Button pressed! Enabling ULP wake up and triggering a PMU interrupt on it.\n");
	esp_sleep_enable_ulp_wakeup();
	ulp_lp_core_sw_intr_trigger();
	printf("Entering deep sleep\n");
	sys_poweroff();

}

int main(void)
{
	uint32_t reset_cause;
	size_t color = 0;
	int ret;

	hwinfo_get_reset_cause(&reset_cause);

	switch (reset_cause) {
	case RESET_LOW_POWER_WAKE:
		LOG_INF("Wake up from deep sleep");
		break;
	default:
		LOG_INF("Not a deep sleep reset");
	}

	if (!device_is_ready(strip)) {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return 0;
	}

	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure %s pin %d",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	LOG_INF("Press the button (BOOT) to enter deep sleep");

	while (1) {
		for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
			memset(&pixels, 0x00, sizeof(pixels));
			memcpy(&pixels[cursor], &colors[color], sizeof(struct led_rgb));

			ret = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
			if (ret) {
				LOG_ERR("Couldn't update strip: %d", ret);
			}

			k_sleep(K_MSEC(100));
		}

		color = (color + 1) % ARRAY_SIZE(colors);
	}


	return 0;
}
