/*
* Copyright 2023-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "FreeRTOS.h"
#include "task.h"

#include "genavb/error.h"
#include "genavb/dsa.h"
#include "genavb/ether.h"
#include "genavb/fdb.h"
#include "genavb/port.h"
#include "genavb/stats.h"
#include "genavb/vlan.h"

#include "genavb/stream_identification.h"
#include "genavb/scheduled_traffic.h"
#include "genavb/frame_preemption.h"
#include "genavb/frer.h"
#include "genavb/psfp.h"
#include "genavb/qos.h"
#include "genavb/hsr.h"

#include "lib_virtual_switch.h"
#include "dsa_virtual_switch.h"

#include "avb_tsn/common/log.h"
#include "avb_tsn/common/genavb.h"
#include "dsa_control.h"
#include "dsa_control_lpspi.h"
#include "dsa_control_utils.h"


#define DSA_CTRL_TASK_NAME "DSA Ctrl"
#define DSA_CTRL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 512)
#define DSA_CTRL_TASK_PRIORITY (configMAX_PRIORITIES - 9)

#define DSA_CTRL_CLK_ID_GET(clk_id) ((genavb_clock_id_t)(GENAVB_CLOCK_BR_0_0 + clk_id))

#define DSA_CTRL_STREAMID_MAX 23
#define DSA_CTRL_STREAM_INGRESS_PORTS	(CONFIG_APP_BR_NUM_PORTS - 1 - 1)

static struct dsa_control_ctx {
	TaskHandle_t task_handle;
} dsa_ctrl_ctx;

enum dsa_ctrl_req_state {
	DSA_CTRL_REQ_IDLE = 0,
	DSA_CTRL_REQ_START,
	DSA_CTRL_REQ_GET_PARAM
};

struct dsa_ctrl_qbv_req {
	enum dsa_ctrl_req_state state;
	struct genavb_st_config st_conf;
	int gcl_num;
	int port;
};

struct dsa_ctrl_psfp_sg_req {
	enum dsa_ctrl_req_state state;
	struct genavb_stream_gate_instance gate;
	int gcl_num;
};

#define ST_CONF_DEFAULT_INIT \
{\
	.enable = 0,\
	.base_time = 0,\
	.cycle_time_p = 0,\
	.cycle_time_q = 0,\
	.cycle_time_ext = 0,\
	.list_length = 0,\
	.control_list = NULL\
}

#define GATE_CONF_DEFAULT_INIT \
{\
	.stream_gate_instance_id = 0,\
	.gate_enable = false,\
	.admin_gate_state = 0,\
	.admin_ipv = 0,\
	.cycle_time_p = 0,\
	.cycle_time_q = 0,\
	.cycle_time_extension = 0,\
	.base_time = 0,\
	.gate_closed_due_to_invalid_rx_enable = false,\
	.gate_closed_due_to_invalid_rx = false,\
	.gate_closed_due_to_octets_exceeded_enable = false,\
	.gate_closed_due_to_octets_exceeded = false,\
	.list_length = 0,\
	.control_list = NULL\
}

#define QBV_CONF_DEFAULT_INIT \
{\
	.state = DSA_CTRL_REQ_IDLE,\
	.st_conf = ST_CONF_DEFAULT_INIT,\
	.gcl_num = 0,\
	.port = 0\
}

#define SG_CONF_DEFAULT_INIT \
{\
	.state = DSA_CTRL_REQ_IDLE,\
	.gate = GATE_CONF_DEFAULT_INIT,\
	.gcl_num = 0\
}

#define ADMIN_STATUS_DEFAULT_INIT \
{\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS,\
    GENAVB_FP_ADMIN_STATUS_EXPRESS\
}

#define GENAVB_FP_CONFIG_802_1Q_DEFAULT_INIT \
{\
	.u.cfg_802_1Q = {\
		.admin_status = ADMIN_STATUS_DEFAULT_INIT,\
		.hold_advance = 0,\
		.release_advance = 0,\
		.preemption_active = 0,\
		.hold_request = GENAVB_FP_HOLD_REQUEST_HOLD\
	}\
}

#define GENAVB_FP_CONFIG_802_3_DEFAULT_INIT \
{\
	.u.cfg_802_3 = {\
		.support = 0,\
		.status_verify = 0,\
		.enable_tx = 0,\
		.verify_disable_tx = 0,\
		.status_tx = 0,\
		.verify_time = 0,\
		.add_frag_size = 0\
	}\
}

static struct dsa_ctrl_request request;
static struct dsa_ctrl_response response;
static struct dsa_ctrl_qbv_req qbvconf = QBV_CONF_DEFAULT_INIT;
static struct dsa_ctrl_psfp_sg_req sgconf = SG_CONF_DEFAULT_INIT;

/* command handler for NETC_CMD_SYS_INFO_GET */
static void dsa_ctrl_sys_info_get(struct dsa_ctrl_resp_sys_info_get *response)
{
	dsa_system_info_get((struct dsa_system_info *)response);
}

