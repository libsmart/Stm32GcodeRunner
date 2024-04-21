/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_STM32GCODERUNNER_HPP
#define LIBSMART_STM32GCODERUNNER_STM32GCODERUNNER_HPP

#include "tx_api.h"
#include "Parser.hpp"
#include "WorkerDynamic.hpp"

namespace Stm32GcodeRunner {

    UINT setupThread(TX_BYTE_POOL *byte_pool);
    extern WorkerDynamic *worker;
    extern Parser parser;


}

#endif //LIBSMART_STM32GCODERUNNER_STM32GCODERUNNER_HPP
