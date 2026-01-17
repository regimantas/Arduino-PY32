#include "Comparator.h"

#if defined(HAL_COMP_MODULE_ENABLED)

// PY32F0xx startup varies:
// - Some parts use ADC_COMP_IRQHandler
// - PY32F002A startup uses ADC_IRQHandler for the shared ADC&COMP vector
// Provide both and forward to Comparator dispatcher.

extern "C" void ADC_IRQHandler(void)
{
  Comparator::_handleIRQ();
}

extern "C" void ADC_COMP_IRQHandler(void)
{
  Comparator::_handleIRQ();
}

#endif // HAL_COMP_MODULE_ENABLED
