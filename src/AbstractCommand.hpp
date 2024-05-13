/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_ABSTRACTCOMMAND_HPP
#define LIBSMART_STM32GCODERUNNER_ABSTRACTCOMMAND_HPP

#include "CommandContext.hpp"
#include "Worker.hpp"
#include "main.hpp"

namespace Stm32GcodeRunner {

    class AbstractCommand {
        friend Worker;
        friend CommandContext;

    public:
        AbstractCommand();

        using u_preFlightCheckReturn = enum class preFlightCheckReturn {
            UNDEF, ERROR, READY
        };

        using u_initReturn = enum class initReturn {
            UNDEF, ERROR, READY
        };

        using u_runReturn = enum class runReturn {
            UNDEF, ERROR, TIMEOUT, RUNNING, FINISHED
        };

        using u_cleanupReturn = enum class cleanupReturn {
            UNDEF, ERROR, OK
        };

        virtual preFlightCheckReturn preFlightCheck() {
            Debugger_log(DBG, "%s::preFlightCheck()", getName());
            return preFlightCheckReturn::ERROR;
        };

        virtual initReturn init() {
            Debugger_log(DBG, "%s::init()", getName());
            return initReturn::ERROR;
        };

        virtual runReturn run() {
            Debugger_log(DBG, "%s::run()", getName());
            return runReturn::ERROR;
        };

        virtual cleanupReturn cleanup() {
            Debugger_log(DBG, "%s::cleanup()", getName());
            return cleanupReturn::ERROR;
        };

        virtual void terminate() {
            Debugger_log(DBG, "%s::terminate()", getName());
        };

        virtual void recycle() {
            Debugger_log(DBG, "%s::recycle()", getName());
            setCommandLine("", 0);
        };


        virtual void setParam(char paramName, const char *paramString);

        virtual void setParam(char paramName, long paramLong) {
        };

        virtual void setParam(char paramName, double paramDouble) {
        };


        virtual void setCommandLine(const char *cmdLine, size_t size);

        virtual const char *getCommandLine();

        virtual const char *getName() { return "AbstractCommand"; };

        void setRunTimeout(unsigned long timeout);

        [[nodiscard]] unsigned long getRunDuration() const;

        void setQuiet(bool quiet = true);

        bool isQuietRun() const;


        virtual AbstractCommand *factory() { return new AbstractCommand; };

    protected:
        /**
         * @brief This virtual function is called when the run timeout occurs.
         *
         * This function is a placeholder that can be overridden in derived classes to implement
         * specific functionality when the run timeout occurs.
         */
        virtual void onRunTimeout() {
        };

        /**
         * @brief This virtual function is called when an error occurs during the execution of the command.
         */
        virtual void onRunError() {
        };

        /**
         * @brief Virtual function called when the execution of the command is finished.
         *
         * The function is called regardless of the error status.
         */
        virtual void onRunFinished() {
        };

        /**
         * @brief Virtual function called when the cleanup is finished.
         *
         * The function is called regardless of the error status.
         */
        virtual void onCleanupFinished() {
        };

        /**
         * @brief Virtual function called when the command has ended and is ready for deletion.
         *
         * The function is called regardless of the error status.
         */
        virtual void onCmdEnd() {
        };

    protected:
        bool write(const char *str);

        bool write(const void *in, size_t strlen);

        bool printf(const char *format, ...);

        CommandContext::cmdOutputBufferClass *out();

    protected:
        /** true: the command is executed immediately and synchronous. */
        bool isSync = false;

    private:
        CommandContext *ctx{};

        void setContext(CommandContext *ctx);

        /** Total [ms] since the command started. */
        unsigned long runDuration = 0;
        /** The value of millis() when the run was first started. */
        unsigned long firstRunMillis = 0;
        /** The value of millis() when the last run started. */
        unsigned long lastRunMillis = 0;
        /** Timeout [ms] in which the command must be completed. */
        unsigned long runTimeout = 0;
        /** Quiet run: no "ok" after run */
        bool quietRun = false;
        /** Used to delay debug output */
        unsigned long lastRunDurationDebugOutput = 0;
        /** Stores the originial command line entered */
        char commandLine[32] = {};
    };
}

#endif //LIBSMART_STM32GCODERUNNER_ABSTRACTCOMMAND_HPP
