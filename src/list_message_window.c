/**
 * Example implementation of the list menu UI pattern.
 */

#include "list_message_window.h"

static Window *main_window;
static MenuLayer *menu_layer;
static TextLayer *list_message_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return LIST_MESSAGE_WINDOW_NUM_ROWS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  static char s_buff[16];
  snprintf(s_buff, sizeof(s_buff), "Item %d", (int)cell_index->row);
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    LIST_MESSAGE_WINDOW_CELL_HEIGHT);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, LIST_MESSAGE_WINDOW_MENU_HEIGHT));
  menu_layer_set_click_config_onto_window(menu_layer, window);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

  const GEdgeInsets message_insets = {.top = 140};
  list_message_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(list_message_layer, GTextAlignmentCenter);
  text_layer_set_text(list_message_layer, LIST_MESSAGE_WINDOW_HINT_TEXT);
  layer_add_child(window_layer, text_layer_get_layer(list_message_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(menu_layer);
  text_layer_destroy(list_message_layer);

  window_destroy(window);
  main_window = NULL;
}

void list_message_window_push() {
  if(!main_window) {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(main_window, true);
} 