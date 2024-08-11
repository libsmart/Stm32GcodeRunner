/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_PARSER_HPP
#define LIBSMART_STM32GCODERUNNER_PARSER_HPP

#include <libsmart_config.hpp>
#include <cstdint>
#include <array>
#include <memory>
#include "tx_api.h"
#include "CommandContext.hpp"
#include "Singleton.hpp"
#include "StaticLoggable.hpp"

namespace Stm32GcodeRunner {
    class AbstractCommand;

    template<typename ConcreteCommand>
    std::shared_ptr<AbstractCommand> dynamic_pointer_cast_dynamic(std::shared_ptr<AbstractCommand> ptr) {
        if (dynamic_cast<ConcreteCommand *>(ptr.get())) {
            return std::dynamic_pointer_cast<ConcreteCommand>(ptr);
        }
        return nullptr;
    }

    class Parser : public Stm32Common::Singleton<Parser>, public Stm32ItmLogger::StaticLoggable {
    public:
        enum class registerCommandReturn {
            SUCCESS, CMD_ALREADY_REGISTERED, CMD_REGISTRY_FULL
        };

        static registerCommandReturn registerCommand(AbstractCommand *cmd);

        static AbstractCommand *findCommand(const char *cmdName);

        /**
         * NOT IMPLEMENTED.
         */
        virtual VOID parserThread() {
        }


        enum class parserReturn {
            OK, WHITESPACE_STRING, GARBAGE_STRING, UNKNOWN_COMMAND
        };

        virtual parserReturn parseArgcArgv(AbstractCommand * &cmd, int argc, const char *const *argv);

        virtual parserReturn parseString(AbstractCommand *&cmd, const char *inputString, uint32_t strlen);

    private:
        using cmdRegistryType = std::array<AbstractCommand *, LIBSMART_GCODERUNNER_COMMAND_REGISTRY_SIZE>;
        static cmdRegistryType cmdRegistry;
    };
}

#endif //LIBSMART_STM32GCODERUNNER_PARSER_HPP
