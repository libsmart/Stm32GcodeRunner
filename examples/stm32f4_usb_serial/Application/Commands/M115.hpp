/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * Firmware Info
 * M115
 */

#ifndef NUCLEO_F429ZI_THREADX_SSH_M115_HPP
#define NUCLEO_F429ZI_THREADX_SSH_M115_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "Helper.hpp"

class M115 : public Stm32GcodeRunner::AbstractCommand {
public:
    M115() { isSync = true; }

    const char *getName() override {
        return F("M115");
    }

    preFlightCheckReturn preFlightCheck() override {
        auto result = AbstractCommand::preFlightCheck();
        result = preFlightCheckReturn::READY;
        return result;
    }

    initReturn init() override {
        auto result = AbstractCommand::init();
        result = initReturn::READY;
        return result;
    }

    runReturn run() override {
        auto result = AbstractCommand::run();
        out()->print(F("FIRMWARE: "));
        out()->print(FIRMWARE_NAME);
        out()->flush();
        out()->print(F(" v"));
        out()->print(FIRMWARE_VERSION);
        out()->flush();
        out()->println(F(" (c) 2024 easy-smart solution GmbH"));

        result = runReturn::FINISHED;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();
        result = cleanupReturn::OK;
        return result;
    }
};

#endif //NUCLEO_F429ZI_THREADX_SSH_M115_HPP
