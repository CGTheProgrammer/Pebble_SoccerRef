#include <pebble.h>

static Window *window;
static TextLayer *team1_name_text_layer;
static TextLayer *team1_score_text_layer;
static TextLayer *team2_name_text_layer;
static TextLayer *team2_score_text_layer;
int team1Score;
int team2Score;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  team1Score++;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  team2Score++;
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	team1Score = 0;
	team2Score = 0;

  team1_name_text_layer = text_layer_create((GRect) { .origin = { 10, 5 }, .size = { bounds.size.w, 20 } });
	team1_score_text_layer = text_layer_create((GRect) { .origin = { 120, 5 }, .size = { bounds.size.w, 20 } });
	team2_name_text_layer = text_layer_create((GRect) { .origin = { 10, 145 }, .size = { bounds.size.w, 20 } });
	team2_score_text_layer = text_layer_create((GRect) { .origin = { 120, 145 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(team1_name_text_layer, "Team1");
	text_layer_set_text(team1_score_text_layer, "0");
	text_layer_set_text(team2_name_text_layer, "Team2");
	text_layer_set_text(team2_score_text_layer, "0");
  layer_add_child(window_layer, text_layer_get_layer(team1_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team1_score_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_score_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(team1_name_text_layer);
	text_layer_destroy(team1_score_text_layer);
	text_layer_destroy(team2_name_text_layer);
	text_layer_destroy(team2_score_text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}