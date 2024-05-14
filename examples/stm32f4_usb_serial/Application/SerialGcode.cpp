/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#include "SerialGcode.hpp"
#include "AbstractCommand.hpp"
#include "Stm32GcodeRunner.hpp"

void SerialGcode::loop() {
    Stm32Common::buf_size_signed_t posR = rxBuffer.findPos('\r');
    Stm32Common::buf_size_signed_t posN = rxBuffer.findPos('\n');
    const Stm32Common::buf_size_signed_t eolPos = std::max(posR, posN);
    if (eolPos > 0) {
        Stm32GcodeRunner::AbstractCommand *cmd{};
        const size_t cmdLen = std::min(posR < 0 ? SIZE_MAX : posR, posN < 0 ? SIZE_MAX : posN);
        auto parserRet = Stm32GcodeRunner::parser->parseString(
                cmd, reinterpret_cast<const char *>(rxBuffer.getReadPointer()), cmdLen);
        rxBuffer.remove(eolPos + 1);

        if (parserRet == Stm32GcodeRunner::Parser::parserReturn::OK) {
            Debugger_log(DBG, "Found command: %s", cmd->getName());
            Stm32GcodeRunner::CommandContext *cmdCtx{};
            Stm32GcodeRunner::worker->createCommandContext(cmdCtx);
            if (cmdCtx == nullptr) {
                txBuffer.println("ERROR: Command buffer full");
                return;
            }
            cmdCtx->setCommand(cmd);

            cmdCtx->registerOnWriteFunction([cmdCtx, this]() {
                // Debugger_log(DBG, "onWriteFn()");
                if (cmdCtx->outputLength() > 0) {
                    const auto result = cmdCtx->outputRead(
                            reinterpret_cast<char *>(this->txBuffer.getWritePointer()),
                            this->txBuffer.getRemainingSpace());
                    this->txBuffer.setWrittenBytes(result);
                }
            });

            cmdCtx->registerOnCmdEndFunction([cmdCtx]() {
                // Debugger_log(DBG, "onCmdEndFn()");
                Stm32GcodeRunner::worker->deleteCommandContext(cmdCtx);
            });

            Stm32GcodeRunner::worker->enqueueCommandContext(cmdCtx);
        } else if (parserRet == Stm32GcodeRunner::Parser::parserReturn::UNKNOWN_COMMAND) {
            txBuffer.println("ERROR: UNKNOWN COMMAND");
        } else if (parserRet == Stm32GcodeRunner::Parser::parserReturn::GARBAGE_STRING) {
            txBuffer.println("ERROR: UNKNOWN COMMAND");
        } else {
            // txBuffer.println("ERROR: ONLY WHITESPACE");
        }
    }

    Stm32Serial::loop();
}
