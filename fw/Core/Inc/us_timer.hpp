/*
 * us_timer.hpp
 *
 *  Created on: Feb 23, 2023
 *      Author: main
 */

#ifndef INC_USTIMER_HPP_
#define INC_USTIMER_HPP_

#include "stm32h7xx_hal.h"

void us_timer_init();
int64_t micros();  // must be called at least once every 2*32 us (one hour) to prevent unnoticed overflow

#endif /* INC_USTIMER_HPP_ */
