/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Worker.hpp"

VOID Stm32GcodeRunner::Worker::workerThread() {
    Debugger_log(DBG, "Worker::workerThread()");

    // Create semaphore
    tx_semaphore_create(&cmdReady, const_cast<CHAR *>("Command Ready"), 0);

    for (;;) {
        // Wait for command
        tx_semaphore_get(&cmdReady, TX_WAIT_FOREVER);


        tx_thread_sleep(1);
    };
}
