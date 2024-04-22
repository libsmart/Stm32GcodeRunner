/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Worker.hpp"
#include "Helper.hpp"
#include "CommandContext.hpp"
#include "Stm32GcodeRunner.hpp"

VOID Stm32GcodeRunner::Worker::workerThread() {
    Debugger_log(DBG, "Worker::workerThread() starting");

    UINT ret{};
    CommandContext *cmdCtx{};
//    UINT state{};
//    TX_THREAD *thread = tx_thread_identify();


    // Create queue
    tx_queue_create(&cmdCtxQueue, const_cast<CHAR *>("Command queue"), sizeof cmdCtxQueueBuffer[0] / 4,
                    &cmdCtxQueueBuffer, sizeof cmdCtxQueueBuffer);

    for (;;) {
        currentCmdCtx = nullptr;

        // Wait for command
        ret = tx_queue_receive(&cmdCtxQueue, &cmdCtx, TX_WAIT_FOREVER);
        if (ret != TX_SUCCESS || cmdCtx == nullptr) {
            Debugger_log(DBG, "%lu: tx_queue_receive() = 0x%02x", millis(), ret);
            continue;
        }

        Debugger_log(DBG, "Worker::workerThread() received command %s", cmdCtx->getName());

        currentCmdCtx = cmdCtx;
        cmdCtx->do_run();
        cmdCtx->do_cleanup();


        /*
        ret = tx_thread_info_get(thread,
                                 reinterpret_cast<CHAR **>(TX_NULL),
                                 &state,
                                 (ULONG *) TX_NULL,
                                 (UINT *) TX_NULL,
                                 (UINT *) TX_NULL,
                                 (ULONG *) TX_NULL,
                                 reinterpret_cast<TX_THREAD **>(TX_NULL),
                                 reinterpret_cast<TX_THREAD **>(TX_NULL)
        );
        if (ret != TX_SUCCESS || state == TX_TERMINATED) {
            Debugger_log(DBG, "Worker::workerThread() TERMINATED");
            break;
        }
*/
        tx_thread_sleep(1);
    };
//    for (;;) { tx_thread_sleep(1); }
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

void Stm32GcodeRunner::Worker::clearCommandContextQueue() {
    Debugger_log(DBG, "Worker::clearCommandContextQueue()");

    UINT ret{};
    CommandContext *cmdCtx{};
    ULONG enqueued{}, available_storage{}, suspended_count{};

    do {
        tx_queue_info_get(&cmdCtxQueue,
                          reinterpret_cast<CHAR **>(TX_NULL),
                          &enqueued,
                          &available_storage,
                          reinterpret_cast<TX_THREAD **>(TX_NULL),
                          &suspended_count,
                          reinterpret_cast<TX_QUEUE **>(TX_NULL)
        );

        Debugger_log(DBG, "enqueued=%d  |  available_storage=%d  |  suspended_count=%d", enqueued, available_storage,
                     suspended_count);

        if (enqueued > 0) {
            if (suspended_count != 0) {
                Debugger_log(DBG, "ERROR: Worker thread is still waiting for command.");
            }
            ret = tx_queue_receive(&cmdCtxQueue, &cmdCtx, TX_NO_WAIT);
            if (ret != TX_SUCCESS || cmdCtx == nullptr) {
                Debugger_log(DBG, "%lu: tx_queue_receive() = 0x%02x", millis(), ret);
                continue;
            }

            cmdCtx->do_terminate();
        }
    } while (enqueued > 0);

//    tx_queue_flush(&cmdCtxQueue);
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

bool Stm32GcodeRunner::Worker::hasRunningCommandContext() {
    return currentCmdCtx != nullptr;
}

Stm32GcodeRunner::CommandContext *Stm32GcodeRunner::Worker::getRunningCommandContext() {
    return currentCmdCtx;
}

void Stm32GcodeRunner::Worker::terminateCurrent() {
    // Terminate running command
    Stm32GcodeRunner::worker->terminate();
    if (Stm32GcodeRunner::worker->hasRunningCommandContext()) {
        auto cmdCtx = Stm32GcodeRunner::worker->getRunningCommandContext();
        cmdCtx->do_terminate();
    }
    Stm32GcodeRunner::worker->reset();

    // Restart worker thread
    Stm32GcodeRunner::worker->resume();
}

void Stm32GcodeRunner::Worker::terminateAll() {
    // Terminate running command
    Stm32GcodeRunner::worker->terminate();
    if (Stm32GcodeRunner::worker->hasRunningCommandContext()) {
        auto cmdCtx = Stm32GcodeRunner::worker->getRunningCommandContext();
        cmdCtx->do_terminate();
    }
    Stm32GcodeRunner::worker->reset();

    // Clear command queue
    Stm32GcodeRunner::worker->clearCommandContextQueue();

    // Restart worker thread
    Stm32GcodeRunner::worker->resume();
}

