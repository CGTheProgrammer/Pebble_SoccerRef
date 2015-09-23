#include <pebble.h>

#define TEAM1_PSCORE 1
#define TEAM2_PSCORE 2
#define DEFAULT_SCORE 0

static Window *window;
static TextLayer *team1_name_text_layer;
static TextLayer *team1_score_text_layer;
static TextLayer *team2_name_text_layer;
static TextLayer *team2_score_text_layer;
int team1Score;
int team2Score;

// Converts an int to a string
char *itoa1(int num){
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;

  if(num > 0) {
		// count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the number
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
  }else{
    string = "0";
  }
  return string;
}

// Converts an int to a string
char *itoa2(int num){
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;

  if(num > 0) {
		// count how many characters in the number
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    // assign the number to the buffer starting at the end of the number
    for(i = 0; i < length; i++) {
      buff[(length-1)-i] = '0' + (num % 10);
      num /= 10;
    }
    buff[i] = '\0'; // can't forget the null byte to properly end our string
  }else{
    string = "0";
  }
  return string;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Pause Time

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  team1Score++;
	text_layer_set_text(team1_score_text_layer, itoa1(team1Score));
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  team2Score++;
	text_layer_set_text(team2_score_text_layer, itoa2(team2Score));
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create Text Layers
  team1_name_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10, bounds.size.h/20 }, .size = { bounds.size.w, 25 } });
	team1_score_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10 * 9, bounds.size.h/20 }, .size = { bounds.size.w, 25 } });
	team2_name_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10, bounds.size.h/20 * 16 }, .size = { bounds.size.w, 25 } });
	team2_score_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10 * 9, bounds.size.h/20 * 16 }, .size = { bounds.size.w, 25 } });

  // Set Font
  text_layer_set_font(team1_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team1_score_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team2_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team2_score_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  // Set Text Layer Values
  text_layer_set_text(team1_name_text_layer, "Team1");
	text_layer_set_text(team1_score_text_layer, itoa1(team1Score));
	text_layer_set_text(team2_name_text_layer, "Team2");
	text_layer_set_text(team2_score_text_layer, itoa2(team2Score));

  // Add Text Layer Childs to Window
  layer_add_child(window_layer, text_layer_get_layer(team1_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team1_score_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_score_text_layer));
}

static void window_unload(Window *window) {
  // Destroy Text Layers
  text_layer_destroy(team1_name_text_layer);
	text_layer_destroy(team1_score_text_layer);
	text_layer_destroy(team2_name_text_layer);
	text_layer_destroy(team2_score_text_layer);
}

static void init(void) {
  // Set Score Values ? Persisent Exist Set Score
  team1Score = persist_exists(TEAM1_PSCORE) ? persist_read_int(TEAM1_PSCORE) : DEFAULT_SCORE;
  team2Score = persist_exists(TEAM2_PSCORE) ? persist_read_int(TEAM2_PSCORE) : DEFAULT_SCORE;

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
  // Persistantly Save Team Scores
  persist_write_int(TEAM1_PSCORE, team1Score);
  persist_write_int(TEAM2_PSCORE, team2Score);

  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
