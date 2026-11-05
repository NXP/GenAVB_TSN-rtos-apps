/*
* Copyright 2023-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _FREERTOS_DSA_CONTROL_H_
#define _FREERTOS_DSA_CONTROL_H_

#include <stdint.h>
#include <stdbool.h>

#define DSA_CTRL_INTF_LPSPI

#define DSA_CTRL_REQUEST_CTRL_LEN	4  /* 32-bit control word */
#define DSA_CTRL_REQUEST_DATA_LEN	16
#define DSA_CTRL_REQUEST_LEN		(DSA_CTRL_REQUEST_CTRL_LEN + DSA_CTRL_REQUEST_DATA_LEN)
#define DSA_CTRL_RESPONSE_MAX_LEN	1024

enum netc_cmd {
	NETC_CMD_SYS_INFO_GET = 0x1,
	NETC_CMD_PORT_DSA_ADD,
	NETC_CMD_PORT_DSA_DEL,
	NETC_CMD_PORT_MTU_SET,
	NETC_CMD_PORT_MTU_GET,
	NETC_CMD_PORT_PHYLINK_MODE_SET,
	NETC_CMD_PORT_PHYLINK_STATUS_GET,
	NETC_CMD_PORT_ETHTOOL_STATS_GET,
	NETC_CMD_PORT_PVID_SET,
	NETC_CMD_PORT_LINK_SET,
	NETC_CMD_PORT_DROP_UNTAG_SET,
	NETC_CMD_FDB_ADD = 0x1000,
	NETC_CMD_FDB_DEL,
	NETC_CMD_FDB_DUMP,
	NETC_CMD_VLAN_ADD,
	NETC_CMD_VLAN_DEL,
	NETC_CMD_VLAN_DUMP,
	NETC_CMD_FORWARD_MASK_SET,
	NETC_CMD_PTP_SYNC_SET = 0x2000,
	NETC_CMD_TIMER_CUR_SET,
	NETC_CMD_TIMER_CUR_GET,
	NETC_CMD_TIMER_RATE_SET,
	NETC_CMD_TIMER_RATE_GET,
	NETC_CMD_TIMER_ADJTIME_SET,
	NETC_CMD_TIMER_ADJFINE_SET,
	NETC_CMD_TIMER_PPS_START,
	NETC_CMD_TIMER_PPS_STOP,
	NETC_CMD_TIMER_EXTTS_START,
	NETC_CMD_TIMER_EXTTS_STOP,
	NETC_CMD_QBV_SET_P1 = 0x3000,
	NETC_CMD_QBV_SET_P2,
	NETC_CMD_QBV_SET_GCL,
	NETC_CMD_QBU_SET,
	NETC_CMD_MM_SET,
	NETC_CMD_MM_GET,
	NETC_CMD_QCI_SF_SET,
	NETC_CMD_QCI_SG_SET_P1,
	NETC_CMD_QCI_SG_SET_P2,
	NETC_CMD_QCI_SG_SET_GCL,
	NETC_CMD_QCI_FM_SET,
	NETC_CMD_QCI_DEL,
	NETC_CMD_QCI_GET,
	NETC_CMD_FRER_SEQG_SET,
	NETC_CMD_FRER_SEQG_DEL,
	NETC_CMD_FRER_SEQR_SET,
	NETC_CMD_FRER_SEQR_DEL,
	NETC_CMD_STREAMID_SET,
	NETC_CMD_STREAMID_DEL,
	NETC_CMD_PRIORITY_MAP_SET,
	NETC_CMD_HSR_SET = 0x3600,
	NETC_CMD_MAX_NUM,
};

enum dsa_ctrl_access_type {
	DSA_CTRL_READ = 0,
	DSA_CTRL_WRITE = 1,
};

enum dsa_cpu_port_type {
	DSA_NETC_NONE = 0,
	DSA_NETC_SWITCH = 1,
	DSA_NETC_ENETC = 2,
};

enum dsa_ctrl_mm_verify_status {
	DSA_MM_VERIFY_STATUS_UNKNOWN,
	DSA_MM_VERIFY_STATUS_INITIAL,
	DSA_MM_VERIFY_STATUS_VERIFYING,
	DSA_MM_VERIFY_STATUS_SUCCEEDED,
	DSA_MM_VERIFY_STATUS_FAILED,
	DSA_MM_VERIFY_STATUS_DISABLED,
};

struct dsa_ctrl_request {
	uint32_t ctrl;
	uint8_t data[DSA_CTRL_REQUEST_DATA_LEN];
};

