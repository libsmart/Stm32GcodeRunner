/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "CommandContext.hpp"
#include "Helper.hpp"
#include "AbstractCommand.hpp"

using namespace Stm32GcodeRunner;
AbstractCommand::preFlightCheckReturn preFlightCheckResult = AbstractCommand::preFlightCheckReturn::UNDEF;
AbstractCommand::initReturn initResult = AbstractCommand::initReturn::UNDEF;
AbstractCommand::runReturn runResult = AbstractCommand::runReturn::UNDEF;
AbstractCommand::cleanupReturn cleanupResult = AbstractCommand::cleanupReturn::UNDEF;


bool CommandContext::hasError() {
    switch (cmdState) {
        case cmdStates::UNDEF:
        case cmdStates::PREFLIGHTCHECK:
        case cmdStates::PREFLIGHTCHECK_DONE:
        case cmdStates::INIT:
        case cmdStates::INIT_DONE:
        case cmdStates::RUN:
        case cmdStates::RUN_DONE:
            return false;

        case cmdStates::PREFLIGHTCHECK_ERROR:
        case cmdStates::INIT_ERROR:
        case cmdStates::RUN_TIMEOUT:
        case cmdStates::RUN_ERROR:
        case cmdStates::TERMINATED:
            void(0);
            break;
    }
    return true;
}

void CommandContext::do_preFlightCheck() {
    if (hasError() || mustRecycle) return;
    if (cmdState != cmdStates::UNDEF) return;
    cmdState = cmdStates::PREFLIGHTCHECK;
    preFlightCheckResult = cmd->preFlightCheck();

    if (preFlightCheckResult == AbstractCommand::preFlightCheckReturn::READY) {
        cmdState = cmdStates::PREFLIGHTCHECK_DONE;
    } else {
        cmdState = cmdStates::PREFLIGHTCHECK_ERROR;
        cmdOutputBuffer.println("ERROR: preFlightCheck failed");
        mustRecycle = true;
    }
}

void CommandContext::do_init() {
    if (hasError() || mustRecycle) return;
    if (cmdState != cmdStates::PREFLIGHTCHECK_DONE) return;
    cmdState = cmdStates::INIT;
    initResult = cmd->init();
    cmdState = initResult == AbstractCommand::initReturn::READY
                   ? cmdStates::INIT_DONE
                   : cmdStates::INIT_ERROR;
    if (hasError()) {
        cmdOutputBuffer.println("ERROR: init failed");
        mustRecycle = true;
    }
}

void CommandContext::do_run() {
    if (hasError() || mustRecycle) return;
    if (cmdState != cmdStates::INIT_DONE && cmdState != cmdStates::RUN) return;
    if (cmdState != cmdStates::RUN) {
        cmd->firstRunMillis = millis();
    }
    cmdState = cmdStates::RUN;

    //    cmd->runDuration += millis() - cmd->lastRunMillis;
    cmd->lastRunMillis = millis();

    if ((cmd->runTimeout > 0) && (cmd->getRunDuration() > cmd->runTimeout)) {
        cmdState = cmdStates::RUN_TIMEOUT;
        runResult = AbstractCommand::runReturn::TIMEOUT;
        this->onRunTimeout();
    }

    if (cmdState == cmdStates::RUN) {
        runResult = cmd->run();
    }

    switch (runResult) {
        case AbstractCommand::runReturn::FINISHED:
            cmdState = cmdStates::RUN_DONE;
            break;

        case AbstractCommand::runReturn::RUNNING:
            cmdState = cmdStates::RUN;
            break;

        case AbstractCommand::runReturn::UNDEF:
        case AbstractCommand::runReturn::TIMEOUT:
        case AbstractCommand::runReturn::ERROR:
            cmdState = cmdStates::RUN_ERROR;
            break;
    }
    cmd->runDuration = cmd->getRunDuration();
    if (hasError()) this->onRunError();
    if (cmdState != cmdStates::RUN) this->onRunFinished();
    if (hasError()) cmdOutputBuffer.println("ERROR: run failed");
}

void CommandContext::do_cleanup() {
    if (mustRecycle) return this->onCmdEnd();
    if (cmdState != cmdStates::RUN_DONE &&
        cmdState != cmdStates::RUN_TIMEOUT &&
        cmdState != cmdStates::RUN_ERROR)
        return this->onCmdEnd();
    cleanupResult = cmd->cleanup();
    // cmdOutputBuffer.write("ERROR: cleanup failed\r\n");
    if (!hasError()) cmdOutputBuffer.println("OK");
    mustRecycle = true;
    this->onCleanupFinished();
    this->onCmdEnd();
}

void CommandContext::do_terminate() {
    cmd->terminate();
    mustRecycle = true;
    cmdState = cmdStates::TERMINATED;
    cmdOutputBuffer.print("NOTICE: command `");
    cmdOutputBuffer.print(getCommandLine());
    cmdOutputBuffer.println("` terminated");
    cmdOutputBuffer.println("OK");
}

void CommandContext::onRunTimeout() {
    cmd->onRunTimeout();
}

void CommandContext::onRunError() {
    cmd->onRunError();
}

void CommandContext::onRunFinished() {
    cmd->onRunFinished();
    onRunFinishedFn();
}

void CommandContext::onCleanupFinished() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32GcodeRunner::CommandContext::onCleanupFinished()");
    cmd->onCleanupFinished();
    onCleanupFinishedFn();
}

void CommandContext::onCmdEnd() {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32GcodeRunner::CommandContext::onCmdEnd()");
    if (cmdOutputBuffer.getLength() > 0) onWriteFn();
    cmd->onCmdEnd();
    onCmdEndFn();
}

bool CommandContext::isCmdSync() {
    return cmd->isSync;
}

void CommandContext::recycle() {
    cmd->recycle();
    cmd = nullptr;
    cmdState = cmdStates::UNDEF;

    preFlightCheckResult = AbstractCommand::preFlightCheckReturn::UNDEF;
    initResult = AbstractCommand::initReturn::UNDEF;
    runResult = AbstractCommand::runReturn::UNDEF;
    cleanupResult = AbstractCommand::cleanupReturn::UNDEF;
    cmdOutputBuffer.clear();

    mustRecycle = false;
}

bool CommandContext::isFinished() const {
    return mustRecycle;
}

const char *CommandContext::getName() {
    return cmd == nullptr ? nullptr : cmd->getName();
}

const char *CommandContext::getCommandLine() {
    return cmd == nullptr ? nullptr : cmd->getCommandLine();
}


bool CommandContext::setCommand(AbstractCommand *command) {
    if (cmd == nullptr) {
        cmd = command;
        cmd->setContext(this);
        return true;
    }
    return false;
}
