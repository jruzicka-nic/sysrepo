/**
 * @file shm_mod.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief header for mod SHM routines
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

#ifndef _SHM_MOD_H
#define _SHM_MOD_H

#include "shm_types.h"
#include "sysrepo_types.h"

/** macro for getting a SHM module on a specific index */
#define SR_SHM_MOD_IDX(mod_shm_addr, idx) ((sr_mod_t *)(((char *)mod_shm_addr) + sizeof(sr_mod_shm_t) + idx * sizeof(sr_mod_t)))

/**
 * @brief Open (and init if needed) Mod SHM.
 *
 * @param[in,out] shm SHM structure to use.
 * @param[in] zero Whether to zero (or init) Mod SHM.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_open(sr_shm_t *shm, int zero);

/**
 * @brief Find a specific SHM module.
 *
 * @param[in] mod_shm Mod SHM.
 * @param[in] name Name of the module.
 * @return Found SHM module, NULL if not found.
 */
sr_mod_t *sr_shmmod_find_module(sr_mod_shm_t *mod_shm, const char *name);

/**
 * @brief Find a specific SHM module RPC.
 *
 * @param[in] mod_shm Mod SHM.
 * @param[in] path Path of the RPC/ation.
 * @return Found SHM RPC, NULL if not found.
 */
sr_rpc_t *sr_shmmod_find_rpc(sr_mod_shm_t *mod_shm, const char *path);

/**
 * @brief Remap mod SHM and store modules and all their static information (name, deps, ...) in it.
 *
 * @param[in] mod_shm Mod SHM.
 * @param[in] first_sr_mod First SR module to add.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_store_modules(sr_mod_shm_t *mod_shm, struct lyd_node *first_sr_mod);

/**
 * @brief Load modules stored in mod SHM into a context.
 *
 * @param[in] conn Connection to use.
 * @param[in,out] ly_ctx libyang context to update.
 * @param[in] skip_mod_name Optional module name to skip.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_ctx_load_modules(sr_conn_ctx_t *conn, struct ly_ctx *ly_ctx, const char *skip_mod_name);

/**
 * @brief Collect required modules found in an edit.
 *
 * @param[in] edit Edit to be applied.
 * @param[in,out] mod_info Mod info to add to.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_collect_edit(const struct lyd_node *edit, struct sr_mod_info_s *mod_info);

/**
 * @brief Collect required modules for evaluating XPath and getting selected data.
 *
 * @param[in] ly_ctx libyang context.
 * @param[in] xpath XPath to be evaluated.
 * @param[in] ds Target datastore where the @p xpath will be evaluated.
 * @param[in] store_xpath Whether to store @p xpath as module xpath (filtering required data).
 * @param[in,out] mod_info Mod info to add to.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_collect_xpath(const struct ly_ctx *ly_ctx, const char *xpath, sr_datastore_t ds,
        int store_xpath, struct sr_mod_info_s *mod_info);

/**
 * @brief Get SHM dependencies of an RPC/action.
 *
 * @param[in] main_shm Main SHM.
 * @param[in] path Path identifying the RPC/action.
 * @param[in] output Whether this is the RPC/action output or input.
 * @param[out] shm_deps Main SHM dependencies.
 * @param[out] shm_dep_count Dependency count.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_get_rpc_deps(sr_main_shm_t *main_shm, const char *path, int output, sr_dep_t **shm_deps,
        uint16_t *shm_dep_count);

/**
 * @brief Get SHM dependencies of a notification.
 *
 * @param[in] main_shm Main SHM.
 * @param[in] notif_mod Module of the notification.
 * @param[in] path Path identifying the notification.
 * @param[out] shm_deps Main SHM dependencies.
 * @param[out] shm_dep_count Dependency count.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_get_notif_deps(sr_main_shm_t *main_shm, const struct lys_module *notif_mod, const char *path,
        sr_dep_t **shm_deps, uint16_t *shm_dep_count);

/**
 * @brief Collect required module dependencies from a SHM dependency array.
 *
 * @param[in] main_shm Main SHM.
 * @param[in] shm_deps Array of SHM dependencies.
 * @param[in] shm_dep_count Number of @p shm_deps.
 * @param[in] ly_ctx libyang context.
 * @param[in] data Data to look for instance-identifiers in.
 * @param[in,out] mod_info Mod info to add to.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_collect_deps(sr_main_shm_t *main_shm, sr_dep_t *shm_deps, uint16_t shm_dep_count,
        struct ly_ctx *ly_ctx, const struct lyd_node *data, struct sr_mod_info_s *mod_info);

/**
 * @brief Collect required modules of (MOD_INFO_REQ & MOD_INFO_CHANGED) | MOD_INFO_INV_DEP modules in mod info.
 * Other modules will not be validated.
 *
 * @param[in] mod_info Mod info with the modules and data.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_collect_deps_modinfo(struct sr_mod_info_s *mod_info);

/**
 * @brief Information structure for the SHM module recovery callback.
 */
struct sr_shmmod_recover_cb_s {
    const struct lys_module *ly_mod;
    sr_datastore_t ds;
    struct srplg_ds_s *ds_plg;
};

/**
 * @brief Recovery callback for SHM module data locks.
 * Recover possibly backed-up data file.
 */
void sr_shmmod_recover_cb(sr_lock_mode_t mode, sr_cid_t cid, void *data);

/**
 * @brief READ lock all modules in mod info.
 *
 * @param[in] mod_info Mod info to use.
 * @param[in] upgradeable Whether the lock will be upgraded to WRITE later. Used only for main DS of @p mod_info!
 * @param[in] sid Sysrepo session ID.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_modinfo_rdlock(struct sr_mod_info_s *mod_info, int upgradeable, uint32_t sid);

/**
 * @brief WRITE lock all modules in mod info. Secondary DS modules, if any, are READ locked.
 *
 * @param[in] mod_info Mod info to use.
 * @param[in] sid Sysrepo session ID.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_modinfo_wrlock(struct sr_mod_info_s *mod_info, uint32_t sid);

/**
 * @brief Upgrade READ lock on modules in mod info to WRITE lock.
 * Works only for upgradeable READ lock, in which case there will only be one
 * thread waiting for WRITE lock.
 *
 * @param[in] mod_info Mod info to use.
 * @param[in] sid Sysrepo session ID.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_modinfo_rdlock_upgrade(struct sr_mod_info_s *mod_info, uint32_t sid);

/**
 * @brief Downgrade WRITE lock on modules in mod info to READ lock.
 * Works only for upgraded READ lock.
 *
 * @param[in] mod_info Mod info to use.
 * @param[in] sid Sysrepo session ID.
 * @return err_info, NULL on success.
 */
sr_error_info_t *sr_shmmod_modinfo_wrlock_downgrade(struct sr_mod_info_s *mod_info, uint32_t sid);

/**
 * @brief Unlock mod info.
 *
 * @param[in] mod_info Mod info to use.
 */
void sr_shmmod_modinfo_unlock(struct sr_mod_info_s *mod_info);

/**
 * @brief Release any locks matching the provided SID.
 *
 * @param[in] conn Connection to use.
 * @param[in] sid Sysrepo session ID.
 */
void sr_shmmod_release_locks(sr_conn_ctx_t *conn, uint32_t sid);

#endif /* _SHM_MOD_H */
