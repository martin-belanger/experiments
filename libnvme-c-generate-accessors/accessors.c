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

#include <stdlib.h>
#include <string.h>
#include "accessors.h"

#include "private.h";

/****************************************************************************
 * Accessors for: struct nvme_path
 */
void nvme_path_name_set(struct nvme_path *p, const char *name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;
}

const char * nvme_path_name_get(struct nvme_path *p) {
    return p->name;
}

void nvme_path_sysfs_dir_set(struct nvme_path *p, const char *sysfs_dir) {
    free(p->sysfs_dir);
    p->sysfs_dir = sysfs_dir ? strdup(sysfs_dir) : NULL;
}

const char * nvme_path_sysfs_dir_get(struct nvme_path *p) {
    return p->sysfs_dir;
}

void nvme_path_ana_state_set(struct nvme_path *p, const char *ana_state) {
    free(p->ana_state);
    p->ana_state = ana_state ? strdup(ana_state) : NULL;
}

const char * nvme_path_ana_state_get(struct nvme_path *p) {
    return p->ana_state;
}

void nvme_path_numa_nodes_set(struct nvme_path *p, const char *numa_nodes) {
    free(p->numa_nodes);
    p->numa_nodes = numa_nodes ? strdup(numa_nodes) : NULL;
}

const char * nvme_path_numa_nodes_get(struct nvme_path *p) {
    return p->numa_nodes;
}

void nvme_path_grpid_set(struct nvme_path *p, int grpid) {
    p->grpid = grpid;
}

int nvme_path_grpid_get(struct nvme_path *p) {
    return p->grpid;
}

void nvme_path_queue_depth_set(struct nvme_path *p, int queue_depth) {
    p->queue_depth = queue_depth;
}

int nvme_path_queue_depth_get(struct nvme_path *p) {
    return p->queue_depth;
}


/****************************************************************************
 * Accessors for: struct nvme_ns
 */
void nvme_ns_nsid_set(struct nvme_ns *p, __u32 nsid) {
    p->nsid = nsid;
}

__u32 nvme_ns_nsid_get(struct nvme_ns *p) {
    return p->nsid;
}

void nvme_ns_name_set(struct nvme_ns *p, const char *name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;
}

const char * nvme_ns_name_get(struct nvme_ns *p) {
    return p->name;
}

void nvme_ns_generic_name_set(struct nvme_ns *p, const char *generic_name) {
    free(p->generic_name);
    p->generic_name = generic_name ? strdup(generic_name) : NULL;
}

const char * nvme_ns_generic_name_get(struct nvme_ns *p) {
    return p->generic_name;
}

void nvme_ns_sysfs_dir_set(struct nvme_ns *p, const char *sysfs_dir) {
    free(p->sysfs_dir);
    p->sysfs_dir = sysfs_dir ? strdup(sysfs_dir) : NULL;
}

const char * nvme_ns_sysfs_dir_get(struct nvme_ns *p) {
    return p->sysfs_dir;
}

void nvme_ns_lba_shift_set(struct nvme_ns *p, int lba_shift) {
    p->lba_shift = lba_shift;
}

int nvme_ns_lba_shift_get(struct nvme_ns *p) {
    return p->lba_shift;
}

void nvme_ns_lba_size_set(struct nvme_ns *p, int lba_size) {
    p->lba_size = lba_size;
}

int nvme_ns_lba_size_get(struct nvme_ns *p) {
    return p->lba_size;
}

void nvme_ns_meta_size_set(struct nvme_ns *p, int meta_size) {
    p->meta_size = meta_size;
}

int nvme_ns_meta_size_get(struct nvme_ns *p) {
    return p->meta_size;
}

void nvme_ns_lba_count_set(struct nvme_ns *p, uint64_t lba_count) {
    p->lba_count = lba_count;
}

uint64_t nvme_ns_lba_count_get(struct nvme_ns *p) {
    return p->lba_count;
}

void nvme_ns_lba_util_set(struct nvme_ns *p, uint64_t lba_util) {
    p->lba_util = lba_util;
}

uint64_t nvme_ns_lba_util_get(struct nvme_ns *p) {
    return p->lba_util;
}


/****************************************************************************
 * Accessors for: struct nvme_ctrl
 */
void nvme_ctrl_name_set(struct nvme_ctrl *p, const char *name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;
}

