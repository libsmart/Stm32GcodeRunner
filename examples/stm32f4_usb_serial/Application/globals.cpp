/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds global variables, used exclusively in C++ code.
 * @see globals.hpp
 */

#include "globals.hpp"
#include "usart.h"
#include "Driver/Stm32EmptyDriver.hpp"
#include "Driver/Stm32HalUartItDriver.hpp"

uint32_t dummyCpp;
Stm32ItmLogger::Stm32ItmLogger logger;
Stm32Serial::Stm32HalUartItDriver uart1Driver(&huart1, "uart1Driver");
//Stm32Serial::Stm32EmptyDriver uart1Driver("uart1Driver");
Stm32Serial::Stm32Serial Serial1(&uart1Driver);
