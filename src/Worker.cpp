/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Worker.hpp"
#include <main.h>
#include "Helper.hpp"
#include "CommandContext.hpp"
#include "Stm32GcodeRunner.hpp"

struct Stm32GcodeRunner::Worker::mem_t {
    CommandContext *cmdCtxPtr[COMMAND_CONTEXT_POOL_SIZE]{};
    CommandContext cmdCtxMem[COMMAND_CONTEXT_POOL_SIZE];
};

VOID Stm32GcodeRunner::Worker::workerThread() {
    log()
            ->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32GcodeRunner::Worker::workerThread() starting");

    UINT ret{};
    CommandContext *cmdCtx{};

    // Create queue
    tx_queue_create(&cmdCtxQueue, const_cast<CHAR *>("Command queue"), sizeof cmdCtxQueueBuffer[0] / 4,
                    &cmdCtxQueueBuffer, sizeof cmdCtxQueueBuffer);

    for (;;) {
        currentCmdCtx = nullptr;

        // Wait for command
        ret = tx_queue_receive(&cmdCtxQueue, &cmdCtx, TX_WAIT_FOREVER);
        if (ret != TX_SUCCESS || cmdCtx == nullptr) {
            log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                    ->printf("%lu: tx_queue_receive() = 0x%02x\r\n", millis(), ret);
            continue;
        }

        log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)->printf(
            "Stm32GcodeRunner::Worker::workerThread() received command %s\r\n", cmdCtx->getName());

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
}

Stm32GcodeRunner::Worker::enqueueCommandReturn Stm32GcodeRunner::Worker::enqueueCommandContext(CommandContext *cmdCtx) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32GcodeRunner::Worker::enqueueCommandContext(%s)\r\n", cmdCtx->getName());

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
        log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("%lu: tx_queue_send() = 0x%02x\r\n", millis(), ret);
        return enqueueCommandReturn::ERROR;
    }
    return enqueueCommandReturn::OK_ASYNC;
}

void Stm32GcodeRunner::Worker::clearCommandContextQueue() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32GcodeRunner::Worker::clearCommandContextQueue()\r\n");

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

        log(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
                ->printf("enqueued=%d  |  available_storage=%d  |  suspended_count=%d\r\n", enqueued, available_storage,
                         suspended_count);

        if (enqueued > 0) {
            if (suspended_count != 0) {
                log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                        ->printf("ERROR: Worker thread is still waiting for command.\r\n");
            }
            ret = tx_queue_receive(&cmdCtxQueue, &cmdCtx, TX_NO_WAIT);
            if (ret != TX_SUCCESS || cmdCtx == nullptr) {
                log(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                        ->printf("%lu: tx_queue_receive() = 0x%02x\r\n", millis(), ret);
                continue;
            }

            cmdCtx->do_terminate();
        }
    } while (enqueued > 0);

    //    tx_queue_flush(&cmdCtxQueue);
}

bool Stm32GcodeRunner::Worker::createCommandContext(CommandContext *&cmdCtx) {
    for (std::size_t index = 0; index < COMMAND_CONTEXT_POOL_SIZE; ++index) {
        if (mem->cmdCtxPtr[index] == nullptr) {
            cmdCtx = mem->cmdCtxPtr[index] = new(&mem->cmdCtxMem[index]) CommandContext();
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
    bool found = (prevCmdCtx == nullptr);
    for (auto &i: mem->cmdCtxPtr) {
        if (found && i != nullptr) {
            return i;
        }
        if (i == prevCmdCtx) {
            found = true;
        }
    }
    return nullptr;
}

void Stm32GcodeRunner::Worker::deleteCommandContext(Stm32GcodeRunner::CommandContext *cmdCtx) {
    for (std::size_t index = 0; index < COMMAND_CONTEXT_POOL_SIZE; ++index) {
        if (mem->cmdCtxPtr[index] == cmdCtx) {
            cmdCtx->recycle();
            // Call destructor manually, because delete is not used if instance was created
            // by a placement new
            cmdCtx->~CommandContext();
            // Clear pointer and memory
            mem->cmdCtxPtr[index] = nullptr;
            memset(&mem->cmdCtxMem[index], 0, sizeof(mem->cmdCtxMem[index]));
            return;
        }
    }
}

void Stm32GcodeRunner::Worker::terminateCommandContext(CommandContext *cmdCtx) {
    if(cmdCtx == nullptr) return;

    if(worker->getRunningCommandContext() == cmdCtx) {
        worker->terminate();
    }

    cmdCtx->do_terminate();
    worker->reset();
    worker->deleteCommandContext(cmdCtx);
    worker->resume();
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

void Stm32GcodeRunner::Worker::createCommandContextPool(mem_t *pMem) {
    createCommandContextPool(pMem, 0);
}

void Stm32GcodeRunner::Worker::createCommandContextPool(mem_t *pMem, size_t size) {
    assert_param(pMem != nullptr);
    // size = size == 0 ? getCommandContextPoolSizeRequirement() : size;
    assert_param(size == 0 || size >= sizeof(mem_t));
    memset(pMem, 0, sizeof(mem_t));
    mem = new(pMem) mem_t;
}

size_t Stm32GcodeRunner::Worker::getCommandContextPoolSizeRequirement() {
    return sizeof(mem_t);
}

