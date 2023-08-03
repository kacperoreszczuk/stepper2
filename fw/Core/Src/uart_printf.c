#include "main.h"
#include "handles.hpp"
#include "uart_printf.h"

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(huart_usb, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