const char * nvme_ctrl_name_get(struct nvme_ctrl *p) {
    return p->name;
}

void nvme_ctrl_sysfs_dir_set(struct nvme_ctrl *p, const char *sysfs_dir) {
    free(p->sysfs_dir);
    p->sysfs_dir = sysfs_dir ? strdup(sysfs_dir) : NULL;
}

const char * nvme_ctrl_sysfs_dir_get(struct nvme_ctrl *p) {
    return p->sysfs_dir;
}

void nvme_ctrl_address_set(struct nvme_ctrl *p, const char *address) {
    free(p->address);
    p->address = address ? strdup(address) : NULL;
}

const char * nvme_ctrl_address_get(struct nvme_ctrl *p) {
    return p->address;
}

void nvme_ctrl_firmware_set(struct nvme_ctrl *p, const char *firmware) {
    free(p->firmware);
    p->firmware = firmware ? strdup(firmware) : NULL;
}

const char * nvme_ctrl_firmware_get(struct nvme_ctrl *p) {
    return p->firmware;
}

void nvme_ctrl_model_set(struct nvme_ctrl *p, const char *model) {
    free(p->model);
    p->model = model ? strdup(model) : NULL;
}

const char * nvme_ctrl_model_get(struct nvme_ctrl *p) {
    return p->model;
}

void nvme_ctrl_state_set(struct nvme_ctrl *p, const char *state) {
    free(p->state);
    p->state = state ? strdup(state) : NULL;
}

const char * nvme_ctrl_state_get(struct nvme_ctrl *p) {
    return p->state;
}

void nvme_ctrl_numa_node_set(struct nvme_ctrl *p, const char *numa_node) {
    free(p->numa_node);
    p->numa_node = numa_node ? strdup(numa_node) : NULL;
}

const char * nvme_ctrl_numa_node_get(struct nvme_ctrl *p) {
    return p->numa_node;
}

void nvme_ctrl_queue_count_set(struct nvme_ctrl *p, const char *queue_count) {
    free(p->queue_count);
    p->queue_count = queue_count ? strdup(queue_count) : NULL;
}

const char * nvme_ctrl_queue_count_get(struct nvme_ctrl *p) {
    return p->queue_count;
}

void nvme_ctrl_serial_set(struct nvme_ctrl *p, const char *serial) {
    free(p->serial);
    p->serial = serial ? strdup(serial) : NULL;
}

const char * nvme_ctrl_serial_get(struct nvme_ctrl *p) {
    return p->serial;
}

void nvme_ctrl_sqsize_set(struct nvme_ctrl *p, const char *sqsize) {
    free(p->sqsize);
    p->sqsize = sqsize ? strdup(sqsize) : NULL;
}

const char * nvme_ctrl_sqsize_get(struct nvme_ctrl *p) {
    return p->sqsize;
}

void nvme_ctrl_transport_set(struct nvme_ctrl *p, const char *transport) {
    free(p->transport);
    p->transport = transport ? strdup(transport) : NULL;
}

const char * nvme_ctrl_transport_get(struct nvme_ctrl *p) {
    return p->transport;
}

void nvme_ctrl_subsysnqn_set(struct nvme_ctrl *p, const char *subsysnqn) {
    free(p->subsysnqn);
    p->subsysnqn = subsysnqn ? strdup(subsysnqn) : NULL;
}

const char * nvme_ctrl_subsysnqn_get(struct nvme_ctrl *p) {
    return p->subsysnqn;
}

void nvme_ctrl_traddr_set(struct nvme_ctrl *p, const char *traddr) {
    free(p->traddr);
    p->traddr = traddr ? strdup(traddr) : NULL;
}

const char * nvme_ctrl_traddr_get(struct nvme_ctrl *p) {
    return p->traddr;
}

void nvme_ctrl_trsvcid_set(struct nvme_ctrl *p, const char *trsvcid) {
    free(p->trsvcid);
    p->trsvcid = trsvcid ? strdup(trsvcid) : NULL;
}

const char * nvme_ctrl_trsvcid_get(struct nvme_ctrl *p) {
    return p->trsvcid;
}

void nvme_ctrl_dhchap_key_set(struct nvme_ctrl *p, const char *dhchap_key) {
    free(p->dhchap_key);
    p->dhchap_key = dhchap_key ? strdup(dhchap_key) : NULL;
}

