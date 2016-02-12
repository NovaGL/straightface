#include <pebble.h>
#define KEY_COLOR_RED     0
#define KEY_COLOR_GREEN   1
#define KEY_COLOR_BLUE    2
#define KEY_LEADING_ZERO  3
#define KEY_DATE_FORMAT   4
#define KEY_LOCALE        5

#define locale_en 0x0
#define locale_fr 0x1
#define locale_de 0x2
#define locale_es 0x3
#define locale_il 0x3

static Window *s_main_window;
static TextLayer *s_hour_layer,*s_minute_layer,*s_day_layer,*s_month_layer;
static GFont s_hour_font, s_minute_font, s_day_font;
GColor time_color;
static bool leading_zero = 0;
static bool euro_date = 1;
static int red, green, blue, locale;
static int padding = 0;

  char lower_to_upper(char c) {
    return (c>='a' && c<='z')?c&0xdf:c;
  }

  const char *DAYS[5][7] = { 
	{"SUNDAY","MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY"},
	{"DIMANCHE","LUNDI","MARDI","MERCREDI","JEUDI","VENDREDI","SAMEDI"},
  {"SONNTAG","MONTAG","DIENSTAG","MITTWOCH","DONNERSTAG","FREITAG","SAMSTAG"},
	{"DOMINGO","LUNES","MARTES","MIÉRCOLES","JUEVES","VIERNES","SÁBADO"},
	{"DOMENICA","LUNEDÌ","MARTEDÌ","MERCOLEDÌ","GIOVEDÌ","VENERDÌ","SABATO"}
};
// months (en, fr, de, es, it)
  const char *MONTHS[5][12] = { 
 	{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"},
	{"JANVIER","FÉVRIER","MARS","AVRIL","MAI","JUIN","JUILLET","AOÛT","SEPTEMBRE","OCTOBRE","NOVEMBRE","DÉCEMBRE"},
	{"JANUAR","FEBRUAR","MÄRZ","APRIL","MAI","JUNI","JULI","AUGUST","SEPTEMBER","OKTOBER","NOVEMBER","DEZEMBER"},
	{"ENERO","FEBRERO","MARZO","ABRIL","MAYO","JUNIO","JULIO","AGOSTO","SEPTIEMBRE","OCTUBRE","NOVIEMBRE","DICIEMBRE"},
	{"GENNAIO","FEBBRAIO","MARZO","APRILE","MAGGIO","GIUGNO","LUGLIO","AGOSTO","SETTEMBRE","OTTOBRE","NOVEMBRE","DICEMBRE"},
};

static void update_time() {
        // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    // Create a long-lived buffer
    static char bufferhr[] = "00:";
    static char buffermin[3];
    static char daybuffer[10];
    static char monthbuffer[12];
    
    // Write the current hours and minutes into the buffer   
  
  if (leading_zero == 1) {
    if(clock_is_24h_style()) {
      strftime(bufferhr, sizeof(" 0:"), "%k:", tick_time);
    } else {
      strftime(bufferhr, sizeof(" 0:"), "%l:", tick_time);
    }
  } else {
     if(clock_is_24h_style()) {
      strftime(bufferhr, sizeof("00:"), "%H:", tick_time);
    } else {
      strftime(bufferhr, sizeof("00:"), "%I:", tick_time);
    }
  }
    
    strftime(buffermin, sizeof("00"), "%M", tick_time);
     
 
  if (locale != locale_en) {
      snprintf(monthbuffer, sizeof(monthbuffer), "%d %s",  tick_time->tm_mday, MONTHS[locale][tick_time->tm_mon]);
      snprintf(daybuffer, sizeof(daybuffer), "%s",  DAYS[locale][tick_time->tm_wday]);
    } else {
      if (euro_date == 1) { 
        strftime(monthbuffer, sizeof(monthbuffer), "%d %B", tick_time);         
      } else { // If the user has selected the WWW DD MMM date format
        strftime(monthbuffer, sizeof(monthbuffer), "%B %d", tick_time); 
      }
      strftime(daybuffer, sizeof(daybuffer), "%A", tick_time);
  }
  
  
  
    // convert to uppercase
    for(char* pc=daybuffer;*pc!=0;++pc) *pc = lower_to_upper(*pc);
    for(char* pc=monthbuffer;*pc!=0;++pc) *pc = lower_to_upper(*pc);
    
    
    // Display this time on the TextLayer
    text_layer_set_text(s_hour_layer, bufferhr);
    text_layer_set_text(s_minute_layer, buffermin);
    text_layer_set_text(s_day_layer, daybuffer); //display current day
    text_layer_set_text(s_month_layer, monthbuffer); //display current month
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  const int text_height = 52;
    
  s_hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIPLE_BOLD_52));
  
  s_day_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  //Set Day Layer
    s_day_layer = text_layer_create(
    GRect(bounds.origin.x ,bounds.size.h * .10, bounds.size.w, text_height));
    text_layer_set_background_color(s_day_layer, GColorClear);  
    text_layer_set_text_color(s_day_layer, GColorWhite);
    text_layer_set_font(s_day_layer, s_day_font);
    text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_day_layer));  
  
  //Set Month Layer
    s_month_layer = text_layer_create(
    GRect(bounds.origin.x ,bounds.size.h * .70, bounds.size.w, text_height));
    text_layer_set_background_color(s_month_layer, GColorClear);  
    text_layer_set_text_color(s_month_layer, GColorWhite);
    text_layer_set_font(s_month_layer, s_day_font);
    text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_month_layer));  
  
  //Set Hour Layer
    s_hour_layer = text_layer_create(
    GRect(bounds.origin.x,bounds.size.h * .3, (bounds.size.w/2)+3, text_height));
    text_layer_set_background_color(s_hour_layer, GColorClear);  
    text_layer_set_text_color(s_hour_layer, GColorWhite);
    text_layer_set_font(s_hour_layer, s_hour_font);
    text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));
  
  //Set Minute Layer
    s_minute_layer = text_layer_create(
    GRect((bounds.size.w/2)+3, bounds.size.h * .3, bounds.size.w, text_height));
    text_layer_set_background_color(s_minute_layer, GColorClear);  
    #if defined(PBL_BW)
      text_layer_set_text_color(s_minute_layer, GColorWhite);
    #elif defined(PBL_COLOR)
      // Use background color setting
      red = persist_read_int(KEY_COLOR_RED);
      green = persist_read_int(KEY_COLOR_GREEN);
      blue = persist_read_int(KEY_COLOR_BLUE);
      time_color = GColorFromRGB(red, green, blue);   
      if (!red || !green || !blue) {
        time_color =  GColorFromRGB(255, 170, 0);
      }
      text_layer_set_text_color(s_minute_layer, time_color);
     #endif
    text_layer_set_text(s_minute_layer, "00:");
    text_layer_set_font(s_minute_layer, s_hour_font);
    text_layer_set_text_alignment(s_minute_layer, GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));
    
  // Ensure time display is not blank on launch
  update_time();
}


