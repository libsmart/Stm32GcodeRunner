/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_STM32GCODERUNNERCPPWRAPPER_HPP
#define LIBSMART_STM32GCODERUNNER_STM32GCODERUNNERCPPWRAPPER_HPP

#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sets up the thread for running the STM32 G-code runner.
 *
 * This function sets up the thread for running the STM32 G-code runner, allowing it to execute G-code commands.
 *
 * @param byte_pool The byte pool that will be used by the thread for memory allocation.
 * @return UINT Returns TX_SUCCESS if the thread is successfully set up, or an error code if the setup fails.
 */
extern UINT Stm32GcodeRunner_setupThread(TX_BYTE_POOL *byte_pool);

#ifdef __cplusplus
}
#endif


#endif //LIBSMART_STM32GCODERUNNER_STM32GCODERUNNERCPPWRAPPER_HPP
