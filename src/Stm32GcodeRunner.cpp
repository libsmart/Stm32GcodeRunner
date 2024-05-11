/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32GcodeRunner.hpp"
#include "Helper.hpp"
#include "Stm32ItmLogger.h"
#include "WorkerDynamic.hpp"
#include "main.hpp"

Stm32GcodeRunner::Parser *Stm32GcodeRunner::parser = {};
Stm32GcodeRunner::WorkerDynamic *Stm32GcodeRunner::worker = {};

UINT Stm32GcodeRunner::setupThread(TX_BYTE_POOL *byte_pool) {
    Debugger_log(DBG, "Stm32GcodeRunner::setupThread()");

    UINT ret = TX_SUCCESS;
    UCHAR *memPtr = nullptr;


    // Allocate memory for the parser object
    ret = tx_byte_allocate(byte_pool, reinterpret_cast<void **>(&memPtr),
                           sizeof(Parser),
                           TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        Debugger_log(DBG, "%lu: tx_byte_allocate() = 0x%02x", millis(), ret);
        assert_param(ret != TX_SUCCESS);
    }
    // Create parser obejct
    parser = new(memPtr) Parser();


    // Allocate memory for the worker object
    ret = tx_byte_allocate(byte_pool, reinterpret_cast<void **>(&memPtr),
                           sizeof(WorkerDynamic),
                           TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        Debugger_log(DBG, "%lu: tx_byte_allocate() = 0x%02x", millis(), ret);
        assert_param(ret != TX_SUCCESS);
    }
    // Create worker obejct
    worker = new(memPtr) WorkerDynamic("GCODE worker");


    // Allocate stack for the worker thread
    ret = tx_byte_allocate(byte_pool, reinterpret_cast<void **>(&memPtr),
                           WORKER_THREAD_STACK_SIZE,
                           TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        Debugger_log(DBG, "%lu: tx_byte_allocate() = 0x%02x", millis(), ret);
        assert_param(ret != TX_SUCCESS);
    }
    worker->setStack(memPtr, WORKER_THREAD_STACK_SIZE);


    // Allocate memory for the context pool
    ret = tx_byte_allocate(byte_pool, reinterpret_cast<void **>(&memPtr),
                           Worker::getCommandContextPoolSizeRequirement(), TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        Debugger_log(DBG, "%lu: tx_byte_allocate() = 0x%02x", millis(), ret);
        assert_param(ret != TX_SUCCESS);
    }
    worker->createCommandContextPool(reinterpret_cast<Worker::mem_t *>(memPtr));


    // Start thread
    worker->createThread();
    worker->resume();

    return TX_SUCCESS;
}



