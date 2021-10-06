/**
 * @file context_change.c
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief Sysrepo context change routines
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

#include "context_change.h"

#include "common.h"
#include "common_types.h"
#include "sysrepo_types.h"

sr_error_info_t *
sr_lycc_lock(sr_conn_ctx_t *conn, sr_lock_mode_t mode)
{
    sr_error_info_t *err_info = NULL;
    sr_main_shm_t *main_shm = SR_CONN_MAIN_SHM(conn);
    sr_lock_mode_t remap_mode = SR_LOCK_NONE;

    /* LOCK */
    if ((err_info = sr_rwlock(&main_shm->context_lock, SR_CONTEXT_LOCK_TIMEOUT, mode, conn->cid, __func__, NULL, NULL))) {
        return err_info;
    }

    if (main_shm->content_id != conn->content_id) {
        /* REMAP LOCK */
        if ((err_info = sr_rwlock(&conn->remap_lock, SR_CONN_REMAP_LOCK_TIMEOUT, SR_LOCK_WRITE, conn->cid, func, NULL, NULL))) {
            goto cleanup_unlock;
        }
        remap_mode = SR_LOCK_WRITE;

        /* remap main SHM */
        if ((err_info = sr_shm_remap(&conn->main_shm, 0))) {
            goto cleanup_unlock;
        }

        /* REMAP DOWNGRADE */
        if ((err_info = sr_rwrelock(&conn->remap_lock, SR_CONN_REMAP_LOCK_TIMEOUT, SR_LOCK_READ, conn->cid, func, NULL,
                NULL))) {
            goto cleanup_unlock;
        }
        remap_mode = SR_LOCK_READ;

        /* context was updated, destroy it */
        ly_ctx_destroy(conn->ly_ctx);
        conn->ly_ctx = NULL;

        /* create it again from SHM modules */
        if ((err_info = sr_shmmain_ctx_load_modules(conn, conn->ly_ctx))) {
            goto cleanup_unlock;
        }
        conn->content_id = main_shm->content_id;
    }

cleanup_unlock:
    if (remap_mode) {
        /* REMAP UNLOCK */
        sr_rwunlock(&conn->remap_lock, SR_CONN_REMAP_LOCK_TIMEOUT, remap_mode, conn->cid, func);
    }

    if (err_info) {
        /* UNLOCK */
        sr_rwunlock(&main_shm->context_lock, SR_CONTEXT_LOCK_TIMEOUT, mode, conn->cid, __func__);
    }
    return err_info;
}

sr_error_info_t *
sr_lycc_relock(sr_conn_ctx_t *conn, sr_lock_mode_t mode)
{

}

void
sr_lycc_unlock(sr_conn_ctx_t *conn, sr_lock_mode_t mode)
{
    sr_main_shm_t *main_shm = SR_CONN_MAIN_SHM(conn);

    /* UNLOCK */
    sr_rwunlock(&main_shm->context_lock, SR_CONTEXT_LOCK_TIMEOUT, mode, conn->cid, __func__);
}

sr_error_info_t *
sr_lycc_check(sr_conn_ctx_t *conn, const struct ly_ctx *ly_ctx, const struct lyd_node *mod_data)
{

}
