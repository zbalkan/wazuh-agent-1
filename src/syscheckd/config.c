/* Copyright (C) 2009 Trend Micro Inc.
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#include "shared.h"
#include "syscheck.h"
#include "config/config.h"

#ifdef WIN32
static char *SYSCHECK_EMPTY[] = { NULL };
static registry REGISTRY_EMPTY[] = { { NULL, 0 } };
#endif


int Read_Syscheck_Config(const char *cfgfile)
{
    int modules = 0;

    modules |= CSYSCHECK;

    syscheck.rootcheck      = 0;
    syscheck.disabled       = 0;
    syscheck.skip_nfs       = 0;
    syscheck.scan_on_start  = 1;
    syscheck.time           = SYSCHECK_WAIT * 2;
    syscheck.ignore         = NULL;
    syscheck.ignore_regex   = NULL;
    syscheck.nodiff         = NULL;
    syscheck.nodiff_regex   = NULL;
    syscheck.scan_day       = NULL;
    syscheck.scan_time      = NULL;
    syscheck.dir            = NULL;
    syscheck.opts           = NULL;
    syscheck.realtime       = NULL;
    syscheck.wdata          = NULL;
#ifdef WIN32
    syscheck.registry       = NULL;
    syscheck.reg_fp         = NULL;
    syscheck.max_fd_win_rt  = 0;
#endif
    syscheck.prefilter_cmd  = NULL;

    mdebug2("Reading Configuration [%s]", cfgfile);

    /* Read config */
    if (ReadConfig(modules, cfgfile, &syscheck, NULL) < 0) {
        return (OS_INVALID);
    }

#ifdef CLIENT
    mdebug2("Reading Client Configuration [%s]", cfgfile);

    /* Read shared config */
    modules |= CAGENT_CONFIG;
    ReadConfig(modules, AGENTCONFIG, &syscheck, NULL);
#endif

#ifndef WIN32
    /* We must have at least one directory to check */
    if (!syscheck.dir || syscheck.dir[0] == NULL) {
        return (1);
    }
#else
    /* We must have at least one directory or registry key to check. Since
       it's possible on Windows to have syscheck enabled but only monitoring
       either the filesystem or the registry, both lists must be valid,
       even if empty.
     */
    if (!syscheck.dir) {
        syscheck.dir = SYSCHECK_EMPTY;
    }
    if (!syscheck.registry) {
            syscheck.registry = REGISTRY_EMPTY;
    }
    if ((syscheck.dir[0] == NULL) && (syscheck.registry[0].entry == NULL)) {
        return (1);
    }
    syscheck.max_fd_win_rt = getDefine_Int("syscheck", "max_fd_win_rt", 256, 1024);
#endif

    return (0);
}