void in_received_handler(DictionaryIterator *received, void *context) {
  
    ////// Colour Scheme
    Tuple *color_red_t    = dict_find(received, KEY_COLOR_RED);
    Tuple *color_green_t  = dict_find(received, KEY_COLOR_GREEN);
    Tuple *color_blue_t   = dict_find(received, KEY_COLOR_BLUE);
  
    Tuple *leading_zero_t = dict_find(received, KEY_LEADING_ZERO);
    Tuple *date_tuple = dict_find(received, KEY_DATE_FORMAT); 
    Tuple *locale_tuple = dict_find(received, KEY_LOCALE);
  
  if(color_red_t && color_green_t && color_blue_t) {
    // Apply the color if available
      APP_LOG(APP_LOG_LEVEL_INFO, "Set time font color");
    #if defined(PBL_BW)
      text_layer_set_text_color(s_minute_layer, GColorWhite);
    #elif defined(PBL_COLOR)
     red = color_red_t->value->int32;
     green = color_green_t->value->int32;
     blue = color_blue_t->value->int32;
    // Persist values
      persist_write_int(KEY_COLOR_RED, red);
      persist_write_int(KEY_COLOR_GREEN, green);
      persist_write_int(KEY_COLOR_BLUE, blue);
      time_color = GColorFromRGB(red, green, blue);      
      text_layer_set_text_color(s_minute_layer, time_color);      
    #endif
  }  

  //Hide Leading Zero
   if (leading_zero_t) {
  	if (strcmp(leading_zero_t->value->cstring, "on") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Hide Leading Zero");
  		leading_zero = 1;
      padding = 5;
  	} else {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Show Leading Zero");
  		leading_zero = 0;
      padding = 0;
  	}     
   }
  
   if (locale_tuple)
  {
    app_log(APP_LOG_LEVEL_DEBUG,
            __FILE__,
            __LINE__,
            "locale=%s",
            locale_tuple->value->cstring);
      
    if (strcmp(locale_tuple->value->cstring, "fr") == 0)
      {
        APP_LOG(APP_LOG_LEVEL_INFO, "FR Language received!");
        persist_write_int(KEY_LOCALE, locale_fr);
        locale = locale_fr;
      }
    else if (strcmp(locale_tuple->value->cstring, "de") == 0)
      {
        APP_LOG(APP_LOG_LEVEL_INFO, "DE Language received!");
        persist_write_int(KEY_LOCALE, locale_de);
        locale = locale_de;
      }
    else if (strcmp(locale_tuple->value->cstring, "es") == 0)
      {
        APP_LOG(APP_LOG_LEVEL_INFO, "ES Language received!");
        persist_write_int(KEY_LOCALE, locale_es);
        locale = locale_es;
      }
   else if (strcmp(locale_tuple->value->cstring, "il") == 0)
      {
        APP_LOG(APP_LOG_LEVEL_INFO, "IL Language received!");
        persist_write_int(KEY_LOCALE, locale_il);
        locale = locale_il;
      }
    else
      {
        APP_LOG(APP_LOG_LEVEL_INFO, "EN Language received!");
        persist_write_int(KEY_LOCALE, locale_en);
        locale = locale_en;
      }
  }
  
  // Set date format either MM DD or DD MM
   if (date_tuple) {
  	if (strcmp(date_tuple->value->cstring, "edate") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using European date");
  		euro_date = 1;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	} else {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using US date");
  		euro_date = 0;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	}     
   }
  
  update_time();
  
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_hour_font);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);

  //register for messages
  app_message_register_inbox_received(in_received_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
    
  app_event_loop();
  deinit();
}
