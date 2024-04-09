"""
Copyright (C) 2015-2024, Wazuh Inc.
Created by Wazuh, Inc. <info@wazuh.com>.
This program is free software; you can redistribute it and/or modify it under the terms of GPLv2
"""
from pathlib import Path
from wazuh_testing.tools.monitors import file_monitor
from wazuh_testing.utils import callbacks
from wazuh_testing.constants.paths.logs import WAZUH_LOG_PATH

# Constants & base paths
TEST_DATA_PATH = Path(Path(__file__).parent, 'data')
TEST_CASES_PATH = Path(TEST_DATA_PATH, 'test_cases')
CONFIGURATIONS_PATH = Path(TEST_DATA_PATH, 'configuration_templates')


def build_tc_config(tc_conf_list):
    '''
    Build the configuration for each test case.

    Args:
        tc_conf_list (list): List of test case configurations.

    Returns:
        list: List of configurations for each test case.
    '''

    config_list = []  # List of configurations for each test case

    # Build the configuration for each test case
    for tc_config in tc_conf_list:
        sections = []
        # Build the configuration for each localfile
        for i, elements in enumerate(tc_config, start=1):
            section = {
                "section": "localfile",
                "attributes": [{"unique_id": str(i)}],  # Prevents duplicated localfiles sections
                "elements": elements
            }
            sections.append(section)

        config_list.append({"sections": sections})

    return config_list


def assert_list_logs(regex_messages: list):
    '''
    Asserts if the expected messages are present in the log file.

    TODO: This function must guarantee the order of the logs and move to a common module

    Args:
        regex_messages (list): List of regular expressions to search in the log file.
    '''

    # Monitor the ossec.log file
    log_monitor = file_monitor.FileMonitor(WAZUH_LOG_PATH)

    for regex in regex_messages:
        log_monitor.start(callback=callbacks.generate_callback(regex))
        assert (log_monitor.callback_result != None), f'Did not receive the expected messages in the log file. Expected: {regex}'

def assert_not_list_logs(regex_messages: list):
    '''
    Asserts if the expected messages are not present in the log file, the timeout is set to 0.

    The function will return an assertion error if the expected messages are found in the log file.
    The function dont wait for the messages to appear in the log file, reads the current content of the file.
    
    Args:
        regex_messages (list): List of regular expressions to search in the log file.
    '''

    # Monitor the ossec.log file
    log_monitor = file_monitor.FileMonitor(WAZUH_LOG_PATH)

    for regex in regex_messages:
        log_monitor.start(callback=callbacks.generate_callback(regex), timeout=0)
        assert (log_monitor.callback_result == None), f'Received the expected messages in the log file. Expected: {regex}'
