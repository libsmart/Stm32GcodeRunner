/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_COMMANDCONTEXT_HPP
#define LIBSMART_STM32GCODERUNNER_COMMANDCONTEXT_HPP

#include "AbstractCommand.hpp"
#include "StringBuffer.hpp"

namespace Stm32GcodeRunner {

    class Parser;

    class CommandContext {

        friend Parser;
        friend AbstractCommand;

    public:
        size_t outputLength() {
            return cmdOutputBuffer.getLength();
        }

        size_t outputRead(char *out, size_t size) {
            return cmdOutputBuffer.read(out, size);
        }

/*
        bool outputPrintf(const char *format, ...) {
            va_list args;
            va_start(args, format);
            auto ret = cmdOutputBuffer.printf(format, args);
            va_end(args);
            return ret;
        };
*/


        bool hasError();

        bool isFinished() const;

        bool isCmdSync();

        void recycle();

    protected:
        bool setCommand(AbstractCommand *command) {
            if (cmd == nullptr) {
                cmd = command;
                cmd->setContext(this);
                return true;
            }
            return false;
        }

        void do_preFlightCheck();

        void do_init();

        void do_run();

        void do_cleanup();


    private:
        AbstractCommand *cmd{};
        using u_cmdStates = enum class cmdStates {
            UNDEF,
            PREFLIGHTCHECK,
            PREFLIGHTCHECK_DONE,
            PREFLIGHTCHECK_ERROR,
            INIT,
            INIT_DONE,
            INIT_ERROR,
            RUN,
            RUN_DONE,
            RUN_TIMEOUT,
            RUN_ERROR
        };
        cmdStates cmdState = cmdStates::UNDEF;

        bool mustRecycle = false;

        AbstractCommand::preFlightCheckReturn preFlightCheckResult = AbstractCommand::preFlightCheckReturn::UNDEF;
        AbstractCommand::initReturn initResult = AbstractCommand::initReturn::UNDEF;
        AbstractCommand::runReturn runResult = AbstractCommand::runReturn::UNDEF;
        AbstractCommand::cleanupReturn cleanupResult = AbstractCommand::cleanupReturn::UNDEF;

        Stm32Common::StringBuffer<256> cmdOutputBuffer{};

    };
}

#endif //LIBSMART_STM32GCODERUNNER_COMMANDCONTEXT_HPP
