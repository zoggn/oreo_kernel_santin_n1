/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

/* flat sensor driver
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/atomic.h>

#include <hwmsensor.h>
#include <sensors_io.h>
#include "flat.h"
#include "situation.h"

#include <hwmsen_helper.h>

#include <SCP_sensorHub.h>
#include <linux/notifier.h>
#include "scp_helper.h"

#define FLAT_TAG                  "[flat] "
#define FLAT_FUN(f)               pr_debug(FLAT_TAG"%s\n", __func__)
#define FLAT_PR_ERR(fmt, args...)    pr_err(FLAT_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define FLAT_LOG(fmt, args...)    pr_debug(FLAT_TAG fmt, ##args)

static int flat_get_data(int *probability, int *status)
{
	int err = 0;
	struct data_unit_t data;
	uint64_t time_stamp = 0;
	uint64_t time_stamp_gpt = 0;

	err = sensor_get_data_from_hub(ID_FLAT, &data);
	if (err < 0) {
		FLAT_PR_ERR("sensor_get_data_from_hub fail!!\n");
		return -1;
	}
	time_stamp = data.time_stamp;
	time_stamp_gpt = data.time_stamp_gpt;
	*probability = data.gesture_data_t.probability;
	FLAT_LOG("recv ipi: timestamp: %lld, timestamp_gpt: %lld, probability: %d!\n", time_stamp, time_stamp_gpt,
		*probability);
	return 0;
}

static int flat_open_report_data(int open)
{
	int ret = 0;

	FLAT_LOG("%s : enable=%d\n", __func__, open);
#if defined CONFIG_MTK_SCP_SENSORHUB_V1
	if (open == 1)
		ret = sensor_set_delay_to_hub(ID_FLAT, 120);
#elif defined CONFIG_NANOHUB

#else

#endif

	ret = sensor_enable_to_hub(ID_FLAT, open);
	return ret;
}

static int flat_batch(int flag, int64_t samplingPeriodNs, int64_t maxBatchReportLatencyNs)
{
	FLAT_LOG("%s : flag=%d\n", __func__, flag);

	return sensor_batch_to_hub(ID_FLAT, flag, samplingPeriodNs, maxBatchReportLatencyNs);
}

static int flat_recv_data(struct data_unit_t *event, void *reserved)
{
	if (event->flush_action == FLUSH_ACTION)
		FLAT_PR_ERR("flat do not support flush\n");
	else if (event->flush_action == DATA_ACTION)
		situation_notify(ID_FLAT);
	return 0;
}

static int flat_local_init(void)
{
	struct situation_control_path ctl = {0};
	struct situation_data_path data = {0};
	int err = 0;

	ctl.open_report_data = flat_open_report_data;
	ctl.batch = flat_batch;
	ctl.is_support_wake_lock = true;
	err = situation_register_control_path(&ctl, ID_FLAT);
	if (err) {
		FLAT_PR_ERR("register flat control path err\n");
		goto exit;
	}

	data.get_data = flat_get_data;
	err = situation_register_data_path(&data, ID_FLAT);
	if (err) {
		FLAT_PR_ERR("register flat data path err\n");
		goto exit;
	}
	err = scp_sensorHub_data_registration(ID_FLAT, flat_recv_data);
	if (err) {
		FLAT_PR_ERR("SCP_sensorHub_data_registration fail!!\n");
		goto exit_create_attr_failed;
	}
	return 0;
exit:
exit_create_attr_failed:
	return -1;
}
static int flat_local_uninit(void)
{
	return 0;
}

static struct situation_init_info flat_init_info = {
	.name = "flat_hub",
	.init = flat_local_init,
	.uninit = flat_local_uninit,
};

static int __init flat_init(void)
{
	situation_driver_add(&flat_init_info, ID_FLAT);
	return 0;
}

static void __exit flat_exit(void)
{
	FLAT_FUN();
}

module_init(flat_init);
module_exit(flat_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GLANCE_GESTURE_HUB driver");
MODULE_AUTHOR("ruixue.su@mediatek.com");

