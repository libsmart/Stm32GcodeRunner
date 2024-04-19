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
#include "AbstractCommand.hpp"


namespace Stm32GcodeRunner {

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

        registerCommandReturn registerCommand(AbstractCommand *cmd);

        AbstractCommand *findCommand(const char * cmdName);

        /**
         * NOT IMPLEMENTED.
         */
        virtual VOID parserThread() {}


        enum class parserReturn {
            OK_SYNC, OK_ASYNC, UNKNOWN_COMMAND, CONTEXT_NOT_READY
        };

        virtual parserReturn parseString(const char *inputString);

        virtual parserReturn parseString(const char *inputString, uint32_t strlen);

        virtual parserReturn parseString(CommandContext *cmdCtx, const char *inputString, uint32_t strlen);

    private:
        std::array<AbstractCommand *, 10> cmdRegistry{};
    };

}

#endif //LIBSMART_STM32GCODERUNNER_PARSER_HPP
