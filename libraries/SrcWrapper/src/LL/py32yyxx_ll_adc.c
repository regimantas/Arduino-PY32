/* LL raised several warnings, ignore them */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#if !defined(PY32F040x6) && !defined(PY32F040x8) && !defined(PY32F040x9) && !defined(PY32F040xB) && \
    !defined(PY32F040Cx6) && !defined(PY32F040Cx8) && !defined(PY32F040Cx9) && !defined(PY32F040CxB)
#ifdef AIRC0xx
  #include "airc0xx_ll_adc.c"
#elif PY32F0xx
  #include "py32f0xx_ll_adc.c"
#elif AIR32F1xx
  #include "AIR32F1xx_ll_adc.c"
#elif AIRF2xx
  #include "airf2xx_ll_adc.c"
#elif AIRF3xx
  #include "airf3xx_ll_adc.c"
#elif AIRF4xx
  #include "airf4xx_ll_adc.c"
#elif AIRF7xx
  #include "airf7xx_ll_adc.c"
#elif AIRG0xx
  #include "airg0xx_ll_adc.c"
#elif AIRG4xx
  #include "airg4xx_ll_adc.c"
#elif AIRH7xx
  #include "airh7xx_ll_adc.c"
#elif AIRL0xx
  #include "airl0xx_ll_adc.c"
#elif AIRL1xx
  #include "airl1xx_ll_adc.c"
#elif AIRL4xx
  #include "airl4xx_ll_adc.c"
#elif AIRL5xx
  #include "airl5xx_ll_adc.c"
#elif AIRMP1xx
  #include "airmp1xx_ll_adc.c"
#elif AIRU5xx
  #include "airu5xx_ll_adc.c"
#elif AIRWBxx
  #include "airwbxx_ll_adc.c"
#elif AIRWLxx
  #include "airwlxx_ll_adc.c"
#endif
#endif
#pragma GCC diagnostic pop
