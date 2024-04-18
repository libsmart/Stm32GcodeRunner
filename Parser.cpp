/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Parser.hpp"
#include "Stm32ItmLogger.h"

extern Debugger *DBG;

inline void char_to_uppercase(char &c) {
    if (c >= 'a' && c <= 'z') {
        c = c - ('a' - 'A');
    }
}

void Stm32GcodeRunner::Parser::parseString(const char *inputString, uint32_t strlen) {

    Debugger_log(DBG, "Stm32GcodeRunner::Parser::parseString('%.*s')", strlen, inputString);

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
                // Skip spaces
                break;

            case 'G':
            case 'T':
            case 'M': {
                // command found
                command_letter = c;
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
                break;
        }
    }

    // Call the commands
    if (command_letter == '?') return; // no valid command found
    if (command_number < 0) return; // no valid command found


    Debugger_log(DBG, "Found command: %c%d", command_letter, command_number);

#define LOG_LETTER_PARAM(letter) \
     if(pVal_##letter != nullptr) \
     Debugger_log(DBG, #letter": f=%f  l=%ld", strtod(pVal_##letter, nullptr), strtol(pVal_##letter, nullptr, 10))

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

}

void Stm32GcodeRunner::Parser::parseString(const char *inputString) {
    return parseString(inputString, strlen(inputString));
}
