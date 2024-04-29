/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "AbstractCommand.hpp"
#include "Stm32GcodeRunner.hpp"
#include "Helper.hpp"

Stm32GcodeRunner::AbstractCommand::AbstractCommand() {
//    Stm32GcodeRunner::parser.registerCommand(this);
}

void Stm32GcodeRunner::AbstractCommand::setCommandLine(const char *cmdLine, const size_t size) {
    auto sz = std::min(size, sizeof commandLine - 1);
    memset(commandLine, 0, sizeof commandLine);
    memcpy(commandLine, cmdLine, sz);
}

const char * Stm32GcodeRunner::AbstractCommand::getCommandLine() {
    return commandLine;
}

void Stm32GcodeRunner::AbstractCommand::setRunTimeout(unsigned long timeout) {
    runTimeout = timeout;
}

unsigned long Stm32GcodeRunner::AbstractCommand::getRunDuration() const {
    return firstRunMillis > 0 ? millis() - firstRunMillis : 0;
}

void Stm32GcodeRunner::AbstractCommand::setQuiet(bool quiet) {
    quietRun = quiet;
}

bool Stm32GcodeRunner::AbstractCommand::isQuietRun() const {
    return quietRun;
}

void Stm32GcodeRunner::AbstractCommand::setContext(Stm32GcodeRunner::CommandContext *cmdContext) {
    ctx = cmdContext;
}

bool Stm32GcodeRunner::AbstractCommand::write(const char *str) {
    if (ctx == nullptr) return false;
    return ctx->cmdOutputBuffer.write(str);
}

bool Stm32GcodeRunner::AbstractCommand::write(const void *in, size_t strlen) {
    if (ctx == nullptr) return false;
    return ctx->cmdOutputBuffer.write(in, strlen);
}

bool Stm32GcodeRunner::AbstractCommand::printf(const char *format, ...) {
    if (ctx == nullptr) return false;
    va_list args;
    va_start(args, format);
    auto ret = ctx->cmdOutputBuffer.vprintf(format, args);
    va_end(args);
    return ret;
}

void Stm32GcodeRunner::AbstractCommand::setParam(char paramName, const char *paramString) {
    setParam(paramName, strtol(paramString, nullptr, 10));
    setParam(paramName, strtod(paramString, nullptr));
}

