/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Parser.hpp"
#include "Stm32ItmLogger.h"
#include "Stm32GcodeRunner.hpp"

extern Debugger *DBG;

#define LOG_LETTER_PARAM(letter) \
     if(pVal_##letter != nullptr) \
     Debugger_log(DBG, #letter": f=%f  l=%ld", strtod(pVal_##letter, nullptr), strtol(pVal_##letter, nullptr, 10));

#define SET_LETTER_PARAM(letter) \
     if(pVal_##letter != nullptr) \
     cmd->setParam(#letter[0], pVal_##letter);

inline void char_to_uppercase(char &c) {
    if (c >= 'a' && c <= 'z') {
        c = c - ('a' - 'A');
    }
}

Stm32GcodeRunner::Parser::parserReturn
Stm32GcodeRunner::Parser::parseString(AbstractCommand* &cmd, const char *inputString, uint32_t strlen) {

    Debugger_log(DBG, "Stm32GcodeRunner::Parser::parseString('%.*s')", strlen, inputString);

    if(strlen == 0) return parserReturn::WHITESPACE_STRING;

    bool nonWhiteSpace = false;
    char command_letter = '?';
    int command_number = -1;
    __attribute__((unused)) const char *pVal_S = nullptr;
    __attribute__((unused)) const char *pVal_F = nullptr;
    __attribute__((unused)) const char *pVal_X = nullptr;
    __attribute__((unused)) const char *pVal_Y = nullptr;
    __attribute__((unused)) const char *pVal_Z = nullptr;
    __attribute__((unused)) const char *pVal_U = nullptr;
    __attribute__((unused)) const char *pVal_V = nullptr;
    __attribute__((unused)) const char *pVal_W = nullptr;
    __attribute__((unused)) const char *pVal_R = nullptr;
    __attribute__((unused)) const char *pVal_P = nullptr;
    __attribute__((unused)) const char *pVal_D = nullptr;

    for (uint32_t index = 0; index < strlen; index++) {
        char c = inputString[index];
        char_to_uppercase(c);
        switch (c) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
                // Skip whitespace
                break;

            case 'G':
            case 'T':
            case 'M': {
                // command found
                command_letter = c;
                nonWhiteSpace = true;
                index++;

                // skip spaces
                while (inputString[index] == ' ') index++;

                // read the command number
                while (((c = inputString[index]) >= '0') && (c <= '9')) {
                    if (command_number == -1) command_number = 0;
                    command_number = command_number * 10 + (c - '0');
                    index++;
                }

                while (index < strlen) {
                    c = inputString[index];
                    char_to_uppercase(c);
                    switch (c) {
                        case 'S':
                            pVal_S = inputString + ++index;
                            break;

                        case 'F':
                            pVal_F = inputString + ++index;
                            break;

                        case 'X':
                            pVal_X = inputString + ++index;
                            break;

                        case 'Y':
                            pVal_Y = inputString + ++index;
                            break;

                        case 'Z':
                            pVal_Z = inputString + ++index;
                            break;

                        case 'U':
                            pVal_U = inputString + ++index;
                            break;

                        case 'V':
                            pVal_V = inputString + ++index;
                            break;

                        case 'W':
                            pVal_W = inputString + ++index;
                            break;

                        case 'R':
                            pVal_R = inputString + ++index;
                            break;

                        case 'P':
                            pVal_P = inputString + ++index;
                            break;

                        case 'D':
                            pVal_D = inputString + ++index;
                            break;

                        default:
                            index++;
                            break;
                    }
                }
            }
                break;
            default:
                nonWhiteSpace = true;
                break;
        }
    }

    if (!nonWhiteSpace) return parserReturn::WHITESPACE_STRING;

    // Call the commands
    if (command_letter == '?') return parserReturn::GARBAGE_STRING;
    if (command_number < 0) return parserReturn::GARBAGE_STRING;


    Debugger_log(DBG, "Parsed possible command: %c%d", command_letter, command_number);

    LOG_LETTER_PARAM(S);
    LOG_LETTER_PARAM(F);
    LOG_LETTER_PARAM(X);
    LOG_LETTER_PARAM(Y);
    LOG_LETTER_PARAM(Z);
    LOG_LETTER_PARAM(U);
    LOG_LETTER_PARAM(V);
    LOG_LETTER_PARAM(W);
    LOG_LETTER_PARAM(R);
    LOG_LETTER_PARAM(P);
    LOG_LETTER_PARAM(D);


    // Find command class in registry

    char cmdName[10];
    snprintf(cmdName, sizeof cmdName, "%c%d", command_letter, command_number);
    AbstractCommand *tmpCmd = findCommand(cmdName);
    if (tmpCmd == nullptr) return parserReturn::UNKNOWN_COMMAND;
    cmd = tmpCmd;

    SET_LETTER_PARAM(S);
    SET_LETTER_PARAM(F);
    SET_LETTER_PARAM(X);
    SET_LETTER_PARAM(Y);
    SET_LETTER_PARAM(Z);
    SET_LETTER_PARAM(U);
    SET_LETTER_PARAM(V);
    SET_LETTER_PARAM(W);
    SET_LETTER_PARAM(R);
    SET_LETTER_PARAM(P);
    SET_LETTER_PARAM(D);


    cmd->setCommandLine(inputString, strlen);

    return parserReturn::OK;

    /*
    if (!cmdCtx->setCommand(cmd)) return parserReturn::CONTEXT_NOT_READY;

    switch (Stm32GcodeRunner::worker->enqueueCommandContext(cmdCtx)) {
        case Worker::enqueueCommandReturn::OK_SYNC:
            return parserReturn::OK_SYNC;

        case Worker::enqueueCommandReturn::OK_ASYNC:
            return parserReturn::OK_ASYNC;

        case Worker::enqueueCommandReturn::ERROR:
            return parserReturn::WORKER_ERROR;
    }
     */
}


Stm32GcodeRunner::Parser::registerCommandReturn
Stm32GcodeRunner::Parser::registerCommand(Stm32GcodeRunner::AbstractCommand *cmd) {

    Debugger_log(DBG, "Stm32GcodeRunner::Parser::registerCommand(%s *cmd)", cmd->getName());

    // Check, if command is already registered
    for (const auto &item: cmdRegistry) {
        if (item == nullptr) continue;
        if (strcmp(item->getName(), cmd->getName()) == 0) {
            // Found
            return registerCommandReturn::CMD_ALREADY_REGISTERED;
        }
    }

    for (auto &item: cmdRegistry) {
        if (item == nullptr) {
            item = cmd;
            return registerCommandReturn::SUCCESS;
        }
    }

    return registerCommandReturn::CMD_REGISTRY_FULL;
}

Stm32GcodeRunner::AbstractCommand *Stm32GcodeRunner::Parser::findCommand(const char *cmdName) {
    for (const auto &item: cmdRegistry) {
        if (item == nullptr) continue;
        if (strcmp(item->getName(), cmdName) == 0) {
            // Found
            return item;
        }
    }

    return nullptr;
}
