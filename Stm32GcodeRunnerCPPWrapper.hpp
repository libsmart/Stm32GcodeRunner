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
 * This function should be called from App_ThreadX_Init() in file app_threadx.c:
 * @example @code
 * // Jump to our C++ thread setup function
 * ret = Stm32GcodeRunner_setupThread(byte_pool);
 * assert_param(ret == TX_SUCCESS);
 * @endcode
 *
 * @param byte_pool The byte pool that will be used by the thread for memory allocation.
 * @return UINT Returns TX_SUCCESS if the thread is successfully set up, or an error code if the setup fails.
 */
extern UINT Stm32GcodeRunner_setupThread(TX_BYTE_POOL *byte_pool);

#ifdef __cplusplus
}
#endif


#endif //LIBSMART_STM32GCODERUNNER_STM32GCODERUNNERCPPWRAPPER_HPP
