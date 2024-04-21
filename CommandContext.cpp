/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "CommandContext.hpp"
#include "Helper.hpp"

bool Stm32GcodeRunner::CommandContext::hasError() {
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
            void(0);

    }
    return true;
}

void Stm32GcodeRunner::CommandContext::do_preFlightCheck() {
    if (hasError() || mustRecycle) return;
    if (cmdState != cmdStates::UNDEF) return;
    cmdState = cmdStates::PREFLIGHTCHECK;
    preFlightCheckResult = cmd->preFlightCheck();

    if (preFlightCheckResult == AbstractCommand::preFlightCheckReturn::READY) {
        cmdState = cmdStates::PREFLIGHTCHECK_DONE;
    } else {
        cmdState = cmdStates::PREFLIGHTCHECK_ERROR;
        cmdOutputBuffer.write("ERROR: preFlightCheck failed\r\n");
        mustRecycle = true;
    }
}

void Stm32GcodeRunner::CommandContext::do_init() {
    if (hasError() || mustRecycle) return;
    if (cmdState != cmdStates::PREFLIGHTCHECK_DONE) return;
    cmdState = cmdStates::INIT;
    initResult = cmd->init();
    cmdState = initResult == AbstractCommand::initReturn::READY
               ? cmdStates::INIT_DONE : cmdStates::INIT_ERROR;
    if (hasError()) {
        cmdOutputBuffer.write("ERROR: init failed\r\n");
        mustRecycle = true;
    }
}

void Stm32GcodeRunner::CommandContext::do_run() {
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
        cmd->onRunTimeout();
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
    if (hasError()) cmd->onRunError();
    if (cmdState != cmdStates::RUN) cmd->onRunFinished();
    if (hasError()) cmdOutputBuffer.write("ERROR: run failed\r\n");
}

void Stm32GcodeRunner::CommandContext::do_cleanup() {
    if (mustRecycle) return;
    if (cmdState != cmdStates::RUN_DONE &&
        cmdState != cmdStates::RUN_TIMEOUT &&
        cmdState != cmdStates::RUN_ERROR)
        return;
    cleanupResult = cmd->cleanup();
    mustRecycle = true;
    // cmdOutputBuffer.write("ERROR: cleanup failed\r\n");
    if (!hasError()) cmdOutputBuffer.write("OK\r\n");
}

bool Stm32GcodeRunner::CommandContext::isCmdSync() {
    return cmd->isSync;
}

void Stm32GcodeRunner::CommandContext::recycle() {
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

bool Stm32GcodeRunner::CommandContext::isFinished() const {
    return mustRecycle;
}

const char *Stm32GcodeRunner::CommandContext::getName() {
    return cmd == nullptr ? nullptr : cmd->getName();
}

bool Stm32GcodeRunner::CommandContext::setCommand(Stm32GcodeRunner::AbstractCommand *command) {
    if (cmd == nullptr) {
        cmd = command;
        cmd->setContext(this);
        return true;
    }
    return false;
}
