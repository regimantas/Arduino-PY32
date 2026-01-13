#include "PY32LowPower.h"
#include "py32f0xx_hal.h"
#include "py32f0xx_hal_lptim.h"
#include "py32f0xx_hal_pwr.h"

LPTIM_HandleTypeDef hlptim;

PY32LowPower::PY32LowPower() {}

void PY32LowPower::begin() {
    HAL_Init();
    __HAL_RCC_LSI_ENABLE();
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET);
    __HAL_RCC_LPTIM_CONFIG(RCC_LPTIMCLKSOURCE_LSI);
    __HAL_RCC_LPTIM_CLK_ENABLE();

    hlptim.Instance = LPTIM1;
    hlptim.Init.Prescaler = LPTIM_PRESCALER_DIV1;
    hlptim.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    if (HAL_LPTIM_Init(&hlptim) != HAL_OK) {
        while (1); // Error in initialization
    }

    HAL_NVIC_SetPriority(LPTIM1_IRQn, 0, 0);
    NVIC_EnableIRQ(LPTIM1_IRQn);  
}

void PY32LowPower::internalDeepSleep(uint32_t cycles) {
    // Didžiausias galimas prescaler yra 8, taigi ciklus padalinsime atsižvelgdami į tai.
    if (cycles <= 65535) {
        hlptim.Init.Prescaler = LPTIM_PRESCALER_DIV1;
    } else if (cycles <= 65535 * 2) {
        hlptim.Init.Prescaler = LPTIM_PRESCALER_DIV2;
        cycles /= 2;
    } else if (cycles <= 65535 * 4) {
        hlptim.Init.Prescaler = LPTIM_PRESCALER_DIV4;
        cycles /= 4;
    } else {
        hlptim.Init.Prescaler = LPTIM_PRESCALER_DIV8;
        cycles /= 8;
    }

    cycles = constrain(cycles, 1, 65535);

    if (HAL_LPTIM_Init(&hlptim) != HAL_OK) {
            while (1); // Halt execution on error
    }

    HAL_LPTIM_SetOnce_Start_IT(&hlptim, cycles);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

}

void PY32LowPower::deepSleep(uint32_t ms) {
    uint64_t total_cycles = (uint64_t)ms * 32768 / 1000;  // Užtikriname, kad netgi skaičiavimo metu būtų naudojamas 64 bitų kintamasis
    uint64_t max_cycles = 65535 * 8;  // Maksimali vienos pertraukos trukmė
    // while total_cycles > 0 loop with sleep use --max_cycles. and make cycle in limit of max_cycles
    while (total_cycles > 0) {
        delay(1); // Minimum delay for Arduino not to hang
        total_cycles--; // Decrease total_cycles by 1 ms to account for the delay
        uint32_t cycles = (total_cycles > max_cycles) ? max_cycles : total_cycles;
        internalDeepSleep(cycles);
        total_cycles -= cycles;
    }
}


extern "C" void LPTIM1_IRQHandler(void) __attribute__((used));
extern "C" void LPTIM1_IRQHandler(void) {
    if (__HAL_LPTIM_GET_FLAG(&hlptim, LPTIM_FLAG_ARRM) != RESET) {
        __HAL_LPTIM_CLEAR_FLAG(&hlptim, LPTIM_FLAG_ARRM);

        SystemClock_Config();

    }
}

void PY32LowPower::exitSleep(void) {
    SystemClock_Config();
}


HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Period)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PERIOD(Period));

  /* Set the LPTIM state */
  hlptim->State= HAL_LPTIM_STATE_BUSY;

  /* Enable Autoreload match interrupt */
  __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_ARRM);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Load the period value in the autoreload register */
  __HAL_LPTIM_AUTORELOAD_SET(hlptim, Period);

  /* Start timer in single mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

  /* Change the TIM state*/
  hlptim->State= HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

HAL_StatusTypeDef HAL_LPTIM_Init(LPTIM_HandleTypeDef *hlptim)
{
  uint32_t tmpcfgr;

  /* Check the LPTIM handle allocation */
  if(hlptim == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  assert_param(IS_LPTIM_CLOCK_PRESCALER(hlptim->Init.Prescaler));

  if(hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hlptim->Lock = HAL_UNLOCKED;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
    /* Reset interrupt callbacks to legacy weak callbacks */
    LPTIM_ResetCallback(hlptim);

    if(hlptim->MspInitCallback == NULL)
    {
      hlptim->MspInitCallback = HAL_LPTIM_MspInit;
    }

    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    hlptim->MspInitCallback(hlptim);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_LPTIM_MspInit(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
  }

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Get the LPTIMx CFGR value */
  tmpcfgr = hlptim->Instance->CFGR;

  /* Clear PRESC, PRELOAD  */
  tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_PRELOAD | LPTIM_CFGR_PRESC));

  /* Set initialization parameters */
  tmpcfgr |= (hlptim->Init.Prescaler|hlptim->Init.UpdateMode);

  /* Write to LPTIMx CFGR */
  hlptim->Instance->CFGR = tmpcfgr;

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

__weak void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_MspInit could be implemented in the user file
   */
}
