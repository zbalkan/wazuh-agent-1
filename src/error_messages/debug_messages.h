/*
 * Copyright (C) 2015-2020, Wazuh Inc.
 * January 17, 2018.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#ifndef DEBUG_MESSAGES_H
#define DEBUG_MESSAGES_H

/* vulnerability-detector messages*/
#define VU_DOWNLOAD           "(5450): Downloading %s database."
#define VU_START_SCAN         "(5452): Starting vulnerability scanning."
#define VU_END_SCAN           "(5453): Vulnerability scanning finished."
#define VU_START_AG_AN        "(5454): Analyzing agent %s vulnerabilities."
#define VU_DETECTED_VUL       "(5455): Vulnerability %s detected in agent %s affecting: %s."
#define VU_NOT_VULN           "(5456): The '%s' package from agent %s is not vulnerable to %s. Condition: package version (%s) %s %s."
#define VU_UPDATE_DATE        "(5457): %s OVAL is in its latest version. Update date: %s"
#define VU_START_REFRESH_DB   "(5458): Refreshing %s databases."
#define VU_STOP_REFRESH_DB    "(5459): Refresh of %s database finished."
#define VU_DB_TIMESTAMP_FEED  "(5460): %s%s feed is outdated, so the update continues."
#define VU_STARTING_UPDATE    "(5461): Starting %s database update."
#define VU_AGENT_SOFTWARE_REQ "(5462): Getting agent %s software."
#define VU_AGENT_UNSOPPORTED  "(5463): Agent %s has an unsupported Wazuh version: %s."
#define VU_UNS_OS_VERSION     "(5464): %s version not supported (agent %s)."
#define VU_AGENT_PENDING      "(5465): Agent %s operating system could not be obtained because it has never been connected on. It will be omitted."
#define VU_UNS_OS             "(5466): Unsupported OS. Agent %s will be omitted."
#define VU_PACK_VER_VULN      "(5467): The '%s' package from agent %s is vulnerable to %s. Condition: %s."
#define VU_UPDATE_PRE         "(5469): Preparse step."
#define VU_UPDATE_PAR         "(5470): Parse step."
#define VU_UPDATE_VU_CO       "(5471): Inserting vulnerability conditions."
#define VU_UPDATE_VU_INFO     "(5472): Inserting vulnerability info."
#define VU_UPDATE_VU          "(5473): Inserting vulnerabilities."
#define VU_AGENT_INFO_ERROR   "(5474): Agent %s operating system could not be obtained. Maybe it is never connected. It will be omitted."
#define VU_NO_SOFTWARE        "(5475): No changes have been found with respect to the last package inventory or no packages have been indexed for agent %s."
#define VU_AG_NO_TARGET       "(5476): The analysis can not be launched because there are no target agents."
#define VU_AG_DISC            "(5477): The vulnerabilities of the agent %s will not be checked because it is disconnected."
#define VU_LOCAL_FETCH        "(5478): Fetching feed from '%s'."
#define VU_OPERATION_NOT_REC  "(5479): Operation '%s' not recognized."
#define VU_UNEXP_VALUE        "(5482): Unexpected %s attribute."
#define VU_SOL_PATCHES        "(5483): Solving patches."
#define VU_INS_TEST_SEC       "(5484): Inserting test section."
#define VU_SYS_CHECKED        "(5485): The last package inventory for the agent %s (ID: %s) has already been checked. The vulnerability search is omitted."
#define VU_AGENT_START        "(5486): Starting vulnerability assessment for agent %s."
#define VU_AGENT_FINISH       "(5487): Finished vulnerability assessment for agent %s."
#define VU_AG_NEVER_CON       "(5488): Agent '%s' never connected."
#define VU_API_REQ_RETRY      "(5490): There was no valid response to '%s'. Retrying in %d seconds."
#define VU_UNEXP_JSON_KEY     "(5492): Unexpected JSON key: '%s'."
#define VU_ENDING_UPDATE      "(5494): The update of the %s feed finished successfully."
#define VU_DOWNLOAD_FAIL      "(5495): The download can not be completed. Retrying in %d seconds."
#define VU_INS_CPES_SEC       "(5498): Inserting CPEs section."
#define VU_AGENT_CPE_RECV     "(5507): The CPE '%s' from the agent %d was indexed."
#define VU_CPE_GENERATED      "(5509): CPE generated from vendor '%s' and product '%s': '%s'."
#define VU_UPDATING_NVD_YEAR  "(5512): Synchronizing the year %d of the vulnerability database."
#define VU_GEN_CPE_COUNT      "(5514): CPEs generated for agent %s: %d/%d."
#define VU_INS_NVD_SEC        "(5516): Inserting NVD section."
#define VU_SOCKET_RETRY       "(5518): Unable to connect to socket '%s'. Waiting %d seconds."
#define VU_INS_CPES_DIC       "(5523): Inserting Wazuh's CPE dictonary."
#define VU_FUNCTION_TIME      "(5529): It took %ld seconds to %s vulnerabilities in agent %s."
#define VU_INS_MSU            "(5530): Inserting Microsoft Security Update dictonary."
#define VU_HOTFIX_VUL         "(5533): Agent %s is vulnerable to %s. Condition: %s"
#define VU_HOTFIX_INSTALLED   "(5534): Agent %s has installed %s that corrects the vulnerability %s."
#define VU_UPDATE_JSON_FEED   "(5537): Updating from '%s'."
#define VU_INDEX_TIME         "(5538): It took %ld seconds to %s vulnerabilities."
#define VU_UPDATING_RH_YEAR   "(5539): Synchronizing the page %d from the vulnerability database."
#define VU_INS_RH_SEC         "(5543): Inserting Red Hat section."
#define VU_OSINFO_DISABLED    "(5544): Could not get the agent %s os_release, so its OS vulnerabilities will not be analysed. It may not have os_info scan activated."
#define VU_ANAL_ACC_REPORTS   "(5545): Analyzing accumulated reports for %s in %s."
#define VU_DISC_ACC_REPORTS   "(5546): Discarding report %s with check %s because %s is higher."
#define VU_ADD_ACC_REPORTS    "(5547): Setting report %s with check %s because %s is lower."
#define VU_NO_VER_REPORT      "(5548): There is no check version in %s, so the vulnerability will be reported directly."
#define VU_UPDATE_PACK_NAME   "(5553): Inserting package names."
#define VU_DOWNLOAD_START     "(5554): Trying to download '%s'."
#define VU_DOWNLOAD_PAGE_SUC  "(5555): Page %d successfully downloaded."
#define VU_NVD_UPD_CANCEL     "(5565): The %s update has failed, so the NVD feed will not be updated."
#define VU_FIND_NVD_VULN_ERR  "(5566): It was not possible to look for the vulnerabilities of %s (%s) in the NVD."
#define VU_INSERT_DATA_ERR    "(5567): It was not possible to insert %s in the agent software table."
#define VU_OVAL_OBJ_INV       "(5569): Invalid OVAL object type: %s."
#define VU_INS_VARIABLES      "(5570): Inserting variables..."
#define VU_DISCARD_MATCH_N    "(5572): The matching configuration node '%s' from %s will be discarded because it is contemplated by '%s' (%s%s%s%s%s%s%s)."
#define VU_AG_FULL_SCAN       "(5573): A full scan will be run on agent %s."
#define VU_AG_PART_SCAN       "(5574): A partial scan will be run on agent %s."
#define VU_NO_PACKAGE_SCAN    "(5576): The package inventory of the agent %s is not available, but a hotfix analysis will be launched."
#define VU_NO_HOTFIX_AVAIL    "(5577): It is not possible to perform a hotfix scan on agent %s."
#define VU_NO_HOTFIX_FEED     "(5578): We have not found a hotfix that solves %s for agent %s in the Microsoft feed, so it is not possible to know it is vulnerable."
#define VU_COMPARISSION_ERROR "(5579): The '%s' and '%s' versions of '%s' package could not be compared. Possible false positive."

