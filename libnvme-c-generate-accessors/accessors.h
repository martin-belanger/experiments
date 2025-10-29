// SPDX-License-Identifier: LGPL-2.1-or-later
/**
 * This file is part of libnvme.
 *
 *   ____                           _           _    ____          _
 *  / ___| ___ _ __   ___ _ __ __ _| |_ ___  __| |  / ___|___   __| | ___
 * | |  _ / _ \ '_ \ / _ \ '__/ _` | __/ _ \/ _` | | |   / _ \ / _` |/ _ \
 * | |_| |  __/ | | |  __/ | | (_| | ||  __/ (_| | | |__| (_) | (_| |  __/
 *  \____|\___|_| |_|\___|_|  \__,_|\__\___|\__,_|  \____\___/ \__,_|\___|
 *
 * Auto-generated struct member accessors (setter/getter)
 */

#ifndef _ACCESSORS_H_
#define _ACCESSORS_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <linux/types.h> /* __u32, __u64, etc. */

/* Forward declarations. These are internal (opaque) structs. */
struct nvme_path;
struct nvme_ns;
struct nvme_ctrl;
struct nvme_subsystem;
struct nvme_host;
struct nvme_fabric_options;

/****************************************************************************
 * Accessors for: struct nvme_path
 */
void nvme_path_name_set(struct nvme_path *p, const char *name);
const char * nvme_path_name_get(struct nvme_path *p);

void nvme_path_sysfs_dir_set(struct nvme_path *p, const char *sysfs_dir);
const char * nvme_path_sysfs_dir_get(struct nvme_path *p);

void nvme_path_ana_state_set(struct nvme_path *p, const char *ana_state);
const char * nvme_path_ana_state_get(struct nvme_path *p);

void nvme_path_numa_nodes_set(struct nvme_path *p, const char *numa_nodes);
const char * nvme_path_numa_nodes_get(struct nvme_path *p);

void nvme_path_grpid_set(struct nvme_path *p, int grpid);
int nvme_path_grpid_get(struct nvme_path *p);

void nvme_path_queue_depth_set(struct nvme_path *p, int queue_depth);
int nvme_path_queue_depth_get(struct nvme_path *p);


/****************************************************************************
 * Accessors for: struct nvme_ns
 */
void nvme_ns_nsid_set(struct nvme_ns *p, __u32 nsid);
__u32 nvme_ns_nsid_get(struct nvme_ns *p);

void nvme_ns_name_set(struct nvme_ns *p, const char *name);
const char * nvme_ns_name_get(struct nvme_ns *p);

void nvme_ns_generic_name_set(struct nvme_ns *p, const char *generic_name);
const char * nvme_ns_generic_name_get(struct nvme_ns *p);

void nvme_ns_sysfs_dir_set(struct nvme_ns *p, const char *sysfs_dir);
const char * nvme_ns_sysfs_dir_get(struct nvme_ns *p);

void nvme_ns_lba_shift_set(struct nvme_ns *p, int lba_shift);
int nvme_ns_lba_shift_get(struct nvme_ns *p);

void nvme_ns_lba_size_set(struct nvme_ns *p, int lba_size);
int nvme_ns_lba_size_get(struct nvme_ns *p);

void nvme_ns_meta_size_set(struct nvme_ns *p, int meta_size);
int nvme_ns_meta_size_get(struct nvme_ns *p);

void nvme_ns_lba_count_set(struct nvme_ns *p, uint64_t lba_count);
uint64_t nvme_ns_lba_count_get(struct nvme_ns *p);

void nvme_ns_lba_util_set(struct nvme_ns *p, uint64_t lba_util);
uint64_t nvme_ns_lba_util_get(struct nvme_ns *p);


/****************************************************************************
 * Accessors for: struct nvme_ctrl
 */
void nvme_ctrl_name_set(struct nvme_ctrl *p, const char *name);
const char * nvme_ctrl_name_get(struct nvme_ctrl *p);

void nvme_ctrl_sysfs_dir_set(struct nvme_ctrl *p, const char *sysfs_dir);
const char * nvme_ctrl_sysfs_dir_get(struct nvme_ctrl *p);

void nvme_ctrl_address_set(struct nvme_ctrl *p, const char *address);
const char * nvme_ctrl_address_get(struct nvme_ctrl *p);

