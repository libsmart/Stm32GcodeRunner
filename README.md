# Stm32GcodeRunner



Implements a parser and runner for GCODE like commands for STM32 MCU.



Requires `Stm32ThreadxThread`



## Installation

In `Core/Src/app_threadx.c`:

```c
#include "Stm32GcodeRunnerCPPWrapper.hpp"


/* ... */


ret = Stm32GcodeRunner_setupThread(byte_pool);
assert_param(ret == TX_SUCCESS);
```