/* command handler for NETC_CMD_PORT_DSA_ADD */
static int dsa_ctrl_port_dsa_add(struct dsa_ctrl_req_port_dsa_add *request)
{
	unsigned int logica_cpu_port;
	unsigned int logica_slave_port;
	int rc;

	logica_cpu_port = br_port_list[request->cpu_port];
	if (logica_cpu_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	logica_slave_port = br_port_list[request->slave_port];
	if (logica_slave_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	dsa_stack_update_host_mac(request->mac_addr);

	rc = genavb_port_dsa_add(logica_cpu_port, request->mac_addr, logica_slave_port);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_DSA_DEL */
static int dsa_ctrl_port_dsa_del(struct dsa_ctrl_req_port_dsa_del *request)
{
	unsigned int logica_slave_port;
	int rc;

	logica_slave_port = br_port_list[request->slave_port];
	if (logica_slave_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	rc = genavb_port_dsa_delete(logica_slave_port);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_MTU_SET */
static int dsa_ctrl_port_mtu_set(struct dsa_ctrl_port_mtu *request)
{
	unsigned int logical_port;
	int rc;

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	rc = genavb_port_set_max_frame_size(logical_port, request->max_frame_size);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_MTU_GET */
static int dsa_ctrl_port_mtu_get(struct dsa_ctrl_port_mtu *request, struct dsa_ctrl_port_mtu *response)
{
	return 0;
}

/* command handler for NETC_CMD_PORT_PHYLINK_MODE_SET */
static int dsa_ctrl_port_phylink_mode_set(struct dsa_ctrl_port_phylink *request)
{
	return 0;
}

/* command handler for NETC_CMD_PORT_PHYLINK_STATUS_GET */
static int dsa_ctrl_port_phylink_status_get(struct dsa_ctrl_req_port *request, struct dsa_ctrl_port_phylink *response)
{
	unsigned int logical_port;
	uint64_t rate;
	int rc;

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	rc = genavb_port_status_get(logical_port, &response->link, &response->duplex, &rate);
	if (rc < 0)
		goto err;

	switch (rate) {
	default:
	case 10000000:
		response->speed = 10;
		break;
	case 100000000:
		response->speed = 100;
		break;
	case 1000000000:
		response->speed = 1000;
		break;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_ETHTOOL_STATS_GET */
static int dsa_ctrl_port_ethtool_stats_get(struct dsa_ctrl_req_port *request, struct dsa_ctrl_resp_port_ethtool_stats *response)
{
	unsigned int logical_port;
	int stats_num;
	int rc;

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	rc = genavb_port_stats_get_number(logical_port);
	if (rc < 0)
		goto err;

	stats_num = rc;
	if (stats_num != NETC_SW_ETHTOOL_STATS_MAX) {
		log_err("dsa_ctrl_port_ethtool_stats_get(): stats number (%d), expected (%d)\n", stats_num, NETC_SW_ETHTOOL_STATS_MAX);
		rc = -GENAVB_ERR_INVALID;
		goto err;
	}

	rc = genavb_port_stats_get(logical_port, response->values, stats_num * sizeof(uint64_t));
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_PVID_SET */
static int dsa_ctrl_port_pvid_set(struct dsa_ctrl_req_port_pvid_set *request)
{
	unsigned int logical_port;
	uint16_t pvid;
	int rc;

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	pvid = request->pvid;
	if (pvid > VLAN_VID_MAX) {
		rc = -GENAVB_ERR_VLAN_VID;
		goto err;
	}

	rc = genavb_vlan_set_port_default(logical_port, pvid);
	if (rc < 0)
			goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_PORT_LINK_SET */
static int dsa_ctrl_port_link_set(struct dsa_ctrl_req_port_link_set *request)
{
	return 0;
}

/* command handler for NETC_CMD_PORT_DROP_UNTAG_SET */
static int dsa_ctrl_port_drop_untag_set(struct dsa_ctrl_req_port *request)
{
	return 0;
}

/* command handler for NETC_CMD_FDB_ADD */
static int dsa_ctrl_fdb_add(struct dsa_ctrl_req_fdb_add *request)
{
	uint8_t address[6];
	uint16_t vid;
	unsigned int logical_port;
	struct genavb_fdb_port_map port_map = {0};
	int rc;

	memcpy(address, request->mac_addr, 6);
	vid = request->vid;
	if (vid > VLAN_VID_MAX) {
		rc = -GENAVB_ERR_VLAN_VID;
		goto err;
	}

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	port_map.port_id = logical_port;
	port_map.control = GENAVB_FDB_PORT_CONTROL_FORWARDING;

	rc = genavb_fdb_update(address, vid, &port_map);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_FDB_DEL */
static int dsa_ctrl_fdb_del(struct dsa_ctrl_req_fdb_del *request)
{
	uint8_t address[6];
	uint16_t vid;
	int rc;

	memcpy(address, request->mac_addr, 6);
	vid = request->vid;
	if (vid > VLAN_VID_MAX) {
		rc = -GENAVB_ERR_VLAN_VID;
		goto err;
	}

	rc = genavb_fdb_delete(address, vid);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_FDB_DUMP */
static void dsa_ctrl_fdb_dump(struct dsa_ctrl_req_fdb_dump *request, struct dsa_ctrl_resp_fdb_dump *response)
{
	uint8_t address[6];
	uint16_t vid;
	bool dynamic;
	struct genavb_fdb_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
	genavb_fdb_status_t status = GENAVB_FDB_STATUS_INVALID;
	uint32_t next = request->resume_entry_id;
	int rc;
	int i;

	rc = genavb_fdb_dump(&next, address, &vid, &dynamic, port_map, &status);
	if (!rc) {
		memcpy(response->mac_addr, address, 6);
		response->vid = vid;
		response->dynamic = dynamic;
		for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
			if (port_map[i].control == GENAVB_FDB_PORT_CONTROL_FORWARDING)
				response->port_map |= (1 << i);
		}
	}
	response->resume_entry_id = next;
}

/* command handler for NETC_CMD_VLAN_ADD */
static int dsa_ctrl_vlan_add(struct dsa_ctrl_req_vlan *request)
{
	uint16_t vid;
	unsigned int logical_port;
	struct genavb_vlan_port_map port_map = {0};
	int rc;

	vid = request->vid;
	if (vid > VLAN_VID_MAX) {
		rc = -GENAVB_ERR_VLAN_VID;
		goto err;
	}

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	port_map.port_id = logical_port;
	port_map.control = GENAVB_VLAN_ADMIN_CONTROL_FIXED;
	port_map.untagged = request->untagged;

	rc = genavb_vlan_update(vid, &port_map);
	if (rc < 0)
		goto err;

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_VLAN_DEL */
static int dsa_ctrl_vlan_del(struct dsa_ctrl_req_vlan *request)
{
	uint16_t vid;
	unsigned int logical_port;
	struct genavb_vlan_port_map port_map = {0};
	struct genavb_vlan_port_map port_map_read[CONFIG_APP_BR_NUM_PORTS] = {0};
	bool dynamic = false;
	uint32_t port_member = 0;
	int i;
	int rc;

	vid = request->vid;
	if (vid > VLAN_VID_MAX) {
		rc = -GENAVB_ERR_VLAN_VID;
		goto err;
	}

	logical_port = br_port_list[request->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -GENAVB_ERR_INVALID_PORT;
		goto err;
	}

	port_map.port_id = logical_port;
	port_map.control = GENAVB_VLAN_ADMIN_CONTROL_FORBIDDEN;
	port_map.untagged = false;

	rc = genavb_vlan_update(vid, &port_map);
	if (rc < 0)
		goto err;

	rc = genavb_vlan_read(vid, &dynamic, port_map_read);
	if (rc < 0)
		goto err;

	for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
		if (port_map_read[i].control == GENAVB_VLAN_ADMIN_CONTROL_FIXED)
			port_member |= (1 << i);

		if (port_map_read[i].control == GENAVB_VLAN_ADMIN_CONTROL_FORBIDDEN)
			port_member &= ~(1 << i);
	}

	if (port_member == 0) {
		rc = genavb_vlan_delete(vid);
		if (rc < 0)
			goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

/* command handler for NETC_CMD_VLAN_DUMP */
static void dsa_ctrl_vlan_dump(struct dsa_ctrl_req_vlan_dump *request, struct dsa_ctrl_resp_vlan_dump *response)
{
	uint16_t vid;
	bool dynamic = false;
	struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
	uint32_t next = request->resume_entry_id;
	int rc;
	int i;

	rc = genavb_vlan_dump(&next, &vid, &dynamic, port_map);
	if (!rc) {
		response->vid = vid;
		response->untagged = port_map[0].untagged;
		for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
			if (port_map[i].control == GENAVB_VLAN_ADMIN_CONTROL_FIXED)
				response->port_map |= (1 << i);
			}
	}
	response->resume_entry_id = next;
}

/* command handler for NETC_CMD_FORWARD_MASK_SET */
static int dsa_ctrl_forward_mask_set(struct dsa_ctrl_req_forward_mask_set *request)
{
	return 0;
}

/* command handler for NETC_CMD_PTP_SYNC_SET */
static int dsa_ctrl_ptp_sync_set(struct dsa_ctrl_req_ptp_sync_set *request)
{
	return 0;
}

/* command handler for NETC_CMD_TIMER_CUR_SET */
static int dsa_ctrl_ptp_cur_set(struct dsa_ctrl_req_ptp_clock *clock)
{
	uint64_t cur_ns;
	int64_t offset;
	genavb_clock_id_t clk_id;
	int rc = 0;

	clk_id = DSA_CTRL_CLK_ID_GET(clock->clock_id);
	rc = genavb_clock_gettime64(clk_id, &cur_ns);
	if (rc < 0)
		return rc;

	offset = clock->ns - cur_ns;

	rc = genavb_clock_setoffset(clk_id, offset);
	if (rc < 0)
		return rc;

	return rc;
}

/* command handler for NETC_CMD_TIMER_CUR_GET */
static int dsa_ctrl_ptp_cur_get(struct dsa_ctrl_req_ptp_clock *clock, uint64_t *ns)
{
	genavb_clock_id_t clk_id;
	int rc = 0;

	clk_id = DSA_CTRL_CLK_ID_GET(clock->clock_id);
	rc = genavb_clock_gettime64(clk_id, ns);
	if (rc < 0)
		return rc;

	return rc;
}

/* command handler for NETC_CMD_TIMER_ADJTIME_SET */
static int dsa_ctrl_ptp_adjoffset_set(struct dsa_ctrl_req_ptp_clock *clock)
{
	genavb_clock_id_t clk_id;
	int rc = 0;

	clk_id = DSA_CTRL_CLK_ID_GET(clock->clock_id);
	rc = genavb_clock_setoffset(clk_id, clock->offset);
	if (rc < 0)
		return rc;

	return rc;
}

/* command handler for NETC_CMD_TIMER_ADJFINE_SET */
static int dsa_ctrl_ptp_adjfreq_set(struct dsa_ctrl_req_ptp_clock *clock)
{
	genavb_clock_id_t clk_id;
	int rc = 0;

	if (clock->ppb != (int32_t)clock->ppb)
		return -1;

	clk_id = DSA_CTRL_CLK_ID_GET(clock->clock_id);
	rc = genavb_clock_setfreq(clk_id, (int32_t)clock->ppb);
	if (rc < 0)
		return rc;

	return rc;
}

/* command handler for NETC_CMD_QBU_SET */
static int dsa_ctrl_qbu_set(struct dsa_ctrl_req_qbu_set *request)
{
	struct genavb_fp_config config = GENAVB_FP_CONFIG_802_1Q_DEFAULT_INIT;
	unsigned int port;

	port = br_port_list[request->port];

	for (int i = 0; i < QOS_PRIORITY_MAX; i++)
		config.u.cfg_802_1Q.admin_status[i] = (request->preemption_mask & (1 << i)) ? GENAVB_FP_ADMIN_STATUS_PREEMPTABLE : GENAVB_FP_ADMIN_STATUS_EXPRESS;

	return genavb_fp_set(port, GENAVB_FP_CONFIG_802_1Q, &config);
}

/* command handler for NETC_CMD_MM_SET */
static int dsa_ctrl_mm_set(struct dsa_ctrl_mm *request)
{
	struct genavb_fp_config config = GENAVB_FP_CONFIG_802_3_DEFAULT_INIT;
	unsigned int port;

	port = br_port_list[request->port];
	config.u.cfg_802_3.enable_tx = request->tx_enabled;
	config.u.cfg_802_3.verify_disable_tx = !request->verify_enabled;
	config.u.cfg_802_3.verify_time = request->verify_time;
	config.u.cfg_802_3.add_frag_size = request->add_frag_size;

	return genavb_fp_set(port, GENAVB_FP_CONFIG_802_3, &config);
}

/* command handler for NETC_CMD_MM_GET */
static int dsa_ctrl_mm_get(struct dsa_ctrl_req_port *request, struct dsa_ctrl_mm *response)
{
	struct genavb_fp_config config = GENAVB_FP_CONFIG_802_3_DEFAULT_INIT;
	unsigned int port;
	int rc;

	port = br_port_list[request->port];

	rc = genavb_fp_get(port, GENAVB_FP_CONFIG_802_3, &config);
	if (rc < 0)
		return rc;

	switch (config.u.cfg_802_3.status_verify) {
	case 0:
		response->verify_status = DSA_MM_VERIFY_STATUS_DISABLED;
		break;
	case 2:
		response->verify_status = DSA_MM_VERIFY_STATUS_VERIFYING;
		break;
	case 3:
		response->verify_status = DSA_MM_VERIFY_STATUS_SUCCEEDED;
		break;
	case 4:
		response->verify_status = DSA_MM_VERIFY_STATUS_FAILED;
		break;
	default:
		response->verify_status = DSA_MM_VERIFY_STATUS_UNKNOWN;
		break;
	}

	response->port = request->port;
	response->verify_time = config.u.cfg_802_3.verify_time;
	response->add_frag_size = config.u.cfg_802_3.add_frag_size;
	response->verify_enabled = config.u.cfg_802_3.verify_disable_tx ? 0 : 1;
	response->verify_status = config.u.cfg_802_3.status_verify;
	response->tx_enabled = config.u.cfg_802_3.enable_tx;
	response->pmac_enabled = config.u.cfg_802_3.enable_tx;
	response->tx_active = config.u.cfg_802_3.enable_tx && (response->verify_status == DSA_MM_VERIFY_STATUS_SUCCEEDED || response->verify_status == DSA_MM_VERIFY_STATUS_DISABLED);

	return 0;
}

static int dsa_ctrl_frer_identification_set(unsigned int port, uint8_t encap, bool del_rtag, uint32_t *streamid, uint8_t stream_n)
{
	struct genavb_sequence_identification entry = {0};

	entry.stream = streamid;
	entry.stream_n = stream_n;
	if (del_rtag)
		entry.active = 0;
	else
		entry.active = 1;

	entry.encapsulation = (genavb_seqi_encapsulation_t)encap;

	return genavb_sequence_identification_update(port, true, &entry);
}

static int dsa_ctrl_frer_sg_set(struct dsa_ctrl_req_frer_sg_set *request)
{
	struct genavb_sequence_generation entry = {0};
	uint32_t streamhandle;
	unsigned int port;
	uint32_t index;
	int rc;

	index = request->stream_handle;
	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	streamhandle = index * DSA_CTRL_STREAM_INGRESS_PORTS;

	entry.stream = &streamhandle;
	entry.stream_n = 1;
	entry.direction_out_facing = 0;

	rc = genavb_sequence_generation_update(index, &entry);
	if (rc < 0)
		return rc;

	for (int i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
		if (i == request->port)
			continue;

		port = br_port_list[i];
		rc = dsa_ctrl_frer_identification_set(port, request->encapsulation, 0, &streamhandle, entry.stream_n);
		if (rc < 0)
			goto err;
	}

	return rc;

err:
	genavb_sequence_generation_delete(index);

	return rc;
}

static int dsa_ctrl_frer_sg_del(uint32_t index)
{
	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	return genavb_sequence_generation_delete(index);
}

static int dsa_ctrl_frer_sr_set(struct dsa_ctrl_req_frer_sr_set *request)
{
	uint32_t stream[DSA_CTRL_STREAM_INGRESS_PORTS];
	struct genavb_sequence_recovery entry = {0};
	unsigned int port;
	uint32_t index;
	int rc;

	index = request->stream_handle;
	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	port = br_port_list[request->port];

	entry.stream = stream;
	for (int i = 0; i < DSA_CTRL_STREAM_INGRESS_PORTS; i++)
		stream[i] = index * DSA_CTRL_STREAM_INGRESS_PORTS + i;

	entry.stream_n = DSA_CTRL_STREAM_INGRESS_PORTS;

	entry.direction_out_facing = 1;
	entry.algorithm = (genavb_seqr_algorithm_t)request->algorithm;
	entry.history_length = request->his_len;
	entry.reset_timeout = request->reset_timeout;
	entry.port_n = 1;
	entry.port = &port;

	rc = genavb_sequence_recovery_update(index, &entry);
	if (rc < 0)
		return rc;

	rc = dsa_ctrl_frer_identification_set(port, request->encapsulation, request->rtag_pop_en, entry.stream, entry.stream_n);
	if (rc < 0)
		goto err;

	return rc;

err:
	genavb_sequence_recovery_delete(index);
	return rc;
}

static int dsa_ctrl_frer_sr_del(uint32_t index)
{
	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	return genavb_sequence_recovery_delete(index);
}

static int dsa_ctrl_streamid_set(struct dsa_ctrl_req_streamid_set *request)
{
	struct genavb_stream_identity entry = {0};
	uint8_t port[CONFIG_APP_BR_NUM_PORTS];
	struct genavb_si_port si_port;
	uint32_t index, streamhandle;
	uint8_t portmask;
	int n = 0;

	index = request->stream_handle;
	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	portmask = request->portmask;

	for (int i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++)
		if (portmask & (1 << i)) {
			port[n++] = i;
		}

	if (n > DSA_CTRL_STREAM_INGRESS_PORTS)
		return -1;

	si_port.pos = GENAVB_SI_PORT_POS_IN_FACING_OUTPUT;
	entry.port = &si_port;
	for (int i = 0; i < n; i++) {
		streamhandle = index * DSA_CTRL_STREAM_INGRESS_PORTS + i;
		entry.handle = streamhandle;
		entry.port_n = 1;
		entry.port->id = br_port_list[port[i]];

		switch(request->type) {
		case GENAVB_SI_NULL:
			entry.type = GENAVB_SI_NULL;
			entry.parameters.null.tagged = (request->vid ? GENAVB_SI_TAGGED : GENAVB_SI_PRIORITY);
			entry.parameters.null.vlan = request->vid;
			memcpy(entry.parameters.null.destination_mac, request->mac_addr, 6);
			break;

		case GENAVB_SI_SRC_MAC_VLAN:
			entry.type = GENAVB_SI_SRC_MAC_VLAN;
			entry.parameters.smac_vlan.tagged = (request->vid ? GENAVB_SI_TAGGED : GENAVB_SI_PRIORITY);
			entry.parameters.smac_vlan.vlan = request->vid;
			memcpy(entry.parameters.smac_vlan.source_mac, request->mac_addr, 6);
			break;

		case GENAVB_SI_DST_MAC_VLAN:
			break;

		default:
			break;
		}

		if (genavb_stream_identification_update(streamhandle, &entry) < 0)
			return -1;
	}

	return 0;
}

static int dsa_ctrl_streamid_del(uint32_t index)
{
	uint32_t stream_handle;

	if (index > DSA_CTRL_STREAMID_MAX)
		return -1;

	for (int i = 0; i < DSA_CTRL_STREAM_INGRESS_PORTS; i++) {
		stream_handle = index * DSA_CTRL_STREAM_INGRESS_PORTS + i;

		if (genavb_stream_identification_delete(stream_handle) < 0)
			return -1;
	}

	return 0;
}

static int dsa_ctrl_priority_map_set(struct dsa_ctrl_req_priority_map *prio_map)
{
	unsigned int logical_port = br_port_list[prio_map->port];
	struct genavb_traffic_class_config config = {0};

	if (logical_port >= CONFIG_APP_LOGICAL_PORTS)
		return -1;

	memcpy(config.tc, prio_map->map, QOS_PRIORITY_MAX);

	return genavb_traffic_class_set(logical_port, &config);
}

static int dsa_ctrl_qbv_set(struct dsa_ctrl_qbv_req *qbvconf)
{
	unsigned int logical_port;
	int rc = 0;

	logical_port = br_port_list[qbvconf->port];
	if (logical_port >= CONFIG_APP_LOGICAL_PORTS) {
		rc = -1;
		goto err;
	}

	rc = genavb_st_set_admin_config(logical_port, GENAVB_CLOCK_BR_0_0, &qbvconf->st_conf);
	if (rc < 0)
		goto err;

err:
	vPortFree(qbvconf->st_conf.control_list);
	memset(qbvconf, 0, sizeof(*qbvconf));

	return rc;
}

static int dsa_ctrl_qbv_param1(struct dsa_ctrl_qbv_req *qbvconf, struct dsa_ctrl_req_qbv_param1 *param)
{
	struct genavb_st_config *st_conf = &qbvconf->st_conf;
	struct genavb_st_gate_control_entry *gcl = NULL;

	if (qbvconf->state != DSA_CTRL_REQ_IDLE) {
		vPortFree(st_conf->control_list);
		memset(qbvconf, 0, sizeof(*qbvconf));
	}

	st_conf->enable = param->enabled;
	st_conf->base_time = param->base_time;
	st_conf->cycle_time_p = param->cycle_time;
	st_conf->cycle_time_q = 1000000000;
	st_conf->list_length = param->gcl_len;
	if (param->gcl_len) {
		gcl = pvPortMalloc(sizeof(struct genavb_st_gate_control_entry) * param->gcl_len);
		if (!gcl)
			return -1;
	}

	st_conf->control_list = gcl;

	qbvconf->port = param->port;
	qbvconf->state = DSA_CTRL_REQ_START;

	return 0;
}

static int dsa_ctrl_qbv_param2(struct dsa_ctrl_qbv_req *qbvconf, struct dsa_ctrl_req_qbv_param2 *param)
{
	struct genavb_st_config *st_conf = &qbvconf->st_conf;

	st_conf->cycle_time_ext = param->cycle_time_ext;

	qbvconf->state = DSA_CTRL_REQ_GET_PARAM;

	if (!st_conf->list_length)
		return dsa_ctrl_qbv_set(qbvconf);

	return 0;
}

static int dsa_ctrl_qbv_gcl(struct dsa_ctrl_qbv_req *qbvconf, struct dsa_ctrl_req_qbv_gcl *param)
{
	struct genavb_st_config *st_conf = &qbvconf->st_conf;
	struct genavb_st_gate_control_entry *gcl = st_conf->control_list;

	for (int i = 0; i < 2; i++) {
		if (qbvconf->gcl_num >= st_conf->list_length)
			return -1;

		gcl[qbvconf->gcl_num].operation = param[i].operation;
		gcl[qbvconf->gcl_num].gate_states = param[i].gate_mask;
		gcl[qbvconf->gcl_num].time_interval = param[i].interval;
		qbvconf->gcl_num++;

		if (qbvconf->gcl_num == st_conf->list_length)
			return dsa_ctrl_qbv_set(qbvconf);
	}

	return 0;
}

static int dsa_ctrl_psfp_sf_set(struct dsa_ctrl_req_psfp_sf_set *request)
{
	struct genavb_stream_filter_instance instance = {0};
	uint32_t streamhandle;
	uint32_t index;
	int rc;

	index = request->stream_handle;
	streamhandle = index * DSA_CTRL_STREAM_INGRESS_PORTS;

	instance.stream_filter_instance_id = index;
	instance.stream_handle = streamhandle;
	instance.max_sdu_size = request->maxsdu;
	if (request->priority_spec < 0)
		instance.priority_spec = GENAVB_PRIORITY_SPEC_WILDCARD;
	else
		instance.priority_spec = request->priority_spec;

	if (request->sg_enable)
		instance.stream_gate_ref = request->stream_handle;
	else
		instance.stream_gate_ref = 0xFFFFFFFF;

	instance.flow_meter_enable = request->fm_enable;
	if (request->fm_enable)
		instance.flow_meter_ref = request->stream_handle;
	else
		instance.flow_meter_ref = 0xFFFFFFFF;

	rc = genavb_stream_filter_update(index, &instance);

	return rc;
}

static int dsa_ctrl_psfp_sg_set(struct dsa_ctrl_psfp_sg_req *sgconf)
{
	uint32_t index = sgconf->gate.stream_gate_instance_id;
	int rc;

	rc = genavb_stream_gate_update(index, GENAVB_CLOCK_BR_0_0, &sgconf->gate);

	vPortFree(sgconf->gate.control_list);
	memset(sgconf, 0, sizeof(*sgconf));

	return rc;
}

static int dsa_ctrl_psfp_sg_param1(struct dsa_ctrl_psfp_sg_req *sgconf, struct dsa_ctrl_req_psfp_sg_param1 *param)
{
	struct genavb_stream_gate_instance *gate = &sgconf->gate;
	struct genavb_stream_gate_control_entry *gcl = NULL;

	if (sgconf->state != DSA_CTRL_REQ_IDLE) {
		vPortFree(gate->control_list);
		memset(sgconf, 0, sizeof(*sgconf));
	}

	gate->stream_gate_instance_id = param->index;
	gate->base_time = param->base_time;
	gate->cycle_time_p = param->cycle_time;
	gate->cycle_time_q = 1000000000;
	gate->list_length = param->gcl_len;
	if (param->gcl_len) {
		gcl = pvPortMalloc(sizeof(struct genavb_stream_gate_control_entry) * param->gcl_len);
		if (!gcl)
			return -1;
	}

	gate->control_list = gcl;

	sgconf->state = DSA_CTRL_REQ_START;

	return 0;
}

static int dsa_ctrl_psfp_sg_param2(struct dsa_ctrl_psfp_sg_req *sgconf, struct dsa_ctrl_req_psfp_sg_param2 *param)
{
	struct genavb_stream_gate_instance *gate = &sgconf->gate;

	gate->cycle_time_extension = param->cycle_time_ext;
	gate->admin_ipv = param->prio;
	sgconf->state = DSA_CTRL_REQ_GET_PARAM;

	return 0;
}

static int dsa_ctrl_psfp_sg_gcl(struct dsa_ctrl_psfp_sg_req *sgconf, struct dsa_ctrl_req_psfp_sg_gcl *param)
{
	struct genavb_stream_gate_control_entry *gcl;

	gcl = &sgconf->gate.control_list[sgconf->gcl_num];

	gcl->time_interval_value = param->interval;

	if (param->maxoctets < 0)
		gcl->interval_octet_max = 0;
	else
		gcl->interval_octet_max = param->maxoctets;

	if (param->ipv < 0)
		gcl->ipv_spec = GENAVB_IPV_SPEC_NULL;
	else
		gcl->ipv_spec = param->ipv;

	gcl->gate_state_value = param->gate_state;

	sgconf->gcl_num++;

	if (sgconf->gcl_num == sgconf->gate.list_length)
		return dsa_ctrl_psfp_sg_set(sgconf);

	return 0;
}

static int dsa_ctrl_psfp_fm_set(struct dsa_ctrl_req_psfp_fm_set *param)
{
	struct genavb_flow_meter_instance fm = {0};
	uint32_t index = param->index;
	int rc;

	fm.flow_meter_instance_id = index;
	fm.committed_information_rate = param->rate;
	fm.committed_burst_size = param->burst;

	rc = genavb_flow_meter_update(index, &fm);

	return rc;
}

static int dsa_ctrl_psfp_sf_del(uint16_t index)
{
	struct genavb_stream_filter_instance instance = {0};
	int rc;

	rc = genavb_stream_filter_read(index, &instance);
	if (rc < 0)
		goto err;

	rc = genavb_stream_filter_delete(index);
	if (rc < 0)
		goto err;

	if (instance.stream_gate_ref != 0xFFFFFFFF) {
		rc = genavb_stream_gate_delete(instance.stream_gate_ref);
		if (rc < 0)
			goto err;
	}

	if (instance.flow_meter_enable) {
		rc = genavb_flow_meter_delete(instance.flow_meter_ref);
		if (rc < 0)
			goto err;
	}

err:
	return rc;
}

static int dsa_ctrl_psfp_sf_get(uint32_t *index, struct dsa_ctrl_resp_psfp_sf_get *resp)
{
	struct genavb_stream_filter_instance instance = {0};
	int rc = 0;

	rc = genavb_stream_filter_read(*index, &instance);
	if (rc < 0)
		return rc;

	resp->pkts = instance.matching_frames_count;
	resp->drops = instance.not_passing_frames_count;

	return rc;
}

static int dsa_ctrl_hsr_set(struct dsa_ctrl_req_hsr_config *request)
{
	struct genavb_handle *genavb_handle = get_genavb_handle();
	uint8_t hsr_port_mask;
	int rc;

	if (request->hsr_enabled)
		hsr_port_mask = (1 << request->hsr_port_a) | (1 << request->hsr_port_b);
	else
		hsr_port_mask = 0;

	rc = genavb_hsr_port_set(genavb_handle, hsr_port_mask);
	if (rc < 0)
		return rc;

	return GENAVB_SUCCESS;
}

#ifdef DSA_CTRL_INTF_LPSPI

static int dsa_ctrl_decode(uint32_t ctrl, uint32_t *access, uint32_t *count, uint32_t *netc_cmd)
{
	if (!access || !count || !netc_cmd)
		return -GENAVB_ERR_INVALID_PARAMS;

	*access = DSA_CTRL_LPSPI_ACCESS_TYPE_DECODE(ctrl);
	*count = DSA_CTRL_LPSPI_RW_COUNT_DECODE(ctrl);
	*netc_cmd = DSA_CTRL_LPSPI_CMD_DECODE(ctrl);

	return GENAVB_SUCCESS;
}

static int dsa_ctrl_recv(uint8_t *buf, uint32_t len)
{
	int rc;

	rc = dsa_ctrl_lpspi_recv(buf, len);
	if (rc < 0) {
		log_err("dsa_ctrl_lpspi_recv() error\n");
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

static int dsa_ctrl_send(uint8_t *buf, uint32_t len)
{
	int rc;

	rc = dsa_ctrl_lpspi_send(buf, len);
	if (rc < 0) {
		log_err("dsa_ctrl_lpspi_send() error\n");
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

static int dsa_ctrl_init(void)
{
	int rc;

	rc = dsa_ctrl_lpspi_init();
	if (rc < 0) {
		log_err("dsa_ctrl_lpspi_init() error\n");
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

#endif

static int dsa_ctrl_cmd_handler(struct dsa_control_ctx *ctx)
{
	uint32_t access_type;
	uint32_t count;	/* number of 32-bit words */
	uint32_t netc_cmd;
	int rc;

	memset(&request, 0, sizeof(struct dsa_ctrl_request));

	/* Receive the 16 bytes fixed length request message from MPU */
	rc = dsa_ctrl_recv((uint8_t *)&request, DSA_CTRL_REQUEST_LEN);
	if (rc < 0)
		goto err;

	rc = dsa_ctrl_decode(request.ctrl, &access_type, &count, &netc_cmd);
	if (rc < 0)
		goto err;

	/* Process the command */
	switch (netc_cmd) {
	case NETC_CMD_SYS_INFO_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_SYS_INFO_GET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		dsa_ctrl_sys_info_get((struct dsa_ctrl_resp_sys_info_get *)response.data);

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_resp_sys_info_get));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_PORT_DSA_ADD:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_DSA_ADD\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_dsa_add((struct dsa_ctrl_req_port_dsa_add *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_dsa_rx_add() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_DSA_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_DSA_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_dsa_del((struct dsa_ctrl_req_port_dsa_del *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_dsa_tx_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_MTU_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_MTU_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_mtu_set((struct dsa_ctrl_port_mtu *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_mtu_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_MTU_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_PORT_MTU_GET\n");
			rc = GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_mtu_get((struct dsa_ctrl_port_mtu *)request.data, (struct dsa_ctrl_port_mtu *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_mtu_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_port_mtu));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_PORT_PHYLINK_MODE_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_PHYLINK_MODE_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_phylink_mode_set((struct dsa_ctrl_port_phylink *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_phylink_mode_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_PHYLINK_STATUS_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_PORT_PHYLINK_STATUS_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_phylink_status_get((struct dsa_ctrl_req_port *)request.data, (struct dsa_ctrl_port_phylink *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_phylink_status_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_port_phylink));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_PORT_ETHTOOL_STATS_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_PORT_ETHTOOL_STATS_GET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_ethtool_stats_get((struct dsa_ctrl_req_port *)request.data, (struct dsa_ctrl_resp_port_ethtool_stats *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_ethtool_status_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_resp_port_ethtool_stats));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_PORT_PVID_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_PVID_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_pvid_set((struct dsa_ctrl_req_port_pvid_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_pvid_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_LINK_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_LINK_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_link_set((struct dsa_ctrl_req_port_link_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_link_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PORT_DROP_UNTAG_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PORT_DROP_UNTAG_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_port_drop_untag_set((struct dsa_ctrl_req_port *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_port_drop_untag_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FDB_ADD:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FDB_ADD\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_fdb_add((struct dsa_ctrl_req_fdb_add *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_fdb_add() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FDB_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FDB_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_fdb_del((struct dsa_ctrl_req_fdb_del *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_fdb_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FDB_DUMP:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_FDB_DUMP\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		dsa_ctrl_fdb_dump((struct dsa_ctrl_req_fdb_dump *)request.data, (struct dsa_ctrl_resp_fdb_dump *)response.data);

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_resp_fdb_dump));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_VLAN_ADD:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_VLAN_ADD\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_vlan_add((struct dsa_ctrl_req_vlan *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_vlan_add() error\n");
			goto err;
		}

		break;

	case NETC_CMD_VLAN_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_VLAN_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_vlan_del((struct dsa_ctrl_req_vlan *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_vlan_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_VLAN_DUMP:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_VLAN_DUMP\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		dsa_ctrl_vlan_dump((struct dsa_ctrl_req_vlan_dump *)request.data, (struct dsa_ctrl_resp_vlan_dump *)response.data);

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_resp_vlan_dump));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_FORWARD_MASK_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FORWARD_MASK_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_forward_mask_set((struct dsa_ctrl_req_forward_mask_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_forward_mask_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PTP_SYNC_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PTP_SYNC_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_ptp_sync_set((struct dsa_ctrl_req_ptp_sync_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_ptp_sync_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_TIMER_CUR_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_TIMER_CUR_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_ptp_cur_set((struct dsa_ctrl_req_ptp_clock *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_ptp_cur_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_TIMER_CUR_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_TIMER_CUR_GET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_ptp_cur_get((struct dsa_ctrl_req_ptp_clock *)request.data, (uint64_t *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_ptp_cur_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(uint64_t));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_TIMER_ADJTIME_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_TIMER_ADJTIME_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_ptp_adjoffset_set((struct dsa_ctrl_req_ptp_clock *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_ptp_adjoffset_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_TIMER_ADJFINE_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_TIMER_ADJFINE_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_ptp_adjfreq_set((struct dsa_ctrl_req_ptp_clock *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_ptp_adjfreq_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QBU_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QBU_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_qbu_set((struct dsa_ctrl_req_qbu_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_qbu_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_MM_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_MM_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_mm_set((struct dsa_ctrl_mm *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_mm_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_MM_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_QBU_GET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_mm_get((struct dsa_ctrl_req_port *)request.data, (struct dsa_ctrl_mm *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_mm_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_mm));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_QCI_SF_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_SF_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sf_set((struct dsa_ctrl_req_psfp_sf_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sf_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_SG_SET_P1:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_SG_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sg_param1(&sgconf, (struct dsa_ctrl_req_psfp_sg_param1 *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sg_param1() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_SG_SET_P2:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_SG_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		if (sgconf.state != DSA_CTRL_REQ_START) {
			log_err("Wrong state(%d) for NETC_CMD_SG_SET\n", sgconf.state);
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sg_param2(&sgconf, (struct dsa_ctrl_req_psfp_sg_param2 *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sg_param2() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_SG_SET_GCL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_SG_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		if (sgconf.state != DSA_CTRL_REQ_GET_PARAM) {
			log_err("Wrong state(%d) for NETC_CMD_SG_SET\n", sgconf.state);
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sg_gcl(&sgconf, (struct dsa_ctrl_req_psfp_sg_gcl *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sg_gcl() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_FM_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_FM_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_fm_set((struct dsa_ctrl_req_psfp_fm_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_fm_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QCI_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sf_del(*(uint16_t *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sf_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QCI_GET:
		if (access_type != DSA_CTRL_READ) {
			log_err("Wrong access type for NETC_CMD_QCI_GET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_psfp_sf_get((uint32_t *)request.data, (struct dsa_ctrl_resp_psfp_sf_get *)response.data);
		if (rc < 0) {
			log_err("dsa_ctrl_psfp_sf_get() error\n");
			goto err;
		}

		rc = dsa_ctrl_send(response.data, sizeof(struct dsa_ctrl_resp_psfp_sf_get));
		if (rc < 0)
			goto err;

		break;

	case NETC_CMD_FRER_SEQG_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FRER_SEQG_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_frer_sg_set((struct dsa_ctrl_req_frer_sg_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_frer_sg_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FRER_SEQG_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FRER_SEQG_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_frer_sg_del(*(uint16_t *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_frer_sg_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FRER_SEQR_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FRER_SEQR_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_frer_sr_set((struct dsa_ctrl_req_frer_sr_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_frer_sr_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_FRER_SEQR_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_FRER_SEQR_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_frer_sr_del(*(uint16_t *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_frer_sr_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_STREAMID_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_STREAMID_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_streamid_set((struct dsa_ctrl_req_streamid_set *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_streamid_set() error\n");
			goto err;
		}

		break;

	case NETC_CMD_STREAMID_DEL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_STREAMID_DEL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_streamid_del(*(uint16_t *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_streamid_del() error\n");
			goto err;
		}

		break;

	case NETC_CMD_PRIORITY_MAP_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_PRIORITY_MAP_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		dsa_ctrl_priority_map_set((struct dsa_ctrl_req_priority_map *)request.data);

		break;

	case NETC_CMD_QBV_SET_P1:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QBV_SET_P1\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_qbv_param1(&qbvconf, (struct dsa_ctrl_req_qbv_param1 *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_qbv_param1() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QBV_SET_P2:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QBV_SET_P2\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		if (qbvconf.state != DSA_CTRL_REQ_START) {
			log_err("Wrong state(%d) for NETC_CMD_QBV_SET_P2\n", qbvconf.state);
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_qbv_param2(&qbvconf, (struct dsa_ctrl_req_qbv_param2 *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_qbv_param2() error\n");
			goto err;
		}

		break;

	case NETC_CMD_QBV_SET_GCL:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_QBV_SET_GCL\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		if (qbvconf.state != DSA_CTRL_REQ_GET_PARAM) {
			log_err("Wrong state(%d) for NETC_CMD_QBV_SET_GCL\n", qbvconf.state);
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_qbv_gcl(&qbvconf, (struct dsa_ctrl_req_qbv_gcl *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_qbv_gcl() error\n");
			goto err;
		}

		break;

	case NETC_CMD_HSR_SET:
		if (access_type != DSA_CTRL_WRITE) {
			log_err("Wrong access type for NETC_CMD_HSR_SET\n");
			rc = -GENAVB_ERR_INVALID_PARAMS;
			goto err;
		}

		rc = dsa_ctrl_hsr_set((struct dsa_ctrl_req_hsr_config *)request.data);
		if (rc < 0) {
			log_err("dsa_ctrl_hsr_set() error\n");
			goto err;
		}

		break;

	default:
		log_err("Unsupported NETC command 0x%x received from MPU\n", request.ctrl);
		rc = -GENAVB_ERR_INVALID_PARAMS;
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

static void dsa_ctrl_task(void *pvParameters)
{
	struct dsa_control_ctx *ctx = (struct dsa_control_ctx *)pvParameters;
	int ret = 0;

	log_info("DSA control task started\n");

	ret = dsa_ctrl_init();
	if (ret < 0)
		goto err;

	/*
	 * Main loop
	 */
	while (1) {
		ret = dsa_ctrl_cmd_handler(ctx);
		if (ret < 0)
			continue;
	}

err:
	vTaskDelete(NULL);
}

int dsa_ctrl_task_init(void)
{
	BaseType_t rc;

	rc = xTaskCreate(&dsa_ctrl_task, DSA_CTRL_TASK_NAME, DSA_CTRL_TASK_STACK_SIZE, (void *)&dsa_ctrl_ctx,
				DSA_CTRL_TASK_PRIORITY, &dsa_ctrl_ctx.task_handle);
	if (rc != pdPASS) {
		log_err("xTaskCreate(%s) failed\n", DSA_CTRL_TASK_NAME);
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

void dsa_ctrl_task_exit(void)
{
	vTaskDelete(dsa_ctrl_ctx.task_handle);
	dsa_ctrl_ctx.task_handle = NULL;
}
