#include <pebble.h>
#include "checkbox_window.h"
#include "ref_calls_window.h"

#define TEAM1_PSCORE 1
#define TEAM2_PSCORE 2
#define DEFAULT_SCORE 0
#define NUM_WINDOWS 3

static Window *window;

// Team Displays
static TextLayer *team1_name_text_layer;
static TextLayer *team1_score_text_layer;
static TextLayer *team2_name_text_layer;
static TextLayer *team2_score_text_layer;

// Game Info Display
static TextLayer *gameTime_text_layer;
static TextLayer *gameHalf_text_layer;
static AppTimer  *gameTime;

// Time Display
#define LAP_TIME_SIZE 5
static char lap_times[LAP_TIME_SIZE][11] = {"00:00:00.0", "00:01:00.0", "00:02:00.0", "00:03:00.0", "00:04:00.0"};
static TextLayer* lap_layers[LAP_TIME_SIZE]; // temporary layer
static int next_lap_layer = 0;
static int lap_time_count = 0;
static double last_lap_time = 0;

#define FONT_BIG_TIME RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_SUBSET_30
#define FONT_SECONDS  RESOURCE_ID_FONT_DEJAVU_SANS_SUBSET_18
#define FONT_LAPS     RESOURCE_ID_FONT_DEJAVU_SANS_SUBSET_22

// Keeping Track of Time
static double elapsed_time = 0;
static bool started = false;
static AppTimer* update_timer = NULL;
static double start_time = 0;
static double pause_time = 0;

#define TIMER_UPDATE 1

// Global Animation Lock.
static int busy_animating = 0;

// Global Varialbes
int gameHalf;
int team1Score;
int team2Score;



static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 12h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(gameTime_text_layer, buffer);
}

static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {
  // Needs to be static because it's used by the system later.
  static char game_time_text[] = "00:00:00";

  strftime(game_time_text, sizeof(game_time_text), "%T", tick_time);
  text_layer_set_text(gameTime_text_layer, game_time_text);
}

// Converts team1 score to a string
char *convertTeam1Score(int num){
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

// Converts team2 score to a string
char *convertTeam2Score(int num){
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

static void timer_callback(void *data) {

}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Pause Time
	team1Score++;
	team2Score++;
	text_layer_set_text(team1_score_text_layer, convertTeam1Score(team1Score));
	text_layer_set_text(team2_score_text_layer, convertTeam2Score(team2Score));

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	int team = 1;
  ref_calls_window_push(team);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	int team = 2;
  ref_calls_window_push(team);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	gameTime = app_timer_register(1500, timer_callback, NULL);
	static GFont time_font;
	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REGULAR_SANSATION_48));

	window_set_click_config_provider(window, click_config_provider);

  // Create Text Layers
  team1_name_text_layer = text_layer_create((GRect)  { .origin = { bounds.size.w/10,     bounds.size.h/20 },      .size = { bounds.size.w,   25 } });
	team1_score_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10 * 9, bounds.size.h/20 },      .size = { bounds.size.w/4, 25 } });
	team2_name_text_layer = text_layer_create((GRect)  { .origin = { bounds.size.w/10,     bounds.size.h/20 * 16},  .size = { bounds.size.w,   25 } });
	team2_score_text_layer = text_layer_create((GRect) { .origin = { bounds.size.w/10 * 9, bounds.size.h/20 * 16},  .size = { bounds.size.w/4, 25 } });
	gameTime_text_layer = text_layer_create((GRect)    { .origin = { bounds.size.w/7,      bounds.size.h/2 - 13},   .size = { bounds.size.w, bounds.size.h/3} });
	gameHalf_text_layer = text_layer_create((GRect)    { .origin = { bounds.size.w/5,      bounds.size.h/2 - 45},   .size = { bounds.size.w/4, 25 } });

  // Set Font
  text_layer_set_font(team1_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team1_score_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team2_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(team2_score_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(gameTime_text_layer, time_font);
  text_layer_set_font(gameHalf_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  // Set Text Layer Values
  text_layer_set_text(team1_name_text_layer, "Team1");
	text_layer_set_text(team1_score_text_layer, convertTeam1Score(team1Score));
	text_layer_set_text(team2_name_text_layer, "Team2");
	text_layer_set_text(team2_score_text_layer, convertTeam2Score(team2Score));
	text_layer_set_text(gameHalf_text_layer, "1");

	// Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
 	time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

  // Add Text Layer Childs to Window
  layer_add_child(window_layer, text_layer_get_layer(team1_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team1_score_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_name_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(team2_score_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(gameTime_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(gameHalf_text_layer));

	window_set_click_config_provider(window, click_config_provider);
}

static void main_window_unload(Window *window) {
  // Unsubscribe from services
	tick_timer_service_unsubscribe();

	// Destroy Text Layers
  text_layer_destroy(team1_name_text_layer);
	text_layer_destroy(team1_score_text_layer);
	text_layer_destroy(team2_name_text_layer);
	text_layer_destroy(team2_score_text_layer);
	text_layer_destroy(gameTime_text_layer);
	text_layer_destroy(gameHalf_text_layer);
}

static void init(void) {
  // Set Score Values ? Persisent Exist Set Score
  team1Score = persist_exists(TEAM1_PSCORE) ? persist_read_int(TEAM1_PSCORE) : DEFAULT_SCORE;
  team2Score = persist_exists(TEAM2_PSCORE) ? persist_read_int(TEAM2_PSCORE) : DEFAULT_SCORE;

  window = window_create();
  window_set_fullscreen(window, true);
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = main_window_load,
    .unload = main_window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);


  // Fonts
  big_font = fonts_load_custom_font(resource_get_handle(FONT_BIG_TIME));
  seconds_font = fonts_load_custom_font(resource_get_handle(FONT_SECONDS));
  laps_font = fonts_load_custom_font(resource_get_handle(FONT_LAPS));


  // Set up the lap time layers. These will be made visible later.
  for(int i = 0; i < LAP_TIME_SIZE; ++i) {
	   lap_layers[i] = text_layer_create(GRect(-139, 52, 139, 30));

      text_layer_set_background_color(lap_layers[i], GColorClear)
      text_layer_set_font(lap_layers[i], laps_font);
      text_layer_set_text_color(lap_layers[i], GColorWhite);
      text_layer_set_text(lap_layers[i], lap_times[i]);
      layer_add_child(root_layer, (Layer*)lap_layers[i]);
    }

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
