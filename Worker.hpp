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

    class Worker {
    protected:
        [[noreturn]] VOID workerThread();


    private:
        TX_SEMAPHORE cmdReady;
    };
}

#endif //LIBSMART_STM32GCODERUNNER_WORKER_HPP
