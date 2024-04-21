/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32GcodeRunner.hpp"
#include "Stm32ItmLogger.h"
#include "WorkerDynamic.hpp"
#include "Commands/G0.hpp"
#include "Commands/M18.hpp"
#include "Commands/M115.hpp"
#include "Commands/M503.hpp"

extern Debugger *DBG;
G0 G0;
M18 M18;
M115 M115;
M503 M503;
Stm32GcodeRunner::Parser Stm32GcodeRunner::parser;

__attribute__((section(".ccmram")))
CHAR workerStack[1024] = {};
Stm32GcodeRunner::WorkerDynamic *Stm32GcodeRunner::worker{};

UINT Stm32GcodeRunner::setupThread(TX_BYTE_POOL *byte_pool) {
    Debugger_log(DBG, "Stm32GcodeRunner::setupThread()");


    parser.registerCommand(&G0);
    parser.registerCommand(&M18);
    parser.registerCommand(&M115);
    parser.registerCommand(&M503);


    worker = new WorkerDynamic(workerStack, sizeof workerStack, Stm32ThreadxThread::thread::priority(),
                               "GCODE worker");

    worker->createThread();
    worker->resume();

    return TX_SUCCESS;
}



