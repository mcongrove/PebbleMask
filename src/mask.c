/**
 * This code is provided under the Apache 2.0 license.
 * Please read the LICENSE.md file for more information
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Based off the concept watch face of Filip Slovacek (http://www.slvczch.com)
 */
#include <pebble.h>

static char THEME[6] = "dark";

Window *window;
Layer *minute_overlay_layer;
Layer *hour_overlay_layer;
static GPath *minute_overlay_path;
static GPath *hour_overlay_path;
GBitmap *numbers_image;
BitmapLayer *numbers_layer;
InverterLayer *inverter_layer;

enum {
	KEY_THEME
};

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

static void set_theme() {
	if (persist_exists(KEY_THEME)) {
		persist_read_string(KEY_THEME, THEME, 6);
	}
	
	APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED THEME: %s", THEME);
	
	bool hide = strcmp(THEME, "light") == 0 ? true : false;
	
	layer_set_hidden(inverter_layer_get_layer(inverter_layer), hide);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *theme_tuple = dict_find(iter, KEY_THEME);
	
	if (theme_tuple) {
		persist_write_string(KEY_THEME, theme_tuple->value->cstring);
		strncpy(THEME, theme_tuple->value->cstring, 6);
		
		set_theme();
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
	
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	if (tick_time->tm_sec == 0) {
		layer_mark_dirty(minute_overlay_layer);
		
		if (tick_time->tm_min % 2 == 0) {
			layer_mark_dirty(hour_overlay_layer);
		}
	}
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
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_open(64, 0);
	
	window = window_create();
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
	
	// Create the inverter layer
	inverter_layer = inverter_layer_create(bounds);
	layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	
	set_theme();
}

static void deinit() {
	window_destroy(window);
	layer_destroy(minute_overlay_layer);
	layer_destroy(hour_overlay_layer);
	gpath_destroy(minute_overlay_path);
	gpath_destroy(hour_overlay_path);
	gbitmap_destroy(numbers_image);
	bitmap_layer_destroy(numbers_layer);
	inverter_layer_destroy(inverter_layer);
	
	tick_timer_service_unsubscribe();
	app_message_deregister_callbacks();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}