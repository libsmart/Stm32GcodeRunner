/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP
#define LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP

#include "Stm32ThreadX.hpp"
#include "Thread.hpp"
#include "Worker.hpp"

namespace Stm32GcodeRunner {
    class WorkerDynamic : public Worker, public Stm32ThreadX::Thread {
    public:
        WorkerDynamic(void *pstack,
                      uint32_t stackSize,
                      const Stm32ThreadX::Thread::priority &prio,
                      const char *name) : Thread(pstack,
                                                 stackSize,
                                                 &Stm32ThreadX::bounce<WorkerDynamic, decltype(&WorkerDynamic::workerThread), &WorkerDynamic::workerThread>,
                                                 reinterpret_cast<ULONG>(this),
                                                 prio,
                                                 name) {
        }

        WorkerDynamic(threadEntry func, Stm32ThreadX::native::ULONG param, const priority &prio, const char *name)
            : Thread(func, param, prio, name) {
        }

        explicit WorkerDynamic(const char *name) : WorkerDynamic(&Stm32ThreadX::BOUNCE(WorkerDynamic, workerThread),
                                                        reinterpret_cast<ULONG>(this),
                                                        Stm32ThreadX::Thread::priority(),
                                                        name) {
        }
    };
}

#endif //LIBSMART_STM32GCODERUNNER_WORKERDYNAMIC_HPP