/* File integrity monitoring debug messages */
#define FIM_DIFF_SKIPPED                    "(6200): Diff execution skipped for containing insecure characters."
#define FIM_SCHED_BATCH                     "(6201): Setting SCHED_BATCH returned: '%d'"
#define FIM_LOCAL_DIFF_DELETE               "(6202): Deleting backup '%s'. Not monitored anymore."
#define FIM_FILE_IGNORE_RESTRICT            "(6203): Ignoring file '%s' due to restriction '%s'"
#define FIM_IGNORE_ENTRY                    "(6204): Ignoring '%s' '%s' due to '%s'"
#define FIM_IGNORE_SREGEX                   "(6205): Ignoring '%s' '%s' due to sregex '%s'"
#define FIM_TAG_ADDED                       "(6206): Adding tag '%s' to directory '%s'"
#define FIM_READING_REGISTRY                "(6207): Attempt to read: '%s%s'"
#define FIM_CLIENT_CONFIGURATION            "(6208): Reading Client Configuration [%s]"
#define FIM_LOCALDIFF_DELETE                "(6209): Deleting '%s' from local hash table."
#define FIM_CANNOT_ACCESS_FILE              "(6210): Cannot access '%s': it was removed during scan."
#define FIM_SCANNING_FILE                   "(6211): File '%s'"
#define FIM_SIMBOLIC_LINK_DISABLE           "(6212): Follow symbolic links disabled."
#define FIM_CHECK_LINK_REALPATH             "(6213): Cannot get the real path of the link '%s'"
#define FIM_HASH_ADD_FAIL                   "(6214): Not enough memory to add inode to db: '%s' (%s) "
#define FIM_HASH_UPDATE                     "(6215): Updating path '%s' in inode hash table: '%s' (%s) "
#define FIM_SCANNING_IRREGFILE              "(6216): IRREG File: '%s'"
#define FIM_MAX_RECURSION_LEVEL             "(6217): Maximum level of recursion reached. Depth:%d recursion_level:%d '%s'"
#define FIM_SYMBOLIC_LINK_DISCARDED         "(6218): Discarding symbolic link '%s' is already added in the configuration."
#define FIM_SYMBOLIC_LINK_ADD               "(6219): Directory added to FIM configuration by link '%s'"
#define FIM_FILE_MSG_DELETE                 "(6220): Sending delete message for file: '%s'"
#define FIM_FREQUENCY_DIRECTORY             "(6221): Directory loaded from syscheck db: '%s'"
#define FIM_STAT_FAILED                     "(6222): Stat() function failed on: '%s' due to [(%d)-(%s)]"
#define FIM_SKIP_NFS                        "(6223): FIM skip_nfs=%d, '%s'::is_nfs=%d"
#define FIM_REALTIME_HASH_DUP               "(6224): Entry '%s' already exists in the RT hash table."
#define FIM_REALTIME_MONITORING             "(6225): The '%s' directory starts to be monitored in real-time mode."
#define FIM_REALTIME_NEWPATH                "(6226): Scanning new file '%s' with options for directory '%s'"
#define FIM_REALTIME_NEWDIRECTORY           "(6227): Directory added for real time monitoring: '%s'"
#define FIM_REALTIME_DISCARD_EVENT          "(6228): Real-time event with same checksum for file: '%s'. Ignoring it."
#define FIM_WHODATA_HANDLE_UPDATE           "(6229): The handler ('%s') will be updated."
#define FIM_WHODATA_NEWDIRECTORY            "(6230): Monitoring with Audit: '%s'"
#define FIM_WHODATA_SCAN                    "(6231): The '%s' directory has been scanned after detecting event of new files."
#define FIM_WHODATA_SCAN_ABORTED            "(6232): Scanning of the '%s' directory is aborted because something has gone wrong."
#define FIM_WHODATA_CHECKTHREAD             "(6233): Checking thread set to '%d' seconds."
#define FIM_LINK_ALREADY_ADDED              "(6234): Directory '%s' already monitored, ignoring link '%s'"
#define FIM_WHODATA_FULLQUEUE               "(6235): Real-time Whodata events queue for Windows is full. Removing the first '%d'"
#define FIM_WHODATA_EVENT_DELETED           "(6236): '%d' events have been deleted from the whodata list."
#define FIM_WHODATA_EVENTQUEUE_VALUES       "(6237): Whodata event queue values for Windows -> max_size:'%d' | max_remove:'%d' | alert_threshold:'%d'",
#define FIM_WHODATA_IGNORE                  "(6238): The file '%s' has been marked as ignored. It will be discarded."
#define FIM_WHODATA_NOT_ACTIVE              "(6239): '%s' is discarded because its monitoring is not activated."
#define FIM_WHODATA_CANCELED                "(6240): The monitoring of '%s' in whodata mode has been canceled. Added to the ignore list."
#define FIM_WHODATA_DIRECTORY_SCANNED       "(6241): The '%s' directory has been scanned. It does not need to do it again."
#define FIM_WHODATA_NEW_FILES               "(6242): New files have been detected in the '%s' directory after the last scan."
#define FIM_WHODATA_DIRECTORY_DISCARDED     "(6243): The '%s' directory has been discarded because it is not being monitored in whodata mode."
#define FIM_WHODATA_CHECK_NEW_FILES         "(6244): New files have been detected in the '%s' directory and will be scanned."
#define FIM_WHODATA_NO_NEW_FILES            "(6245): The '%s' directory has not been scanned because no new files have been detected. Mask: '%x'"
#define FIM_WHODATA_INVALID_UID             "(6246): Invalid identifier for user '%s'"
#define FIM_AUDIT_EVENT                     "(6247): audit_event: uid=%s, auid=%s, euid=%s, gid=%s, pid=%i, ppid=%i, inode=%s, path=%s, pname=%s",
#define FIM_AUDIT_EVENT1                    "(6248): audit_event_1/2: uid=%s, auid=%s, euid=%s, gid=%s, pid=%i, ppid=%i, inode=%s, path=%s, pname=%s",
#define FIM_AUDIT_EVENT2                    "(6249): audit_event_2/2: uid=%s, auid=%s, euid=%s, gid=%s, pid=%i, ppid=%i, inode=%s, path=%s, pname=%s",
#define FIM_AUDIT_DELETE_RULE               "(6250): Deleting Audit rules."
#define FIM_AUDIT_MATCH_KEY                 "(6251): Match audit_key: '%s'"
#define FIM_HEALTHCHECK_CREATE              "(6252): Whodata health-check: Detected file creation event (%s)"
#define FIM_HEALTHCHECK_DELETE              "(6253): Whodata health-check: Detected file deletion event (%s)"
#define FIM_HEALTHCHECK_UNRECOGNIZED_EVENT  "(6254): Whodata health-check: Unrecognized event (%s)"
#define FIM_HEALTHCHECK_THREAD_ATIVE        "(6255): Whodata health-check: Reading thread active."
#define FIM_HEALTHCHECK_THREAD_FINISED      "(6256): Whodata health-check: Reading thread finished."
#define FIM_HEALTHCHECK_WAIT_CREATE         "(6257): Whodata health-check: Waiting creation event."
#define FIM_HEALTHCHECK_CREATE_RECEIVE      "(6258): Whodata health-check: Creation event received."
#define FIM_HEALTHCHECK_WAIT_DELETE         "(6259): Whodata health-check: Waiting deletion event."
#define FIM_HEALTHCHECK_DELETE_RECEIVE      "(6260): Whodata health-check: Deletion event received."
#define FIM_HEALTHCHECK_SUCCESS             "(6261): Whodata health-check: Success."
#define FIM_HEALTHCHECK_CHECK_RULE          "(6262): Couldn't delete audit health check rule."
#define FIM_SACL_CHECK_CONFIGURE            "(6263): Setting up SACL for '%s'"
#define FIM_SACL_RESTORED                   "(6265): The SACL of '%s' has been restored correctly."
#define FIM_SACL_CONFIGURE                  "(6266): The SACL of '%s' will be configured."
#define FIM_SACL_NOT_FOUND                  "(6267): No SACL found on target. A new one will be created."
#define FIM_ELEVATE_PRIVILEGE               "(6268): The '%s' privilege has been added."
#define FIM_REDUCE_PRIVILEGE                "(6269): The '%s' privilege has been removed."
#define FIM_AUDIT_NEWRULE                   "(6270): Added audit rule for monitoring directory: '%s'"
#define FIM_AUDIT_RULEDUP                   "(6271): Audit rule for monitoring directory '%s' already added."
#define FIM_INOTIFY_ADD_WATCH               "(6272): Unable to add inotify watch to real time monitoring: '%s'. '%d' '%d':'%s'"
#define FIM_AUDIT_NOCONF                    "(6273): Cannot apply Audit config."
#define FIM_AUDIT_NORULES                   "(6274): No rules added. Audit events reader thread will not start."
#define FIM_AUDIT_RELOADING_RULES           "(6275): Reloading Audit rules."
#define FIM_AUDIT_RELOADED_RULES            "(6276): Audit rules reloaded. Rules loaded: %i"
#define FIM_AUDIT_THREAD_STOPED             "(6277): Audit thread finished."
#define FIM_AUDIT_HEALTHCHECK_RULE          "(6278): Couldn't add audit health check rule."
#define FIM_AUDIT_HEALTHCHECK_START         "(6279): Whodata health-check: Starting."
#define FIM_AUDIT_HEALTHCHECK_FILE          "(6280): Couldn't create audit health check file."
#define FIM_SYSCOM_ARGUMENTS                "(6281): SYSCOM %s needs arguments."
#define FIM_SYSCOM_UNRECOGNIZED_COMMAND     "(6282): SYSCOM Unrecognized command '%s'"
#define FIM_SYSCOM_FAIL_GETCONFIG           "(6283): At SYSCOM getconfig: Could not get '%s' section."
#define FIM_SYSCOM_REQUEST_READY            "(6284): Local requests thread ready."
#define FIM_SYSCOM_EMPTY_MESSAGE            "(6285): Empty message from local client."
#define FIM_SYSCOM_THREAD_FINISED           "(6286): Local server thread finished."
#define FIM_CONFIGURATION_FILE              "(6287): Reading configuration file: '%s'"
#define FIM_SCAL_NOREFRESH                  "(6288): Could not refresh the SACL of '%s'. Its event will not be reported."
#define FIM_DISCARD_RECYCLEBIN              "(6289): File '%s' is in the recycle bin. It will be discarded."
#define FIM_REALTIME_ADD                    "(6290): Unable to add directory to real time monitoring: '%s'"
#define FIM_WHODATA_REPLACELINK             "(6291): Replacing the symbolic link: '%s' -> '%s'"
#define FIM_WHODATA_FILENOEXIST             "(6292): The '%s' file does not exist, but this will be notified when the corresponding event is received."
#define FIM_LINKCHECK_TIME                  "(6293): Configured symbolic links will be checked every %d seconds."
#define FIM_LINKCHECK_CHANGE                "(6294): Configured symbolic links will be checked every %d seconds."
#define FIM_LINKCHECK_NOCHANGE              "(6295): The symbolic link of '%s' has not changed."
#define FIM_LINKCHECK_FINALIZE              "(6296): Links check finalized."
#define FIM_LINKCHECK_FILE                  "(6297): File '%s' was inside the unlinked directory '%s'. It will be notified."
#define FIM_WHODATA_REMOVE_FOLDEREVENT      "(6298): Removed folder event received for '%s'"
#define FIM_WHODATA_UNCONTROLLED_EVENT      "(6299): Uncontrolled whodata event on '%s'"
#define FIM_WHODATA_DIRECTORY_REMOVED       "(6300): Directory '%s' has been moved or removed."
#define FIM_WHODATA_UNCONTROLLED_REMOVE     "(6301): Uncontrolled removed folder event."
#define FIM_WHODATA_IGNORE_EVENT            "(6302): Ignoring removing event for '%s' directory."
#define FIM_WHODATA_DEVICE_LETTER           "(6303): Device '%s' associated with the mounting point '%s'"
#define FIM_WHODATA_DEVICE_PATH             "(6304): Find device '%s' in path '%s'"
#define FIM_WHODATA_DEVICE_REPLACE          "(6305): Replacing '%s' to '%s'"
#define FIM_WHODATA_PATH_NOPROCCESED        "(6306): The path could not be processed in Whodata mode. Error: %u"
#define FIM_WHODATA_CONVERT_PATH            "(6307): Convert '%s' to '%s' to process the whodata event."
#define FIM_WHODATA_FOLDER_REMOVED          "(6308): File '%s' was inside the removed directory '%s'. It will be notified."
#define FIM_WHODATA_IGNORE_FILEEVENT        "(6309): Ignoring remove event for file '%s' because it has already been reported."
#define FIM_CHECKSUM_MSG                    "(6310): Sending msg to the server: %s"
#define FIM_PATH_EXEED_MAX                  "(6311): The path exeed maximum permited (%d): '%s'"
#define FIM_DBSYNC_NO_ARGUMENT              "(6312): Data synchronization command '%s' with no argument."
#define FIM_DBSYNC_UNKNOWN_CMD              "(6313): Unknown data synchronization command: '%s'"
#define FIM_DBSYNC_INVALID_ARGUMENT         "(6314): Invalid data synchronization argument: '%s'"
#define FIM_DBSYNC_DEC_ID                   "(6315): Setting global ID back to lower message ID (%ld)"
#define FIM_DBSYNC_DROP_MESSAGE             "(6316): Dropping message with id (%ld) greater than global id (%ld)"
#define FIM_DBSYNC_SEND                     "(6317): Sending integrity control message: %s"
#define FIM_MONITORING_FILES_COUNT          "(6318): Number of indexed files %s scanning: %u"
#define FIM_CONFIGURATION_NOTFOUND          "(6319): No configuration found for (%s):'%s'"
#define FIM_PROCESS_PRIORITY                "(6320): Setting process priority to: '%d'"
#define FIM_SEND                            "(6321): Sending FIM event: %s"
#define FIM_AUDIT_RELOADED                  "(6322): Reloaded audit rule for monitoring directory: '%s'"
#define FIM_REALTIME_DIRECTORYCHANGES       "(6323): Unable to set 'ReadDirectoryChangesW' for directory: '%s'"
#define FIM_HASHES_FAIL                     "(6324): Couldn't generate hashes for '%s'"
#define FIM_EXTRACT_PERM_FAIL               "(6325): It was not possible to extract the permissions of '%s'. Error: %d"
#define FIM_RBTREE_DUPLICATE_INSERT         "(6326): Couldn't insert entry, duplicate path: '%s'"
#define FIM_HASH_INSERT_INODE_HASH          "(6327): Unable to add inode to db: '%s' => '%s'"
#define FIM_RBTREE_REPLACE                  "(6328): Unable to update file to db, key not found: '%s'"
#define FIM_REGISTRY_EVENT_FAIL             "(6329): Unable to save registry key: '%s'"
#define FIM_RUNNING_SCAN                    "(6330): The scan has been running during: %.3f sec (%.3f clock sec)"
#define FIM_GET_ATTRIBUTES                  "(6331): Couldn't get attributes for file: '%s'"
#define FIM_WARN_REALTIME_UNSUPPORTED       "(6332): The realtime monitoring request on unsupported system."
#define FIM_WARN_WHODATA_UNSUPPORTED        "(6333): The who-data monitoring request on unsupported system."
#define FIM_AUDIT_INVALID_AUID              "(6334): Audit: Invalid 'auid' value read. Check Audit configuration (PAM)."
#define FIM_ENTRIES_INFO                    "(6335): Fim entries: %d"
#define FIM_INODES_INFO                     "(6336): Fim inode entries: %d, path count: %d"
#define FIM_WHODATA_INVALID_UNKNOWN_UID     "(6337): The user ID could not be extracted from the event."
#define FIM_EMPTY_DIRECTORIES_CONFIG        "(6338): Empty directories tag found in the configuration."
#define FIM_DELETE_EVENT_PATH_NOCONF        "(6339): Delete event from path without configuration: '%s'"
#define FIM_DELETE_DB_TRY                   "(6340): Failed to delete FIM database '%s'- %dº try."
#define FIM_DELETE_DB                       "(6341): Failed to delete FIM database '%s'."

#endif /* DEBUG_MESSAGES_H */
