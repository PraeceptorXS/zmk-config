/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include "layer_info.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
struct layer_info_state {
    zmk_keymap_layer_index_t index;
    const char *label;
};

static lv_color_t layer_widget_buf[DISP_WIDTH * LAYER_HEIGHT];

static void draw_layer(lv_obj_t *canvas, struct layer_info_state state) {

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_unscii_16, LV_TEXT_ALIGN_CENTER);

    // Fill background
    lv_canvas_draw_rect(canvas, 0, 0, DISP_WIDTH, LAYER_HEIGHT, &rect_black_dsc);

    // Draw layer
    if (state.label == NULL) {
        char text[4] = {};
        sprintf(text, "L%i", state.index);
        lv_canvas_draw_text(canvas, 0, 5, 68, &label_dsc, text);
    } else {
        lv_canvas_draw_text(canvas, 0, 5, 68, &label_dsc, state.label);
    }

    // Rotate canvas
    rotate_canvas(canvas);
}

static void set_layer_info(struct zmk_widget_layer_info *widget, struct layer_info_state state) {
    draw_layer(widget->obj, state);
}

static void layer_info_update_cb(struct layer_info_state state) {
    struct zmk_widget_layer_info *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_info(widget, state); }
}

static struct layer_info_state layer_info_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct layer_info_state){
        .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_info, struct layer_info_state, layer_info_update_cb,
                            layer_info_get_state)

ZMK_SUBSCRIPTION(widget_layer_info, zmk_layer_state_changed);

int zmk_widget_layer_info_init(struct zmk_widget_layer_info *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);
    lv_obj_set_size(widget->obj, DISP_WIDTH, LAYER_HEIGHT);
    lv_canvas_set_buffer(widget->obj, &layer_widget_buf, DISP_WIDTH, LAYER_HEIGHT,
                         LV_IMG_CF_TRUE_COLOR);
    sys_slist_append(&widgets, &widget->node);

    widget_layer_info_init();

    return 0;
}

lv_obj_t *zmk_widget_layer_info_obj(struct zmk_widget_layer_info *widget) { return widget->obj; }
