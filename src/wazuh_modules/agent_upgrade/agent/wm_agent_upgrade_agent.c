/*
 * Wazuh Module for Agent Upgrading
 * Copyright (C) 2015-2020, Wazuh Inc.
 * July 30, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */
#ifdef CLIENT

#include "wazuh_modules/wmodules.h"
#include "wm_agent_upgrade_agent.h"

const char* upgrade_values[] = {
    [WM_UPGRADE_SUCCESSFULL] = "0",
    [WM_UPGRADE_FAILED] = "2"
};

const char* upgrade_messages[] = {
    [WM_UPGRADE_SUCCESSFULL] = "Upgrade was successful",
    [WM_UPGRADE_FAILED]      = "Upgrade failed"
};

/* TODO: This was copied from task-manager, but should be in common location */
static const char *task_statuses[] = {
    [WM_UPGRADE_SUCCESSFULL] = "Done",
    [WM_UPGRADE_FAILED] = "Failed"
};

/**
 * Reads the upgrade_result file if it is present and sends the upgrade result message to the manager.
 * Example message:
 * {
 *   "command": "agent_upgraded/agent_upgrade_failed",
 *   "params":  {
 *     "error": 0/{ERROR_CODE},
 *     "message": "Upgrade was successfull"
 *   }
 * }
 * @param queue_fd File descriptor of the upgrade queue
 * @param state upgrade result state
 * */
static void wm_upgrade_agent_send_ack_message(int queue_fd, wm_upgrade_agent_state state);

/**
 * Checks in the upgrade_results file for a code that determines the result
 * of the upgrade operation, then sends it to the current manager
 * @param queue_fd file descriptor of the queue where the notification will be sent
 * @return a flag indicating if any result was found
 * @retval true information was found on the upgrade_result file
 * @retval either the upgrade_result file does not exist or contains invalid information
 * */
static bool wm_upgrade_agent_search_upgrade_result(int queue_fd);

void wm_agent_upgrade_check_status(wm_agent_configs agent_config) {
    /**
     *  StartMQ will wait until agent connection which is when the pkg_install.sh will write 
     *  the upgrade result
    */
    int queue_fd = StartMQ(DEFAULTQPATH, WRITE, MAX_OPENQ_ATTEMPS);

    if (queue_fd < 0) {
        mterror(WM_AGENT_UPGRADE_LOGTAG, WM_UPGRADE_QUEUE_FD);
    } else {
        bool result_available = true;
        unsigned int wait_time = agent_config.upgrade_wait_start;
        /**
         * This loop will send the upgrade result notification to the manager
         * If the manager is able to update the upgrade status will notify the agent
         * erasing the result file and exiting this loop 
         * */
        while (result_available) {
            result_available = wm_upgrade_agent_search_upgrade_result(queue_fd);

            if(result_available) {
                sleep(wait_time);

                wait_time *= agent_config.ugprade_wait_factor_increase;
                if (wait_time > agent_config.upgrade_wait_max) {
                    wait_time = agent_config.upgrade_wait_max;
                }
            }
        }
        close(queue_fd);
    }
}

static void wm_upgrade_agent_send_ack_message(int queue_fd, wm_upgrade_agent_state state) {
    int msg_delay = 1000000 / wm_max_eps;
    cJSON* root = cJSON_CreateObject();
    cJSON* params = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "command", WM_UPGRADE_AGENT_UPDATED_COMMAND);
    cJSON_AddNumberToObject(params, "error", atoi(upgrade_values[state]));
    cJSON_AddStringToObject(params, "message", upgrade_messages[state]);
    cJSON_AddStringToObject(params, "status", task_statuses[state]);
    cJSON_AddItemToObject(root, "params", params);

    char *msg_string = cJSON_PrintUnformatted(root);
    if (wm_sendmsg(msg_delay, queue_fd, msg_string, WM_AGENT_UPGRADE_MODULE_NAME, UPGRADE_MQ) < 0) {
        mterror(WM_AGENT_UPGRADE_LOGTAG, QUEUE_ERROR, DEFAULTQUEUE, strerror(errno));
    }

    mtdebug1(WM_AGENT_UPGRADE_LOGTAG, WM_UPGRADE_ACK_MESSAGE, msg_string);
    os_free(msg_string);
    cJSON_Delete(root);
}

static bool wm_upgrade_agent_search_upgrade_result(int queue_fd) {
    char buffer[20];
    FILE * result_file;
    const char * PATH = WM_AGENT_UPGRADE_RESULT_FILE;

    if (result_file = fopen(PATH, "r"), result_file) {
        fgets(buffer, 20, result_file);
        fclose(result_file);

        wm_upgrade_agent_state state;
        for(state = 0; state < WM_UPGRADE_MAX_STATE; state++) {
            // File can either be "0\n" or "2\n", so we are expecting a positive match
            if (strstr(buffer, upgrade_values[state]) != NULL) {
                // Matched value, send message
                wm_upgrade_agent_send_ack_message(queue_fd, state);
                return true;
            }
        }
    }
    return false;
}

#endif
