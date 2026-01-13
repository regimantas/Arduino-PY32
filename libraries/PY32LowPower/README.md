# PY32LowPower
A simple low-power library for the PY32F002A microcontroller. Uses py32duino Arduino core, at: https://github.com/py32duino/Arduino-PY32
Make deepSleep with Arduino posible in 10 cents PY32F002A microcontroller's :)


The power consumption drops to as low as 7µA. However, calling the analogWrite() function before entering deep sleep increases the sleep mode power consumption to 70µA, which is 10 times higher. To reduce power usage, you should call HAL_GPIO_DeInit(GPIOA, GPIO_PIN_All); after analogWrite(), and then reinitialize the pins from scratch, setting them as input, output, etc.
