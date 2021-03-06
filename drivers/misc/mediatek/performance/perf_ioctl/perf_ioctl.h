/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#ifndef PERF_IOCTL_H
#define PERF_IOCTL_H
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/utsname.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>

#include <linux/platform_device.h>

#include <linux/ioctl.h>
#include <legacy_controller.h>

#define DEV_MAJOR 121
#define DEV_NAME "debug"

enum {
	SWUI = 0,
	HWUI,
	GLSURFACE
};

enum {
	WINDOW_DISCONNECT = 0,
	NATIVE_WINDOW_API_EGL = 1,
	NATIVE_WINDOW_API_CPU = 2,
	NATIVE_WINDOW_API_MEDIA = 3,
	NATIVE_WINDOW_API_CAMERA = 4,
};

typedef struct _FPSGO_PACKAGE {
	__u32 tid;
	union {
		__u64 frame_time;
		__u64 bufID;
	};
	union {
		__u32 start;
		__u32 connectedAPI;
		__u32 render_method;
	};
} FPSGO_PACKAGE;

#define FPSGO_QUEUE                  _IOW('g', 1, FPSGO_PACKAGE)
#define FPSGO_DEQUEUE                _IOW('g', 3, FPSGO_PACKAGE)
#define FPSGO_VSYNC                  _IOW('g', 5, FPSGO_PACKAGE)
#define FPSGO_ACT_SWITCH             _IOW('g', 8, FPSGO_PACKAGE)
#define FPSGO_GAME                   _IOW('g', 9, FPSGO_PACKAGE)
#define FPSGO_TOUCH                  _IOW('g', 10, FPSGO_PACKAGE)
#define FPSGO_FRAME_COMPLETE         _IOW('g', 11, FPSGO_PACKAGE)
#define FPSGO_INTENDED_VSYNC         _IOW('g', 12, FPSGO_PACKAGE)
#define FPSGO_NO_RENDER              _IOW('g', 13, FPSGO_PACKAGE)
#define FPSGO_SWAP_BUFFER            _IOW('g', 14, FPSGO_PACKAGE)
#define FPSGO_QUEUE_CONNECT          _IOW('g', 15, FPSGO_PACKAGE)

#endif

