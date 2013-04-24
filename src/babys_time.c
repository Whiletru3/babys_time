#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x99, 0xA0, 0x30, 0xB3, 0xF1, 0x80, 0x42, 0x7F, 0x9F, 0x91, 0x5E, 0xD1, 0x09, 0x2F, 0x86, 0xF5 }
PBL_APP_INFO(MY_UUID,
             "Baby's Time", "Whiletrue",
             0x2, 0, /* App version */
             INVALID_RESOURCE,
             APP_INFO_WATCH_FACE);

// App-specific data
Window window; // All apps must have at least one window

TextLayer timeLayer; // The clock
TextLayer countdownLayer; // The countdown
TextLayer textLayer; // The countdown

char countText[] = "000 days";
char descText[] = "til baby";

// transforme un entier en char* (8 caracteres max)
void itoa2(int num, char* buffer) {
    const char digits[10] = "0123456789";
    int iSize;
    
    if(num > 999) {
        buffer[0] = '9';
        buffer[1] = '9';
        buffer[2] = '9';
		iSize = 3;
        
    } else if(num > 99) {
        
        buffer[0] = digits[num / 100];
        if(num%100 > 9) {
            buffer[1] = digits[(num%100) / 10];
        } else {
            buffer[1] = '0';
        }
        buffer[2] = digits[num%10];
        iSize = 3;
    }
    
    else if(num > 9) {
        buffer[0] = digits[num / 10];
        buffer[1] = digits[num % 10];
        iSize = 2;
        buffer[7] = ' ';
    } else {
        buffer[0] = digits[num % 10];
        iSize = 1;
        buffer[6] = ' ';
        buffer[7] = ' ';
    }
    
    buffer[iSize] = ' ';
    buffer[iSize+1] = 'd';
    buffer[iSize+2] = 'a';
    buffer[iSize+3] = 'y';
    if(num < 2)
    	{
    	buffer[iSize+4] = ' ';
    	}
    else
    	{
    	buffer[iSize+4] = 's';
    	}
    		
}


// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
  static char countText[] = "000 days";
    
  PblTm currentTime;
  int i3Jul = 183; // 03 july
  
  get_time(&currentTime);
	int iyDay = currentTime.tm_yday;
	int iNumDays = i3Jul - iyDay;
	if(iNumDays < 0)
		{
		iNumDays = 0;
		}
		
	itoa2(iNumDays, &countText[0]);
	text_layer_set_text(&countdownLayer, countText);
    text_layer_set_text(&textLayer, descText);
	  
  char *time_format;  
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(timeText, sizeof(timeText), time_format, &currentTime);

  text_layer_set_text(&timeLayer, timeText);

    
}


// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {
    
  // Create our app's base window
  window_init(&window, "Baby's Time");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  // Init the text layer used to show the time
  text_layer_init(&timeLayer, GRect(1, 5, 144-1 /* width */, 168-20/* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  
  // countdown
    text_layer_init(&countdownLayer, GRect(1, 65, 144-1 /* width */, 168-20/* height */));
    text_layer_set_text_color(&countdownLayer, GColorWhite);
    text_layer_set_background_color(&countdownLayer, GColorClear);
    text_layer_set_font(&countdownLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
    text_layer_set_text(&countdownLayer, countText);
    layer_add_child(&window.layer, &countdownLayer.layer); 
  // til baby
    text_layer_init(&textLayer, GRect(1, 100, 144-1 /* width */, 168-20/* height */));
    text_layer_set_text_color(&textLayer, GColorWhite);
    text_layer_set_background_color(&textLayer, GColorClear);
    text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
    text_layer_set_text(&textLayer, descText);
    layer_add_child(&window.layer, &textLayer.layer);
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(app_ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);
}


// The main event/run loop for the watchface
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init_app,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
