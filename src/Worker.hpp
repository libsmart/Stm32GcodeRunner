/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_WORKER_HPP
#define LIBSMART_STM32GCODERUNNER_WORKER_HPP

#include "tx_api.h"
#include "Stm32ItmLogger.h"

#define COMMAND_CONTEXT_POOL_SIZE 4
#define COMMAND_CONTEXT_QUEUE_SIZE 4

//extern Debugger *DBG;

namespace Stm32GcodeRunner {
    class CommandContext;
    class AbstractCommand;

    class Worker {
    public:
        struct mem_t;

        // cmdCtxQueue
        enum class enqueueCommandReturn { OK_SYNC, OK_ASYNC, ERROR };

        enqueueCommandReturn enqueueCommandContext(CommandContext *cmdCtx);

        CommandContext *getNextCommandContext(CommandContext *prevCmdCtx);

        void clearCommandContextQueue();

        // Thread
        bool hasRunningCommandContext();

        CommandContext *getRunningCommandContext();

        // cmdCtxStorage
        bool createCommandContext(CommandContext *&cmdCtx);

        bool createCommandContext(CommandContext *&cmdCtx, AbstractCommand *cmd);

        void deleteCommandContext(CommandContext *cmdCtx);

        static void terminateCurrent();

        static void terminateAll();

        /**
         * @brief Creates a pool of CommandContext objects.
         *
         * This method is used to create a pool of CommandContext objects
         * which can be used by the worker thread for processing commands.
         *
         * @param pMem Pointer to the memory structure used for storing the CommandContext objects
         * @deprecated Use createCommandContextPool(mem_t *pMem, size_t size) instead to also check the size.
         */
        void createCommandContextPool(mem_t *pMem);

        /**
         * @brief Creates a pool of CommandContext objects.
         *
         * This method is used to create a pool of CommandContext objects
         * which can be used by the worker thread for processing commands.
         *
         * @param pMem Pointer to the memory structure used for storing the CommandContext objects
         * @param size The size of the memory structure. If size is not provided, it will be determined based on the size of mem_t.
         *
         * @deprecated Use createCommandContextPool(mem_t *pMem, size_t size) instead to also check the size.
         */
        void createCommandContextPool(mem_t *pMem, size_t size);

        static size_t getCommandContextPoolSizeRequirement();

    protected:
        [[noreturn]] VOID workerThread();

    private:
        CommandContext *currentCmdCtx{};
        //        TX_SEMAPHORE cmdReady{};
        TX_QUEUE cmdCtxQueue{};
        CommandContext *cmdCtxQueueBuffer[COMMAND_CONTEXT_QUEUE_SIZE]{};

        mem_t *mem = {};
    };
}

#endif //LIBSMART_STM32GCODERUNNER_WORKER_HPP