void nvme_ctrl_firmware_set(struct nvme_ctrl *p, const char *firmware);
const char * nvme_ctrl_firmware_get(struct nvme_ctrl *p);

void nvme_ctrl_model_set(struct nvme_ctrl *p, const char *model);
const char * nvme_ctrl_model_get(struct nvme_ctrl *p);

void nvme_ctrl_state_set(struct nvme_ctrl *p, const char *state);
const char * nvme_ctrl_state_get(struct nvme_ctrl *p);

void nvme_ctrl_numa_node_set(struct nvme_ctrl *p, const char *numa_node);
const char * nvme_ctrl_numa_node_get(struct nvme_ctrl *p);

void nvme_ctrl_queue_count_set(struct nvme_ctrl *p, const char *queue_count);
const char * nvme_ctrl_queue_count_get(struct nvme_ctrl *p);

void nvme_ctrl_serial_set(struct nvme_ctrl *p, const char *serial);
const char * nvme_ctrl_serial_get(struct nvme_ctrl *p);

void nvme_ctrl_sqsize_set(struct nvme_ctrl *p, const char *sqsize);
const char * nvme_ctrl_sqsize_get(struct nvme_ctrl *p);

void nvme_ctrl_transport_set(struct nvme_ctrl *p, const char *transport);
const char * nvme_ctrl_transport_get(struct nvme_ctrl *p);

void nvme_ctrl_subsysnqn_set(struct nvme_ctrl *p, const char *subsysnqn);
const char * nvme_ctrl_subsysnqn_get(struct nvme_ctrl *p);

void nvme_ctrl_traddr_set(struct nvme_ctrl *p, const char *traddr);
const char * nvme_ctrl_traddr_get(struct nvme_ctrl *p);

void nvme_ctrl_trsvcid_set(struct nvme_ctrl *p, const char *trsvcid);
const char * nvme_ctrl_trsvcid_get(struct nvme_ctrl *p);

void nvme_ctrl_dhchap_key_set(struct nvme_ctrl *p, const char *dhchap_key);
const char * nvme_ctrl_dhchap_key_get(struct nvme_ctrl *p);

void nvme_ctrl_dhchap_ctrl_key_set(struct nvme_ctrl *p, const char *dhchap_ctrl_key);
const char * nvme_ctrl_dhchap_ctrl_key_get(struct nvme_ctrl *p);

void nvme_ctrl_keyring_set(struct nvme_ctrl *p, const char *keyring);
const char * nvme_ctrl_keyring_get(struct nvme_ctrl *p);

void nvme_ctrl_tls_key_identity_set(struct nvme_ctrl *p, const char *tls_key_identity);
const char * nvme_ctrl_tls_key_identity_get(struct nvme_ctrl *p);

void nvme_ctrl_tls_key_set(struct nvme_ctrl *p, const char *tls_key);
const char * nvme_ctrl_tls_key_get(struct nvme_ctrl *p);

void nvme_ctrl_cntrltype_set(struct nvme_ctrl *p, const char *cntrltype);
const char * nvme_ctrl_cntrltype_get(struct nvme_ctrl *p);

void nvme_ctrl_cntlid_set(struct nvme_ctrl *p, const char *cntlid);
const char * nvme_ctrl_cntlid_get(struct nvme_ctrl *p);

void nvme_ctrl_dctype_set(struct nvme_ctrl *p, const char *dctype);
const char * nvme_ctrl_dctype_get(struct nvme_ctrl *p);

void nvme_ctrl_phy_slot_set(struct nvme_ctrl *p, const char *phy_slot);
const char * nvme_ctrl_phy_slot_get(struct nvme_ctrl *p);

void nvme_ctrl_discovery_ctrl_set(struct nvme_ctrl *p, bool discovery_ctrl);
bool nvme_ctrl_discovery_ctrl_get(struct nvme_ctrl *p);

void nvme_ctrl_unique_discovery_ctrl_set(struct nvme_ctrl *p, bool unique_discovery_ctrl);
bool nvme_ctrl_unique_discovery_ctrl_get(struct nvme_ctrl *p);

void nvme_ctrl_discovered_set(struct nvme_ctrl *p, bool discovered);
bool nvme_ctrl_discovered_get(struct nvme_ctrl *p);

void nvme_ctrl_persistent_set(struct nvme_ctrl *p, bool persistent);
bool nvme_ctrl_persistent_get(struct nvme_ctrl *p);


