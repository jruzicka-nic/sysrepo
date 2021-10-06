/**
 * @file context_change.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief header for sysrepo context change routines
 *
 * @copyright
 * Copyright (c) 2021 Deutsche Telekom AG.
 * Copyright (c) 2021 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef _CONTEXT_CHANGE_H
#define _CONTEXT_CHANGE_H

#include "common_types.h"
#include "sysrepo_types.h"

/**
 * @brief Lock context and update it if needed.
 *
 * @param[in] conn Connection to use.
 * @param[in] mode Requested lock mode.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lycc_lock(sr_conn_ctx_t *conn, sr_lock_mode_t mode);

/**
 * @brief Relock context.
 *
 * @param[in] conn Connection to use.
 * @param[in] mode Requested lock mode.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lycc_relock(sr_conn_ctx_t *conn, sr_lock_mode_t mode);

/**
 * @brief Unlock context after it is no longer accessed.
 *
 * @param[in] conn Connection to use.
 * @param[in] mode Lock mode.
 */
void sr_lycc_unlock(sr_conn_ctx_t *conn, sr_lock_mode_t mode);

/**
 * @brief Check that a changed context can be used in the current state of sysrepo.
 *
 * @param[in] conn Connection to use.
 * @param[in] ly_ctx Context to check.
 * @param[in] mod_data Optional new module initial data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lycc_check(sr_conn_ctx_t *conn, const struct ly_ctx *ly_ctx, const struct lyd_node *mod_data);

#endif
