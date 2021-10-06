/**
 * @file lyd_mods.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief header for sysrepo module data routines
 *
 * @copyright
 * Copyright (c) 2018 - 2021 Deutsche Telekom AG.
 * Copyright (c) 2018 - 2021 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef _LYD_MODS_H
#define _LYD_MODS_H

#include <pthread.h>

#include <libyang/libyang.h>

#include "shm_types.h"
#include "sysrepo_types.h"

/**
 * @brief Lock lydmods lock with a recovery callback.
 *
 * @param[in] lock Lydmods lock to lock.
 * @param[in] ly_ctx libyang context.
 * @param[in] func Name of the calling function for logging.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_lock(pthread_mutex_t *lock, const struct ly_ctx *ly_ctx, const char *func);

/**
 * @brief Parse internal module data.
 *
 * @param[in] ly_ctx Context to use for parsing the data.
 * @param[out] sr_mods_p Sysrepo module data tree.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_parse(struct ly_ctx *ly_ctx, struct lyd_node **sr_mods_p);

/**
 * @brief Add a module to SR internal module data.
 *
 * @param[in] conn Connection to use.
 * @param[in] ly_mod Module that is scheduled to be installed.
 * @param[in] module_ds Datastore implementation plugin name for each config datastore.
 * @param[out] sr_mods SR internal module data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_change_add_module(sr_conn_ctx_t *conn, const struct lys_module *ly_mod,
        const sr_module_ds_t *module_ds, struct lyd_node **sr_mods);

/**
 * @brief Remove a module from SR internal module data.
 *
 * @param[in] conn Connection to use.
 * @param[in] mod_name Module name to delete.
 * @param[out] sr_mods SR internal module data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_change_del_module(sr_conn_ctx_t *conn, const char *mod_name, struct lyd_node **sr_mods);

/**
 * @brief Update a module in SR internal module data.
 *
 * @param[in] conn Connection to use.
 * @param[in] ly_mod Updated module.
 * @param[out] sr_mods SR internal module data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_change_upd_module(sr_conn_ctx_t *conn, const struct lys_module *ly_mod,
        struct lyd_node **sr_mods);

/**
 * @brief Change a feature (enable/disable) of a module in SR internal module data.
 *
 * @param[in] conn Connection to use.
 * @param[in] mod_name Name of the module with the feature.
 * @param[in] feat_name Feature name.
 * @param[in] enable Whether to enable or disable the feature.
 * @param[out] sr_mods SR internal module data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_change_mod_feature(sr_conn_ctx_t *conn, const char *mod_name, const char *feat_name,
        int enable, struct lyd_node **sr_mods);

/**
 * @brief Change replay support of a module in SR internal module data.
 *
 * @param[in] conn Connection to use.
 * @param[in] ly_mod Module to update. NULL to update all the modules.
 * @param[in] enable Whether replay should be enabled or disabled.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_lydmods_change_mod_replay_support(sr_conn_ctx_t *conn, const struct lys_module *ly_mod,
        int enable);

#endif
