#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
#include "stm32f4xx_hal.h"

#define  _KEYPAD_DEBOUNCE_TIME_MS        20


const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{ GPIOA,
  GPIOA,
  GPIOC,
  GPIOC

};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  GPIO_PIN_3,
  GPIO_PIN_1,
  GPIO_PIN_3,
  GPIO_PIN_1
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
	GPIOB,
  GPIOC,
  GPIOA,
  GPIOA
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
	GPIO_PIN_1,
  GPIO_PIN_5,
  GPIO_PIN_7,
  GPIO_PIN_5
};

#endif
