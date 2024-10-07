#include "irq_lock.hpp"

IrqLock::IrqLock() : initialIrqState(__get_PRIMASK() == 0) {
	__disable_irq();  // disable all interrupts
}

IrqLock::~IrqLock() {
	if(initialIrqState) {
		__enable_irq();
	}
}