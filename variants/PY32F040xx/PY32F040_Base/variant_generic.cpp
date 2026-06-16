/*
 *******************************************************************************
 * Copyright (c) 2023, AirM2M
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#if defined(PY32F040x6) || defined(PY32F040x8) || defined(PY32F040x9) || defined(PY32F040xB) || \
    defined(PY32F040Cx6) || defined(PY32F040Cx8) || defined(PY32F040Cx9) || defined(PY32F040CxB)
#include "pins_arduino.h"

const PinName digitalPin[] = {
    PA_0,  PA_1,  PA_2,  PA_3,  PA_4,  PA_5,  PA_6,  PA_7,
    PA_8,  PA_9,  PA_10, PA_11, PA_12, PA_13, PA_14, PA_15,
    PB_0,  PB_1,  PB_2,  PB_3,  PB_4,  PB_5,  PB_6,  PB_7,
    PB_8,  PB_9,  PB_10, PB_11, PB_12, PB_13, PB_14, PB_15,
    PF_0,  PF_1,  PF_2,  PF_5,  PF_6,  PF_8,  PF_9,
    PC_13, PC_14, PC_15,
};

const uint32_t analogInputPin[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 16, 17
};

#endif
