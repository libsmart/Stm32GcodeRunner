/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32GcodeRunner.hpp"
#include "Stm32ItmLogger.h"

extern Debugger *DBG;

Stm32GcodeRunner::Parser Stm32GcodeRunner::parser;

UINT Stm32GcodeRunner::setupThread(TX_BYTE_POOL *byte_pool) {
    Debugger_log(DBG, "Stm32GcodeRunner::setupThread()");
    return TX_SUCCESS;
}



