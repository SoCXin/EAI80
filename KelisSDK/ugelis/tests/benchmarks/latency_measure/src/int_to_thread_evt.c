/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *
 * @brief measure time from ISR to a rescheduled thread
 *
 * This file contains test that measures time to switch from an interrupt
 * handler to executing a thread after rescheduling. In other words, execution
 * after interrupt handler resume in a different thread than the one which got
 * interrupted.
 */

#include <ugelis.h>
#include <irq_offload.h>

#include "timestamp.h"
#include "utils.h"

#include <arch/cpu.h>

static u32_t timestamp;


K_SEM_DEFINE(INTSEMA, 0, 1);
K_ALERT_DEFINE(EVENT0, NULL, 10);

/**
 *
 * @brief Test ISR used to measure best case interrupt latency
 *
 * The interrupt handler gets the second timestamp.
 *
 * @return N/A
 */
static void latency_test_isr(void *unused)
{
	ARG_UNUSED(unused);

	k_alert_send(&EVENT0);
	timestamp = TIME_STAMP_DELTA_GET(0);
}

/**
 *
 * @brief Software interrupt generating thread
 *
 * Lower priority thread that, when it starts, it waits for a semaphore. When
 * it gets it, released by the main thread, sets up the interrupt handler and
 * generates the software interrupt
 *
 * @return 0 on success
 */
void int_thread(void)
{
	k_sem_take(&INTSEMA, K_FOREVER);
	irq_offload(latency_test_isr, NULL);
	k_thread_suspend(k_current_get());
}


K_THREAD_DEFINE(int_thread_id, 512,
		(k_thread_entry_t) int_thread, NULL, NULL, NULL,
		11, 0, K_NO_WAIT);

/**
 *
 * @brief The test main function
 *
 * @return 0 on success
 */
int int_to_thread_evt(void)
{
	PRINT_FORMAT(" 2 - Measure time from ISR to executing a different thread"
		     " (rescheduled)");
	TICK_SYNCH();
	k_sem_give(&INTSEMA);
	k_alert_recv(&EVENT0, K_FOREVER);
	timestamp = TIME_STAMP_DELTA_GET(timestamp);
	PRINT_FORMAT(" switch time is %u tcs = %u nsec",
		     timestamp, SYS_CLOCK_HW_CYCLES_TO_NS(timestamp));
	return 0;
}