const char * nvme_ctrl_dhchap_key_get(struct nvme_ctrl *p) {
    return p->dhchap_key;
}

void nvme_ctrl_dhchap_ctrl_key_set(struct nvme_ctrl *p, const char *dhchap_ctrl_key) {
    free(p->dhchap_ctrl_key);
    p->dhchap_ctrl_key = dhchap_ctrl_key ? strdup(dhchap_ctrl_key) : NULL;
}

const char * nvme_ctrl_dhchap_ctrl_key_get(struct nvme_ctrl *p) {
    return p->dhchap_ctrl_key;
}

void nvme_ctrl_keyring_set(struct nvme_ctrl *p, const char *keyring) {
    free(p->keyring);
    p->keyring = keyring ? strdup(keyring) : NULL;
}

const char * nvme_ctrl_keyring_get(struct nvme_ctrl *p) {
    return p->keyring;
}

void nvme_ctrl_tls_key_identity_set(struct nvme_ctrl *p, const char *tls_key_identity) {
    free(p->tls_key_identity);
    p->tls_key_identity = tls_key_identity ? strdup(tls_key_identity) : NULL;
}

const char * nvme_ctrl_tls_key_identity_get(struct nvme_ctrl *p) {
    return p->tls_key_identity;
}

void nvme_ctrl_tls_key_set(struct nvme_ctrl *p, const char *tls_key) {
    free(p->tls_key);
    p->tls_key = tls_key ? strdup(tls_key) : NULL;
}

const char * nvme_ctrl_tls_key_get(struct nvme_ctrl *p) {
    return p->tls_key;
}

void nvme_ctrl_cntrltype_set(struct nvme_ctrl *p, const char *cntrltype) {
    free(p->cntrltype);
    p->cntrltype = cntrltype ? strdup(cntrltype) : NULL;
}

const char * nvme_ctrl_cntrltype_get(struct nvme_ctrl *p) {
    return p->cntrltype;
}

void nvme_ctrl_cntlid_set(struct nvme_ctrl *p, const char *cntlid) {
    free(p->cntlid);
    p->cntlid = cntlid ? strdup(cntlid) : NULL;
}

const char * nvme_ctrl_cntlid_get(struct nvme_ctrl *p) {
    return p->cntlid;
}

void nvme_ctrl_dctype_set(struct nvme_ctrl *p, const char *dctype) {
    free(p->dctype);
    p->dctype = dctype ? strdup(dctype) : NULL;
}

const char * nvme_ctrl_dctype_get(struct nvme_ctrl *p) {
    return p->dctype;
}

void nvme_ctrl_phy_slot_set(struct nvme_ctrl *p, const char *phy_slot) {
    free(p->phy_slot);
    p->phy_slot = phy_slot ? strdup(phy_slot) : NULL;
}

const char * nvme_ctrl_phy_slot_get(struct nvme_ctrl *p) {
    return p->phy_slot;
}

void nvme_ctrl_discovery_ctrl_set(struct nvme_ctrl *p, bool discovery_ctrl) {
    p->discovery_ctrl = discovery_ctrl;
}

bool nvme_ctrl_discovery_ctrl_get(struct nvme_ctrl *p) {
    return p->discovery_ctrl;
}

void nvme_ctrl_unique_discovery_ctrl_set(struct nvme_ctrl *p, bool unique_discovery_ctrl) {
    p->unique_discovery_ctrl = unique_discovery_ctrl;
}

bool nvme_ctrl_unique_discovery_ctrl_get(struct nvme_ctrl *p) {
    return p->unique_discovery_ctrl;
}

void nvme_ctrl_discovered_set(struct nvme_ctrl *p, bool discovered) {
    p->discovered = discovered;
}

bool nvme_ctrl_discovered_get(struct nvme_ctrl *p) {
    return p->discovered;
}

void nvme_ctrl_persistent_set(struct nvme_ctrl *p, bool persistent) {
    p->persistent = persistent;
}

bool nvme_ctrl_persistent_get(struct nvme_ctrl *p) {
    return p->persistent;
}


/****************************************************************************
 * Accessors for: struct nvme_subsystem
 */
void nvme_subsystem_name_set(struct nvme_subsystem *p, const char *name) {
    free(p->name);
    p->name = name ? strdup(name) : NULL;
}