/****************************************************************************
 * Accessors for: struct nvme_subsystem
 */
void nvme_subsystem_name_set(struct nvme_subsystem *p, const char *name);
const char * nvme_subsystem_name_get(struct nvme_subsystem *p);

void nvme_subsystem_sysfs_dir_set(struct nvme_subsystem *p, const char *sysfs_dir);
const char * nvme_subsystem_sysfs_dir_get(struct nvme_subsystem *p);

void nvme_subsystem_subsysnqn_set(struct nvme_subsystem *p, const char *subsysnqn);
const char * nvme_subsystem_subsysnqn_get(struct nvme_subsystem *p);

void nvme_subsystem_model_set(struct nvme_subsystem *p, const char *model);
const char * nvme_subsystem_model_get(struct nvme_subsystem *p);

void nvme_subsystem_serial_set(struct nvme_subsystem *p, const char *serial);
const char * nvme_subsystem_serial_get(struct nvme_subsystem *p);

void nvme_subsystem_firmware_set(struct nvme_subsystem *p, const char *firmware);
const char * nvme_subsystem_firmware_get(struct nvme_subsystem *p);

void nvme_subsystem_subsystype_set(struct nvme_subsystem *p, const char *subsystype);
const char * nvme_subsystem_subsystype_get(struct nvme_subsystem *p);

void nvme_subsystem_application_set(struct nvme_subsystem *p, const char *application);
const char * nvme_subsystem_application_get(struct nvme_subsystem *p);

void nvme_subsystem_iopolicy_set(struct nvme_subsystem *p, const char *iopolicy);
const char * nvme_subsystem_iopolicy_get(struct nvme_subsystem *p);


/****************************************************************************
 * Accessors for: struct nvme_host
 */
void nvme_host_hostnqn_set(struct nvme_host *p, const char *hostnqn);
const char * nvme_host_hostnqn_get(struct nvme_host *p);

void nvme_host_hostid_set(struct nvme_host *p, const char *hostid);
const char * nvme_host_hostid_get(struct nvme_host *p);

void nvme_host_dhchap_key_set(struct nvme_host *p, const char *dhchap_key);
const char * nvme_host_dhchap_key_get(struct nvme_host *p);

void nvme_host_hostsymname_set(struct nvme_host *p, const char *hostsymname);
const char * nvme_host_hostsymname_get(struct nvme_host *p);

void nvme_host_pdc_enabled_set(struct nvme_host *p, bool pdc_enabled);
bool nvme_host_pdc_enabled_get(struct nvme_host *p);

void nvme_host_pdc_enabled_valid_set(struct nvme_host *p, bool pdc_enabled_valid);
bool nvme_host_pdc_enabled_valid_get(struct nvme_host *p);


/****************************************************************************
 * Accessors for: struct nvme_fabric_options
 */
void nvme_fabric_options_cntlid_set(struct nvme_fabric_options *p, bool cntlid);
bool nvme_fabric_options_cntlid_get(struct nvme_fabric_options *p);

void nvme_fabric_options_concat_set(struct nvme_fabric_options *p, bool concat);
bool nvme_fabric_options_concat_get(struct nvme_fabric_options *p);

void nvme_fabric_options_ctrl_loss_tmo_set(struct nvme_fabric_options *p, bool ctrl_loss_tmo);
bool nvme_fabric_options_ctrl_loss_tmo_get(struct nvme_fabric_options *p);

void nvme_fabric_options_data_digest_set(struct nvme_fabric_options *p, bool data_digest);
bool nvme_fabric_options_data_digest_get(struct nvme_fabric_options *p);

void nvme_fabric_options_dhchap_ctrl_secret_set(struct nvme_fabric_options *p, bool dhchap_ctrl_secret);
bool nvme_fabric_options_dhchap_ctrl_secret_get(struct nvme_fabric_options *p);

void nvme_fabric_options_dhchap_secret_set(struct nvme_fabric_options *p, bool dhchap_secret);
bool nvme_fabric_options_dhchap_secret_get(struct nvme_fabric_options *p);

void nvme_fabric_options_disable_sqflow_set(struct nvme_fabric_options *p, bool disable_sqflow);
bool nvme_fabric_options_disable_sqflow_get(struct nvme_fabric_options *p);

void nvme_fabric_options_discovery_set(struct nvme_fabric_options *p, bool discovery);
bool nvme_fabric_options_discovery_get(struct nvme_fabric_options *p);

