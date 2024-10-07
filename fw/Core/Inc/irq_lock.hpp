#ifndef INC_IRQLOCK_HPP_
#define INC_IRQLOCK_HPP_

#include "stm32h7xx_hal.h"

/**
 * Interrupt lock object
 *
 * Interrupts are disabled in the scope where the object exists
 *
 */
class IrqLock {
public:
    __attribute__((used)) IrqLock();
    __attribute__((used)) ~IrqLock();
private:
    bool initialIrqState;
};

#endif /* INC_IRQLOCK_HPP_ */
