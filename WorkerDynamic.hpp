/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP
#define LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP

#include "Worker.hpp"
#include "Stm32ThreadxThread.hpp"

namespace Stm32GcodeRunner {
    class WorkerDynamic : public Worker, public Stm32ThreadxThread::thread {
    public:
        WorkerDynamic(void *pstack,
                      uint32_t stackSize,
                      const Stm32ThreadxThread::thread::priority &prio,
                      const char *name) : thread(pstack,
                                                 stackSize,
                                                 &Stm32ThreadxThread::BOUNCE(WorkerDynamic, workerThread),
                                                 reinterpret_cast<ULONG>(this),
                                                 prio,
                                                 name) {
        }

        WorkerDynamic(threadEntry func, Stm32ThreadxThread::native::ULONG param, const priority &prio, const char *name)
            : thread(func, param, prio, name) {
        }

        explicit WorkerDynamic(const char *name) : WorkerDynamic(&Stm32ThreadxThread::BOUNCE(WorkerDynamic, workerThread),
                                                        reinterpret_cast<ULONG>(this),
                                                        Stm32ThreadxThread::thread::priority(),
                                                        name) {
        }
    };
}

#endif //LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP
