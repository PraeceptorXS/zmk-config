/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include "util.h"

struct zmk_widget_layer_info {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_layer_info_init(struct zmk_widget_layer_info *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_layer_info_obj(struct zmk_widget_layer_info *widget);
