/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds global variables, used exclusively in C++ code.
 * @see globals.hpp
 */

#include "globals.hpp"
#include "main.h"

//uint32_t micros() {
//    uint32_t micros = HAL_GetTick() * 1000;  // Millisekunden in Mikrosekunden umrechnen
//    micros += (SysTick->LOAD - SysTick->VAL) /
//              (SystemCoreClock / 1000000); // Zeit seit dem letzten Millisekunden-Überlauf hinzufügen
//    return micros;
//}

uint32_t dummyCpp;

