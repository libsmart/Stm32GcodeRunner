/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Parser.hpp"
#include "Stm32GcodeRunner.hpp"
#include "AbstractCommand.hpp"

#define LOG_LETTER_PARAM(letter) \
     if(pVal_##letter != nullptr) \
     log(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING) \
        ->printf(#letter": f=%f  l=%ld\r\n", strtod(pVal_##letter, nullptr), strtol(pVal_##letter, nullptr, 10));

#define SET_LETTER_PARAM(letter) \
     if(pVal_##letter != nullptr) \
     cmd->setParam(#letter[0], pVal_##letter);

inline void char_to_uppercase(char &c) {
    if (c >= 'a' && c <= 'z') {
        c = c - ('a' - 'A');
    }
}

using namespace Stm32GcodeRunner;

Parser::parserReturn Parser::parseArgcArgv(AbstractCommand * &cmd, int argc, const char *const *argv) {

    if(argc < 1) return parserReturn::UNKNOWN_COMMAND;

    AbstractCommand *tmpCmd = findCommand(argv[0]);
    if (tmpCmd == nullptr) return parserReturn::UNKNOWN_COMMAND;
    cmd = tmpCmd;

    // Command inherits logger from the parser
    cmd->setLogger(getLogger());
    // cmd->setCommandLine(inputString, strlen);

    for(int i = 1; i < argc; i++) {
        cmd->setParam(argv[i][0], &argv[i][1]);
    }

    return parserReturn::OK;
}

Parser::parserReturn Parser::parseString(AbstractCommand * &cmd, const char *inputString, uint32_t strlen) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32GcodeRunner::Parser::parseString('%.*s')\r\n", strlen, inputString);

    if (strlen == 0) return parserReturn::WHITESPACE_STRING;

    bool nonWhiteSpace = false;
    char command_letter = '?';
    int command_number = -1;
    __attribute__((unused)) const char *pVal_A = nullptr;
    __attribute__((unused)) const char *pVal_B = nullptr;
    __attribute__((unused)) const char *pVal_C = nullptr;
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

            case 'E':
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
                        case 'A':
                            pVal_A = inputString + ++index;
                        break;

                        case 'B':
                            pVal_B = inputString + ++index;
                        break;

                        case 'C':
                            index++;
                            pVal_C = pVal_C != nullptr ? pVal_C : (inputString + index);
                        break;

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

    log(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)->printf("Parsed possible command: %c%d\r\n",
                                                                      command_letter, command_number);

    LOG_LETTER_PARAM(A);
    LOG_LETTER_PARAM(B);
    LOG_LETTER_PARAM(C);
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

    // Command inherits logger from the parser
    cmd->setLogger(getLogger());
    cmd->setCommandLine(inputString, strlen);

    SET_LETTER_PARAM(A);
    SET_LETTER_PARAM(B);
    SET_LETTER_PARAM(C);
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

Parser::cmdRegistryType Parser::cmdRegistry = {};

Parser::registerCommandReturn Parser::registerCommand(AbstractCommand *cmd) {
    log(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32GcodeRunner::Parser::registerCommand(%s *cmd)\r\n", cmd->getName());

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

AbstractCommand *Parser::findCommand(const char *cmdName) {
    for (const auto &item: cmdRegistry) {
        if (item == nullptr) continue;
        if (strcmp(item->getName(), cmdName) == 0) {
            // Found
            return item;
        }
    }

    return nullptr;
}