const char * nvme_subsystem_name_get(struct nvme_subsystem *p) {
    return p->name;
}

void nvme_subsystem_sysfs_dir_set(struct nvme_subsystem *p, const char *sysfs_dir) {
    free(p->sysfs_dir);
    p->sysfs_dir = sysfs_dir ? strdup(sysfs_dir) : NULL;
}

const char * nvme_subsystem_sysfs_dir_get(struct nvme_subsystem *p) {
    return p->sysfs_dir;
}

void nvme_subsystem_subsysnqn_set(struct nvme_subsystem *p, const char *subsysnqn) {
    free(p->subsysnqn);
    p->subsysnqn = subsysnqn ? strdup(subsysnqn) : NULL;
}

const char * nvme_subsystem_subsysnqn_get(struct nvme_subsystem *p) {
    return p->subsysnqn;
}

void nvme_subsystem_model_set(struct nvme_subsystem *p, const char *model) {
    free(p->model);
    p->model = model ? strdup(model) : NULL;
}

const char * nvme_subsystem_model_get(struct nvme_subsystem *p) {
    return p->model;
}

void nvme_subsystem_serial_set(struct nvme_subsystem *p, const char *serial) {
    free(p->serial);
    p->serial = serial ? strdup(serial) : NULL;
}

const char * nvme_subsystem_serial_get(struct nvme_subsystem *p) {
    return p->serial;
}

void nvme_subsystem_firmware_set(struct nvme_subsystem *p, const char *firmware) {
    free(p->firmware);
    p->firmware = firmware ? strdup(firmware) : NULL;
}

const char * nvme_subsystem_firmware_get(struct nvme_subsystem *p) {
    return p->firmware;
}

void nvme_subsystem_subsystype_set(struct nvme_subsystem *p, const char *subsystype) {
    free(p->subsystype);
    p->subsystype = subsystype ? strdup(subsystype) : NULL;
}

const char * nvme_subsystem_subsystype_get(struct nvme_subsystem *p) {
    return p->subsystype;
}

void nvme_subsystem_application_set(struct nvme_subsystem *p, const char *application) {
    free(p->application);
    p->application = application ? strdup(application) : NULL;
}

const char * nvme_subsystem_application_get(struct nvme_subsystem *p) {
    return p->application;
}

void nvme_subsystem_iopolicy_set(struct nvme_subsystem *p, const char *iopolicy) {
    free(p->iopolicy);
    p->iopolicy = iopolicy ? strdup(iopolicy) : NULL;
}

const char * nvme_subsystem_iopolicy_get(struct nvme_subsystem *p) {
    return p->iopolicy;
}


/****************************************************************************
 * Accessors for: struct nvme_host
 */
void nvme_host_hostnqn_set(struct nvme_host *p, const char *hostnqn) {
    free(p->hostnqn);
    p->hostnqn = hostnqn ? strdup(hostnqn) : NULL;
}

const char * nvme_host_hostnqn_get(struct nvme_host *p) {
    return p->hostnqn;
}

void nvme_host_hostid_set(struct nvme_host *p, const char *hostid) {
    free(p->hostid);
    p->hostid = hostid ? strdup(hostid) : NULL;
}

const char * nvme_host_hostid_get(struct nvme_host *p) {
    return p->hostid;
}

void nvme_host_dhchap_key_set(struct nvme_host *p, const char *dhchap_key) {
    free(p->dhchap_key);
    p->dhchap_key = dhchap_key ? strdup(dhchap_key) : NULL;
}

const char * nvme_host_dhchap_key_get(struct nvme_host *p) {
    return p->dhchap_key;
}

void nvme_host_hostsymname_set(struct nvme_host *p, const char *hostsymname) {
    free(p->hostsymname);
    p->hostsymname = hostsymname ? strdup(hostsymname) : NULL;
}

const char * nvme_host_hostsymname_get(struct nvme_host *p) {
    return p->hostsymname;
}

void nvme_host_pdc_enabled_set(struct nvme_host *p, bool pdc_enabled) {
    p->pdc_enabled = pdc_enabled;
}

bool nvme_host_pdc_enabled_get(struct nvme_host *p) {
    return p->pdc_enabled;
}

void nvme_host_pdc_enabled_valid_set(struct nvme_host *p, bool pdc_enabled_valid) {
    p->pdc_enabled_valid = pdc_enabled_valid;
}

