/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */


/**
 * Size of the stack for the worker thread.
 */
#undef LIBSMART_GCODERUNNER_WORKER_THREAD_STACK_SIZE
#define LIBSMART_GCODERUNNER_WORKER_THREAD_STACK_SIZE 1024


/**
 * Number of commands that can be registered to the registry.
 */
#undef LIBSMART_GCODERUNNER_COMMAND_REGISTRY_SIZE
#define LIBSMART_GCODERUNNER_COMMAND_REGISTRY_SIZE 12


