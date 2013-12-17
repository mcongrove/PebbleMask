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
#include <pebble.h>

Window *window;
Layer *minute_overlay_layer;
Layer *hour_overlay_layer;
static GPath *minute_overlay_path;
static GPath *hour_overlay_path;
GBitmap *numbers_image;
BitmapLayer *numbers_layer;

const GPathInfo MINUTE_OVERLAY_POINTS = {
	4,
	(GPoint[]) {
		{0, 0},
		{-12, -46},
		{-1, -47},
		{12, -46}
	}
};

const GPathInfo HOUR_OVERLAY_POINTS = {
	3,
	(GPoint[]) {
		{-31, -125},
		{0, 0},
		{31, -125}
	}
};

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(minute_overlay_layer);
	layer_mark_dirty(hour_overlay_layer);
	/*
	if (tick_time->tm_sec == 0) {
		layer_mark_dirty(minute_overlay_layer);
		
		if (tick_time->tm_min % 2 == 0) {
			layer_mark_dirty(hour_overlay_layer);
		}
	}
	*/
}

static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);

	unsigned int angle = ((t->tm_min - (t->tm_min % 5)) * 6);
	gpath_rotate_to(minute_overlay_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 44);
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, minute_overlay_path);
}

static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);

	unsigned int angle = (t->tm_hour * 30);
	gpath_rotate_to(hour_overlay_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 125);
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, hour_overlay_path);
	graphics_fill_circle(ctx, center, 47);
}

static void init() {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true);
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	// Create hour path
	hour_overlay_layer = layer_create(bounds);
	layer_set_update_proc(hour_overlay_layer, hour_display_layer_update_callback);
	layer_add_child(window_layer, hour_overlay_layer);
	hour_overlay_path = gpath_create(&HOUR_OVERLAY_POINTS);
	gpath_move_to(hour_overlay_path, grect_center_point(&bounds));
	
	// Create minute path
	minute_overlay_layer = layer_create(bounds);
	layer_set_update_proc(minute_overlay_layer, minute_display_layer_update_callback);
	layer_add_child(window_layer, minute_overlay_layer);
	minute_overlay_path = gpath_create(&MINUTE_OVERLAY_POINTS);
	gpath_move_to(minute_overlay_path, grect_center_point(&bounds));
	
	// Create background image
	numbers_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBERS);
	numbers_layer = bitmap_layer_create(layer_get_frame(window_layer));
	bitmap_layer_set_bitmap(numbers_layer, numbers_image);
	bitmap_layer_set_compositing_mode(numbers_layer, GCompOpOr);
	layer_add_child(window_layer, bitmap_layer_get_layer(numbers_layer));
	
//	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);
}

static void deinit() {
	window_destroy(window);
	layer_destroy(minute_overlay_layer);
	layer_destroy(hour_overlay_layer);
	gpath_destroy(minute_overlay_path);
	gpath_destroy(hour_overlay_path);
	gbitmap_destroy(numbers_image);
	bitmap_layer_destroy(numbers_layer);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}