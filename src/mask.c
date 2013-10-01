/**
 * This code is provided under the MIT License.
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Based off the concept watch face of Filip Slovacek (http://www.slvczch.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x6D, 0xB9, 0x1B, 0x59, 0x28, 0x59, 0x44, 0x59, 0xB3, 0x7D, 0xB7, 0x0D, 0x40, 0x04, 0xBB, 0x68 }

PBL_APP_INFO(MY_UUID, "Mask", "Matthew Congrove", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;
GRect AnalogueGRect;
BmpContainer numbers_image;
Layer minute_overlay_layer, hour_overlay_layer;
GPath minute_overlay_path, hour_overlay_path;

const GPathInfo MINUTE_OVERLAY_POINTS = {
	3,
	(GPoint[]) {
		{-12, -47},
		{0, 0},
		{12, -47}
	}
};

const GPathInfo HOUR_OVERLAY_POINTS = {
	3,
	(GPoint[]) {
		{-28, -125},
		{0, 0},
		{28, -125}
	}
};

void init_layer_path_and_center(Layer *layer, GPath *path, const GPathInfo *pathInfo, const void *updateProc) {
	layer_init(layer, AnalogueGRect);
	layer->update_proc = updateProc;
	
	gpath_init(path, pathInfo);
	gpath_move_to(path, grect_center_point(&layer->frame));
}

void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;

	PblTm t;
	get_time(&t);

	unsigned int angle = (t.tm_min * 6);
	gpath_rotate_to(&minute_overlay_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, grect_center_point(&me->frame), 44);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, &minute_overlay_path);
}

void hour_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;

	PblTm t;
	get_time(&t);

	unsigned int angle = (t.tm_hour * 30) + (t.tm_min / 2);
	gpath_rotate_to(&hour_overlay_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, grect_center_point(&me->frame), 125);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, grect_center_point(&me->frame), 47);
	gpath_draw_filled(ctx, &hour_overlay_path);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	if (t->tick_time->tm_sec == 0) {
		layer_mark_dirty(&minute_overlay_layer);
		
		if (t->tick_time->tm_min % 2 == 0) {
			layer_mark_dirty(&hour_overlay_layer);
		}
	}
}

void display_init(AppContextRef *ctx) {
	window_init(&window, "Mask");
	window_stack_push(&window, true);
	
	init_layer_path_and_center(&hour_overlay_layer, &hour_overlay_path, &HOUR_OVERLAY_POINTS, &hour_display_layer_update_callback);
	layer_add_child(&window.layer, &hour_overlay_layer);
	
	init_layer_path_and_center(&minute_overlay_layer, &minute_overlay_path, &MINUTE_OVERLAY_POINTS, &minute_display_layer_update_callback);
	layer_add_child(&window.layer, &minute_overlay_layer);
	
	bmp_init_container(RESOURCE_ID_IMAGE_NUMBERS, &numbers_image);
	bitmap_layer_set_compositing_mode(&numbers_image.layer, GCompOpOr);
	layer_add_child(&window.layer, &numbers_image.layer.layer);
}

void handle_init(AppContextRef ctx) {
	resource_init_current_app(&APP_RESOURCES);
	
	AnalogueGRect = GRect(0, 0, 144, 168);
	
	display_init(&ctx);
	
	layer_mark_dirty(&minute_overlay_layer);
	layer_mark_dirty(&hour_overlay_layer);
}

void handle_deinit(AppContextRef ctx) {
	bmp_deinit_container(&numbers_image);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	
	app_event_loop(params, &handlers);
}