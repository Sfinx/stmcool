#pragma once

#ifndef USE_RTT
#define SEGGER_RTT_printf(x...)
#else
#include <SEGGER_RTT.h>
#endif

#define debug(x...)	SEGGER_RTT_printf(0, x)
