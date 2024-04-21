/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_WORKER_HPP
#define LIBSMART_STM32GCODERUNNER_WORKER_HPP

#include "tx_api.h"
#include "Stm32ItmLogger.h"

extern Debugger *DBG;

namespace Stm32GcodeRunner {

    class CommandContext;
    class AbstractCommand;

    class Worker {
    public:
        enum class enqueueCommandReturn { OK_SYNC, OK_ASYNC, ERROR };
        enqueueCommandReturn enqueueCommandContext(CommandContext *cmdCtx);
        CommandContext *getNextCommandContext(CommandContext *prevCmdCtx);

        bool createCommandContext(CommandContext *&cmdCtx);
        bool createCommandContext(CommandContext *&cmdCtx, AbstractCommand *cmd);
        void deleteCommandContext(CommandContext *&cmdCtx);

    protected:
        [[noreturn]] VOID workerThread();


    private:
//        TX_SEMAPHORE cmdReady{};
        CommandContext *cmdCtxStorage[10]{};
        TX_QUEUE cmdCtxQueue{};
        CommandContext *cmdCtxQueueBuffer[10]{};
    };
}

#endif //LIBSMART_STM32GCODERUNNER_WORKER_HPP
