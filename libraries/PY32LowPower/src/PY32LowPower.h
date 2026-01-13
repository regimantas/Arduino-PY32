#ifndef PY32LOWPOWER_H
#define PY32LOWPOWER_H

#include "Arduino.h"
#include "Arduino.h"
#include "py32f0xx_hal.h"
#include "py32f0xx_hal_lptim.h"
#include "py32f0xx_hal_pwr.h"

class PY32LowPower {
public:
    PY32LowPower();
    void begin();
    void deepSleep(uint32_t ms);
    void exitSleep();

private:
    void internalDeepSleep(uint32_t cycles) __attribute__((used));
};

__weak void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_Init(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Period);


#endif // PY32LOWPOWER_H
