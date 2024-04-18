/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_PARSER_HPP
#define LIBSMART_STM32GCODERUNNER_PARSER_HPP

#include <cstdint>
#include "tx_api.h"

namespace Stm32GcodeRunner {

    class Parser {
    public:
        Parser() {};


        /**
         * NOT IMPLEMENTED.
         */
        virtual VOID parserThread() {}


        virtual void parseString(const char *inputString);
        virtual void parseString(const char *inputString, uint32_t strlen);

    };

}

#endif //LIBSMART_STM32GCODERUNNER_PARSER_HPP