bool nvme_host_pdc_enabled_valid_get(struct nvme_host *p) {
    return p->pdc_enabled_valid;
}


/****************************************************************************
 * Accessors for: struct nvme_fabric_options
 */
void nvme_fabric_options_cntlid_set(struct nvme_fabric_options *p, bool cntlid) {
    p->cntlid = cntlid;
}

bool nvme_fabric_options_cntlid_get(struct nvme_fabric_options *p) {
    return p->cntlid;
}

void nvme_fabric_options_concat_set(struct nvme_fabric_options *p, bool concat) {
    p->concat = concat;
}

bool nvme_fabric_options_concat_get(struct nvme_fabric_options *p) {
    return p->concat;
}

void nvme_fabric_options_ctrl_loss_tmo_set(struct nvme_fabric_options *p, bool ctrl_loss_tmo) {
    p->ctrl_loss_tmo = ctrl_loss_tmo;
}

bool nvme_fabric_options_ctrl_loss_tmo_get(struct nvme_fabric_options *p) {
    return p->ctrl_loss_tmo;
}

void nvme_fabric_options_data_digest_set(struct nvme_fabric_options *p, bool data_digest) {
    p->data_digest = data_digest;
}

bool nvme_fabric_options_data_digest_get(struct nvme_fabric_options *p) {
    return p->data_digest;
}

void nvme_fabric_options_dhchap_ctrl_secret_set(struct nvme_fabric_options *p, bool dhchap_ctrl_secret) {
    p->dhchap_ctrl_secret = dhchap_ctrl_secret;
}

bool nvme_fabric_options_dhchap_ctrl_secret_get(struct nvme_fabric_options *p) {
    return p->dhchap_ctrl_secret;
}

void nvme_fabric_options_dhchap_secret_set(struct nvme_fabric_options *p, bool dhchap_secret) {
    p->dhchap_secret = dhchap_secret;
}

bool nvme_fabric_options_dhchap_secret_get(struct nvme_fabric_options *p) {
    return p->dhchap_secret;
}

void nvme_fabric_options_disable_sqflow_set(struct nvme_fabric_options *p, bool disable_sqflow) {
    p->disable_sqflow = disable_sqflow;
}

bool nvme_fabric_options_disable_sqflow_get(struct nvme_fabric_options *p) {
    return p->disable_sqflow;
}

void nvme_fabric_options_discovery_set(struct nvme_fabric_options *p, bool discovery) {
    p->discovery = discovery;
}

bool nvme_fabric_options_discovery_get(struct nvme_fabric_options *p) {
    return p->discovery;
}

void nvme_fabric_options_duplicate_connect_set(struct nvme_fabric_options *p, bool duplicate_connect) {
    p->duplicate_connect = duplicate_connect;
}

bool nvme_fabric_options_duplicate_connect_get(struct nvme_fabric_options *p) {
    return p->duplicate_connect;
}

void nvme_fabric_options_fast_io_fail_tmo_set(struct nvme_fabric_options *p, bool fast_io_fail_tmo) {
    p->fast_io_fail_tmo = fast_io_fail_tmo;
}

bool nvme_fabric_options_fast_io_fail_tmo_get(struct nvme_fabric_options *p) {
    return p->fast_io_fail_tmo;
}

void nvme_fabric_options_hdr_digest_set(struct nvme_fabric_options *p, bool hdr_digest) {
    p->hdr_digest = hdr_digest;
}

bool nvme_fabric_options_hdr_digest_get(struct nvme_fabric_options *p) {
    return p->hdr_digest;
}

void nvme_fabric_options_host_iface_set(struct nvme_fabric_options *p, bool host_iface) {
    p->host_iface = host_iface;
}

bool nvme_fabric_options_host_iface_get(struct nvme_fabric_options *p) {
    return p->host_iface;
}

void nvme_fabric_options_host_traddr_set(struct nvme_fabric_options *p, bool host_traddr) {
    p->host_traddr = host_traddr;
}

bool nvme_fabric_options_host_traddr_get(struct nvme_fabric_options *p) {
    return p->host_traddr;
}

void nvme_fabric_options_hostid_set(struct nvme_fabric_options *p, bool hostid) {
    p->hostid = hostid;
}

bool nvme_fabric_options_hostid_get(struct nvme_fabric_options *p) {
    return p->hostid;
}

