/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32GCODERUNNER_LIBSMART_CONFIG_DIST_HPP
#define LIBSMART_STM32GCODERUNNER_LIBSMART_CONFIG_DIST_HPP

#define LIBSMART_STM32GCODERUNNER

/**
 * Size of the stack for the worker thread.
 */
#undef LIBSMART_GCODERUNNER_WORKER_THREAD_STACK_SIZE
#define LIBSMART_GCODERUNNER_WORKER_THREAD_STACK_SIZE 1024

#undef LIBSMART_GCODERUNNER_OUTPUT_BUFFER_SIZE
#define LIBSMART_GCODERUNNER_OUTPUT_BUFFER_SIZE 192


/**
 * Number of commands that can be registered to the registry.
 */
#undef LIBSMART_GCODERUNNER_COMMAND_REGISTRY_SIZE
#define LIBSMART_GCODERUNNER_COMMAND_REGISTRY_SIZE 12


#endif