struct dsa_ctrl_response {
	uint8_t data[DSA_CTRL_RESPONSE_MAX_LEN];
};

/* No request message data for NETC_CMD_SYS_INFO_GET */

/* request message data for NETC_CMD_PORT_DSA_ADD */
struct dsa_ctrl_req_port_dsa_add {
	uint8_t cpu_port;	/* switch port 0, 1, 2 or 3 */
	uint8_t slave_port;	/* switch port 0, 1, 2 or 3 */
	uint8_t mac_addr[6];	/* MAC address of master Ethernet interface on MPU */
};

/* request message data for NETC_CMD_PORT_DSA_DEL */
struct dsa_ctrl_req_port_dsa_del {
	uint8_t slave_port;	/* switch port 0, 1, 2 or 3 */
	uint8_t reserved[3];
};

/* request message data for NETC_CMD_PORT_MTU_SET,
 * response message data for NETC_CMD_PORT_MTU_GET
 */
struct dsa_ctrl_port_mtu {
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	uint8_t reserved;
	uint16_t max_frame_size;
};

/* request message data for NETC_CMD_PORT_MTU_GET,
 *							NETC_CMD_PORT_PHYLINK_STATUS_GET,
 *							NETC_CMD_PORT_ETHTOOL_STATS_GET,
 *							NETC_CMD_PORT_DROP_UNTAG_SET
 */
struct dsa_ctrl_req_port {
	uint32_t port;	/* switch port 0, 1, 2 or 3 */
};

/* request message data for NETC_CMD_PORT_PHYLINK_MODE_SET,
 * response message data for NETC_CMD_PORT_PHYLINK_STATUS_GET
 */
struct dsa_ctrl_port_phylink {
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	bool link;	/* 0: down; 1: up */
	uint16_t speed;	/* 10: 10Mbps; 100: 100Mbps; 1000: 1000Mbps */
	bool duplex;	/* 0: half duplex; 1: full duplex */
	uint8_t reserved[3];
};

/* request message data for NETC_CMD_PORT_PVID_SET */
struct dsa_ctrl_req_port_pvid_set {
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	uint8_t reserved;
	uint16_t pvid;
};

/* request message data for NETC_CMD_PORT_LINK_SET */
struct dsa_ctrl_req_port_link_set {
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	bool link;	/* 0: down; 1: up */
	uint8_t reserved[2];
};

/* request message data for NETC_CMD_FDB_ADD */
struct dsa_ctrl_req_fdb_add {
	uint8_t mac_addr[6];
	uint16_t vid;
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	uint8_t reserved[3];
};

/* request message data for NETC_CMD_FDB_DEL */
struct dsa_ctrl_req_fdb_del {
	uint8_t mac_addr[6];
	uint16_t vid;
};

/* request message data for NETC_CMD_FDB_DUMP */
struct dsa_ctrl_req_fdb_dump {
	uint32_t resume_entry_id;	/* 0 for starting a search in FDB table */
};

/* request message data for NETC_CMD_VLAN_ADD, NETC_CMD_VLAN_DEL */
struct dsa_ctrl_req_vlan {
	uint16_t vid;
	uint8_t port;	/* switch port 0, 1, 2 or 3 */
	bool untagged;
};

/* request message data for NETC_CMD_VLAN_DUMP */
struct dsa_ctrl_req_vlan_dump {
	uint32_t resume_entry_id;	/* 0 for starting a search in VFT table */
};

/* request message data for NETC_CMD_FORWARD_MASK_SET */
struct dsa_ctrl_req_forward_mask_set {
	/* members to be defined */
};

/* request message data for NETC_CMD_PTP_SYNC_SET */
struct dsa_ctrl_req_ptp_sync_set {
	/* members to be defined */
};

struct dsa_ctrl_req_ptp_clock {
	union {
		uint64_t ns;
		int64_t offset;
		int64_t ppb;
	};
	uint8_t clock_id;
};

/* request message data for NETC_CMD_QBU_SET */
struct dsa_ctrl_req_qbu_set {
	uint8_t preemption_mask;
	uint8_t port;
	uint8_t reserved[14];
};

struct dsa_ctrl_mm {
	uint32_t verify_time;
	uint32_t add_frag_size;
	uint8_t verify_enabled;
	uint8_t verify_status;
	uint8_t tx_enabled;
	uint8_t pmac_enabled;
	uint8_t tx_active;
	uint8_t port;
	uint8_t reserved[2];
};

