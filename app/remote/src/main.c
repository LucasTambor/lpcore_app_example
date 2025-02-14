/*
 * Copyright (c) 2025 Espressif Systems (Shanghai) Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <ulp_lp_core_utils.h>
#include "ulp_lp_core_interrupts.h"

volatile uint32_t lp_core_pmu_intr_count;
volatile bool lp_wake_hp_flag;

void ulp_lp_core_lp_pmu_intr_handler(void)
{
	ulp_lp_core_sw_intr_clear();
	lp_wake_hp_flag = true;
	printf("LP Core PMU Interrupt %d\n", ++lp_core_pmu_intr_count);

}

int main(void)
{
	lp_core_pmu_intr_count = 0;
	lp_wake_hp_flag = false;

	ulp_lp_core_intr_enable();
	ulp_lp_core_sw_intr_enable(true);

	printf("Hello World! %s\r\n", CONFIG_BOARD_TARGET);

	while(1) {
		if(lp_wake_hp_flag) {
			lp_wake_hp_flag = false;
			printf("Waking HP Core in 5 seconds\n");
			k_sleep(K_SECONDS(5));
			ulp_lp_core_wakeup_main_processor();
		}
	}

	return 0;
}
