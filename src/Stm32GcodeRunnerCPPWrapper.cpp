/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32GcodeRunnerCPPWrapper.hpp"
#include "Stm32GcodeRunner.hpp"

UINT Stm32GcodeRunner_setupThread(TX_BYTE_POOL *byte_pool) {
    return Stm32GcodeRunner::setupThread(byte_pool);
}