struct dsa_ctrl_req_frer_sg_set {
	uint16_t stream_handle;
	uint8_t encapsulation;
	uint8_t port;
};

struct dsa_ctrl_req_frer_sr_set {
	uint16_t stream_handle;
	uint16_t reset_timeout;
	uint8_t his_len;
	uint8_t encapsulation;
	uint8_t algorithm;
	bool rtag_pop_en;
	uint8_t port;
	uint8_t reserved[3];
};

struct dsa_ctrl_req_streamid_set {
	uint8_t mac_addr[6];
	uint16_t vid;
	uint16_t stream_handle;
	uint8_t type;
	uint8_t portmask;
};

struct dsa_ctrl_req_priority_map {
	uint8_t port;
	uint8_t map[8];
	uint8_t reserved[7];
};

struct dsa_ctrl_req_psfp_sf_set {
	uint32_t maxsdu;
	uint16_t stream_handle;
	int8_t priority_spec;
	uint8_t sg_enable;
	uint8_t fm_enable;
	uint8_t port;
	uint8_t reserved[6];
};

struct dsa_ctrl_req_psfp_sg_param1 {
	uint64_t base_time;
	uint32_t cycle_time;
	uint16_t gcl_len;
	uint16_t index;
};

struct dsa_ctrl_req_psfp_sg_param2 {
	uint32_t cycle_time_ext;
	int32_t prio;
	uint8_t reserved[8];
};

struct dsa_ctrl_req_psfp_sg_gcl {
	uint32_t interval;
	int32_t maxoctets;
	int32_t ipv;
	uint8_t gate_state;
	uint8_t reserved[3];
};

struct dsa_ctrl_req_psfp_fm_set {
	uint64_t rate;
	uint32_t burst;
	uint16_t index;
	uint8_t reserved[2];
};

struct dsa_ctrl_resp_psfp_sf_get {
	uint64_t pkts;
	uint64_t drops;
};

struct dsa_ctrl_req_qbv_gcl {
	uint32_t interval;
	uint16_t gate_mask;
	uint16_t operation;
};

struct dsa_ctrl_req_qbv_param1 {
	uint64_t base_time;
	uint32_t cycle_time;
	uint16_t gcl_len;
	uint8_t enabled;
	uint8_t port;
};

struct dsa_ctrl_req_qbv_param2 {
	uint32_t cycle_time_ext;
	uint8_t reserved[12];
};

/* request message data for NETC_CMD_HSR_SET */
struct dsa_ctrl_req_hsr_config {
	uint8_t hsr_enabled;
	uint8_t hsr_port_a;
	uint8_t hsr_port_b;
	uint8_t reserved[13];
};

/* response message for NETC_CMD_SYS_INFO_GET */
struct dsa_ctrl_resp_sys_info_get {
	uint16_t device_id;
	uint16_t vendor_id;
	uint8_t sw_version_major;
	uint8_t sw_version_minor;
	uint8_t sw_version_revsion;
	uint8_t dsa_cpu_port;	/* DSA_NETC_SWITCH or DSA_NETC_ENETC */
};

#define NETC_SW_ETHTOOL_STATS_MAX 120

/* response message for NETC_CMD_PORT_ETHTOOL_STATS_GET */
struct dsa_ctrl_resp_port_ethtool_stats {
	uint64_t values[NETC_SW_ETHTOOL_STATS_MAX];
};

/* response message for NETC_CMD_FDB_DUMP */
struct dsa_ctrl_resp_fdb_dump {
	uint8_t mac_addr[6];
	uint16_t vid;
	uint32_t port_map;	/* bit 0: switch port 0 etc. */
	bool dynamic;
	uint8_t reserved[3];
	uint32_t resume_entry_id;	/* non-zero means there are remaining entries, 0 means no more entries */
};

/* response message for NETC_CMD_VLAN_DUMP */
struct dsa_ctrl_resp_vlan_dump {
	uint16_t vid;
	bool untagged;
	uint8_t reserved;
	uint32_t port_map;	/* bit 0: switch port 0 etc. */
	uint32_t resume_entry_id;	/* non-zero means there are remaining entries, 0 means no more entries */
};

/* response message for NETC_CMD_QBU_GET */
struct dsa_ctrl_resp_qbu_get {
	/* members to be defined */
};

int dsa_ctrl_task_init(void);

#endif /* _FREERTOS_DSA_CONTROL_H_ */