void nvme_fabric_options_hostnqn_set(struct nvme_fabric_options *p, bool hostnqn) {
    p->hostnqn = hostnqn;
}

bool nvme_fabric_options_hostnqn_get(struct nvme_fabric_options *p) {
    return p->hostnqn;
}

void nvme_fabric_options_instance_set(struct nvme_fabric_options *p, bool instance) {
    p->instance = instance;
}

bool nvme_fabric_options_instance_get(struct nvme_fabric_options *p) {
    return p->instance;
}

void nvme_fabric_options_keep_alive_tmo_set(struct nvme_fabric_options *p, bool keep_alive_tmo) {
    p->keep_alive_tmo = keep_alive_tmo;
}

bool nvme_fabric_options_keep_alive_tmo_get(struct nvme_fabric_options *p) {
    return p->keep_alive_tmo;
}

void nvme_fabric_options_keyring_set(struct nvme_fabric_options *p, bool keyring) {
    p->keyring = keyring;
}

bool nvme_fabric_options_keyring_get(struct nvme_fabric_options *p) {
    return p->keyring;
}

void nvme_fabric_options_nqn_set(struct nvme_fabric_options *p, bool nqn) {
    p->nqn = nqn;
}

bool nvme_fabric_options_nqn_get(struct nvme_fabric_options *p) {
    return p->nqn;
}

void nvme_fabric_options_nr_io_queues_set(struct nvme_fabric_options *p, bool nr_io_queues) {
    p->nr_io_queues = nr_io_queues;
}

bool nvme_fabric_options_nr_io_queues_get(struct nvme_fabric_options *p) {
    return p->nr_io_queues;
}

void nvme_fabric_options_nr_poll_queues_set(struct nvme_fabric_options *p, bool nr_poll_queues) {
    p->nr_poll_queues = nr_poll_queues;
}

bool nvme_fabric_options_nr_poll_queues_get(struct nvme_fabric_options *p) {
    return p->nr_poll_queues;
}

void nvme_fabric_options_nr_write_queues_set(struct nvme_fabric_options *p, bool nr_write_queues) {
    p->nr_write_queues = nr_write_queues;
}

bool nvme_fabric_options_nr_write_queues_get(struct nvme_fabric_options *p) {
    return p->nr_write_queues;
}

void nvme_fabric_options_queue_size_set(struct nvme_fabric_options *p, bool queue_size) {
    p->queue_size = queue_size;
}

bool nvme_fabric_options_queue_size_get(struct nvme_fabric_options *p) {
    return p->queue_size;
}

void nvme_fabric_options_reconnect_delay_set(struct nvme_fabric_options *p, bool reconnect_delay) {
    p->reconnect_delay = reconnect_delay;
}

bool nvme_fabric_options_reconnect_delay_get(struct nvme_fabric_options *p) {
    return p->reconnect_delay;
}

void nvme_fabric_options_tls_set(struct nvme_fabric_options *p, bool tls) {
    p->tls = tls;
}

bool nvme_fabric_options_tls_get(struct nvme_fabric_options *p) {
    return p->tls;
}

void nvme_fabric_options_tls_key_set(struct nvme_fabric_options *p, bool tls_key) {
    p->tls_key = tls_key;
}

bool nvme_fabric_options_tls_key_get(struct nvme_fabric_options *p) {
    return p->tls_key;
}

void nvme_fabric_options_tos_set(struct nvme_fabric_options *p, bool tos) {
    p->tos = tos;
}

bool nvme_fabric_options_tos_get(struct nvme_fabric_options *p) {
    return p->tos;
}

void nvme_fabric_options_traddr_set(struct nvme_fabric_options *p, bool traddr) {
    p->traddr = traddr;
}

bool nvme_fabric_options_traddr_get(struct nvme_fabric_options *p) {
    return p->traddr;
}

void nvme_fabric_options_transport_set(struct nvme_fabric_options *p, bool transport) {
    p->transport = transport;
}

bool nvme_fabric_options_transport_get(struct nvme_fabric_options *p) {
    return p->transport;
}

void nvme_fabric_options_trsvcid_set(struct nvme_fabric_options *p, bool trsvcid) {
    p->trsvcid = trsvcid;
}

bool nvme_fabric_options_trsvcid_get(struct nvme_fabric_options *p) {
    return p->trsvcid;
}