void nvme_fabric_options_duplicate_connect_set(struct nvme_fabric_options *p, bool duplicate_connect);
bool nvme_fabric_options_duplicate_connect_get(struct nvme_fabric_options *p);

void nvme_fabric_options_fast_io_fail_tmo_set(struct nvme_fabric_options *p, bool fast_io_fail_tmo);
bool nvme_fabric_options_fast_io_fail_tmo_get(struct nvme_fabric_options *p);

void nvme_fabric_options_hdr_digest_set(struct nvme_fabric_options *p, bool hdr_digest);
bool nvme_fabric_options_hdr_digest_get(struct nvme_fabric_options *p);

void nvme_fabric_options_host_iface_set(struct nvme_fabric_options *p, bool host_iface);
bool nvme_fabric_options_host_iface_get(struct nvme_fabric_options *p);

void nvme_fabric_options_host_traddr_set(struct nvme_fabric_options *p, bool host_traddr);
bool nvme_fabric_options_host_traddr_get(struct nvme_fabric_options *p);

void nvme_fabric_options_hostid_set(struct nvme_fabric_options *p, bool hostid);
bool nvme_fabric_options_hostid_get(struct nvme_fabric_options *p);

void nvme_fabric_options_hostnqn_set(struct nvme_fabric_options *p, bool hostnqn);
bool nvme_fabric_options_hostnqn_get(struct nvme_fabric_options *p);

void nvme_fabric_options_instance_set(struct nvme_fabric_options *p, bool instance);
bool nvme_fabric_options_instance_get(struct nvme_fabric_options *p);

void nvme_fabric_options_keep_alive_tmo_set(struct nvme_fabric_options *p, bool keep_alive_tmo);
bool nvme_fabric_options_keep_alive_tmo_get(struct nvme_fabric_options *p);

void nvme_fabric_options_keyring_set(struct nvme_fabric_options *p, bool keyring);
bool nvme_fabric_options_keyring_get(struct nvme_fabric_options *p);

void nvme_fabric_options_nqn_set(struct nvme_fabric_options *p, bool nqn);
bool nvme_fabric_options_nqn_get(struct nvme_fabric_options *p);

void nvme_fabric_options_nr_io_queues_set(struct nvme_fabric_options *p, bool nr_io_queues);
bool nvme_fabric_options_nr_io_queues_get(struct nvme_fabric_options *p);

void nvme_fabric_options_nr_poll_queues_set(struct nvme_fabric_options *p, bool nr_poll_queues);
bool nvme_fabric_options_nr_poll_queues_get(struct nvme_fabric_options *p);

void nvme_fabric_options_nr_write_queues_set(struct nvme_fabric_options *p, bool nr_write_queues);
bool nvme_fabric_options_nr_write_queues_get(struct nvme_fabric_options *p);

void nvme_fabric_options_queue_size_set(struct nvme_fabric_options *p, bool queue_size);
bool nvme_fabric_options_queue_size_get(struct nvme_fabric_options *p);

void nvme_fabric_options_reconnect_delay_set(struct nvme_fabric_options *p, bool reconnect_delay);
bool nvme_fabric_options_reconnect_delay_get(struct nvme_fabric_options *p);

void nvme_fabric_options_tls_set(struct nvme_fabric_options *p, bool tls);
bool nvme_fabric_options_tls_get(struct nvme_fabric_options *p);

void nvme_fabric_options_tls_key_set(struct nvme_fabric_options *p, bool tls_key);
bool nvme_fabric_options_tls_key_get(struct nvme_fabric_options *p);

void nvme_fabric_options_tos_set(struct nvme_fabric_options *p, bool tos);
bool nvme_fabric_options_tos_get(struct nvme_fabric_options *p);

void nvme_fabric_options_traddr_set(struct nvme_fabric_options *p, bool traddr);
bool nvme_fabric_options_traddr_get(struct nvme_fabric_options *p);

void nvme_fabric_options_transport_set(struct nvme_fabric_options *p, bool transport);
bool nvme_fabric_options_transport_get(struct nvme_fabric_options *p);

void nvme_fabric_options_trsvcid_set(struct nvme_fabric_options *p, bool trsvcid);
bool nvme_fabric_options_trsvcid_get(struct nvme_fabric_options *p);

#endif /* _ACCESSORS_H_ */
