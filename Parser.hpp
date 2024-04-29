/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_PARSER_HPP
#define LIBSMART_STM32GCODERUNNER_PARSER_HPP

#include <cstdint>
#include <array>
#include <memory>
#include "tx_api.h"
#include "CommandContext.hpp"
//#include "AbstractCommand.hpp"


namespace Stm32GcodeRunner {

    class AbstractCommand;

    template<typename ConcreteCommand>
    std::shared_ptr<AbstractCommand> dynamic_pointer_cast_dynamic(std::shared_ptr<AbstractCommand> ptr) {
        if (dynamic_cast<ConcreteCommand*>(ptr.get())) {
            return std::dynamic_pointer_cast<ConcreteCommand>(ptr);
        }
        return nullptr;
    }

    class Parser {
    public:
        Parser() {};

        enum class registerCommandReturn {
            SUCCESS, CMD_ALREADY_REGISTERED, CMD_REGISTRY_FULL
        };

        static registerCommandReturn registerCommand(AbstractCommand *cmd);

        static AbstractCommand *findCommand(const char * cmdName);

        /**
         * NOT IMPLEMENTED.
         */
        virtual VOID parserThread() {}


        enum class parserReturn {
            OK, WHITESPACE_STRING, GARBAGE_STRING, UNKNOWN_COMMAND
        };

        virtual parserReturn parseString(AbstractCommand *&cmd, const char *inputString, uint32_t strlen);

    private:
        static std::array<AbstractCommand *, 10> cmdRegistry;
    };

}

#endif //LIBSMART_STM32GCODERUNNER_PARSER_HPP
