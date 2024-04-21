/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Worker.hpp"
#include "Helper.hpp"
#include "CommandContext.hpp"

VOID Stm32GcodeRunner::Worker::workerThread() {
    Debugger_log(DBG, "Worker::workerThread()");

    UINT ret{};
    CommandContext *cmdCtx{};

    // Create queue
    tx_queue_create(&cmdCtxQueue, const_cast<CHAR *>("Command queue"), sizeof cmdCtxQueueBuffer[0] / 4,
                    &cmdCtxQueueBuffer, sizeof cmdCtxQueueBuffer);

    // Create semaphore
//    tx_semaphore_create(&cmdReady, const_cast<CHAR *>("Command Ready"), 0);

    for (;;) {
        // Wait for command
//        tx_semaphore_get(&cmdReady, TX_WAIT_FOREVER);

        ret = tx_queue_receive(&cmdCtxQueue, &cmdCtx, TX_WAIT_FOREVER);
        if (ret != TX_SUCCESS || cmdCtx == nullptr) {
            Debugger_log(DBG, "%lu: tx_queue_receive() = 0x%02x", millis(), ret);
            continue;
        }

        Debugger_log(DBG, "Worker::workerThread() received command %s", cmdCtx->getName());

        cmdCtx->do_run();
        cmdCtx->do_cleanup();

        tx_thread_sleep(1);
    };
}

Stm32GcodeRunner::Worker::enqueueCommandReturn Stm32GcodeRunner::Worker::enqueueCommandContext(CommandContext *cmdCtx) {
    Debugger_log(DBG, "Worker::enqueueCommandContext(%s)", cmdCtx->getName());

    cmdCtx->do_preFlightCheck();
    cmdCtx->do_init();

    if (cmdCtx->isCmdSync()) {
        cmdCtx->do_run();
        cmdCtx->do_cleanup();
        return enqueueCommandReturn::OK_SYNC;
    }

    // Add the pointer to the command context to the worker queue
    auto ret = tx_queue_send(&cmdCtxQueue, &cmdCtx, TX_NO_WAIT);
    if (ret != TX_SUCCESS) {
        Debugger_log(DBG, "%lu: tx_queue_send() = 0x%02x", millis(), ret);
        return enqueueCommandReturn::ERROR;
    }
    return enqueueCommandReturn::OK_ASYNC;
}

bool Stm32GcodeRunner::Worker::createCommandContext(CommandContext *&cmdCtx) {
    cmdCtx = new CommandContext();
    for (auto &i: cmdCtxStorage) {
        if (i == nullptr) {
            i = cmdCtx;
            return true;
        }
    }
    return false;
}

bool Stm32GcodeRunner::Worker::createCommandContext(Stm32GcodeRunner::CommandContext *&cmdCtx,
                                                    Stm32GcodeRunner::AbstractCommand *cmd) {

    if (createCommandContext(cmdCtx)) {
        cmdCtx->setCommand(cmd);
        enqueueCommandContext(cmdCtx);
        return true;
    }
    return false;
}

Stm32GcodeRunner::CommandContext *
Stm32GcodeRunner::Worker::getNextCommandContext(Stm32GcodeRunner::CommandContext *prevCmdCtx) {
    bool found = prevCmdCtx == nullptr;
    for (auto &i: cmdCtxStorage) {
        if (found && i != nullptr) {
            return i;
        }
        if (i == prevCmdCtx) {
            found = true;
        }
    }
    return nullptr;
}

void Stm32GcodeRunner::Worker::deleteCommandContext(Stm32GcodeRunner::CommandContext *&cmdCtx) {
    for (auto &i: cmdCtxStorage) {
        if (i == cmdCtx) {
            cmdCtx->recycle();
            delete cmdCtx;
            i = nullptr;
            return;
        }
    }
}

