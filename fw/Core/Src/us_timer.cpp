/*
 * usTimer.cpp
 *
 *  Created on: Feb 23, 2023
 *      Author: main
 */

#include "main.h"
#include "us_timer.hpp"
#include "handles.hpp"
#include "irq_lock.hpp"

volatile uint32_t repetitions = 0;
volatile uint32_t lastVal = 0;


void us_timer_init()
{
	repetitions = 0;
	lastVal = 0;
	HAL_TIM_Base_Start(htim_micros);
}

int64_t micros()
{
	static int64_t result;
	static uint32_t currentVal;

	{
		IrqLock lock;
		currentVal = __HAL_TIM_GetCounter(htim_micros);
		if (currentVal < lastVal)
			++repetitions;
		result = ((int64_t)repetitions << 32) + currentVal;
		lastVal = currentVal;
	}
	return result;
}
