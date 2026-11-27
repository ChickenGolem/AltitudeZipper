/*/*
Driver for E-ink display screen on smart zipper
Created by Isaac Rounds
*/


/* ---- Includes ---- */
#include "zipper_screen.h"
#include "../../Images/fastupdate_src/FuPu_Data.h"
#include "../../Images/globalupdate_src/demoImageData.h"
#include "../../Images/Fonts/MainAltitude/MainAltitudeFont.h"
#include "../../Images/Fonts/MaximumAltitude/MaximumAltitude.h"
/* ---- Enums/Declares ---- */

#define EPD_WIDTH   200
#define EPD_HEIGHT  96
#define EPD_BUF_SIZE (EPD_WIDTH * EPD_HEIGHT / 8) // 2400 bytes

struct font_package_t {
	uint8_t height_px;
	uint8_t width_px;
	const uint8_t meter_symbol_height_px;
	const uint8_t meter_symbol_width_px;
	const uint8_t * const meter_bmp;
	const uint8_t foot_symbol_height_px;
	const uint8_t foot_symbol_width_px;
	const uint8_t * const foot_bmp;
	const uint8_t degree_symbol_height_px;
	const uint8_t degree_symbol_width_px;
	const uint8_t * const degree_bmp;
	const uint8_t * const negative_bmp;
	const uint8_t * const *font_link_list;
};

struct screen_state_t {
	int32_t altitude;
	int32_t max_altitude;
	int16_t temperature;
	int16_t max_temperature;
	int16_t min_temperature;
	uint8_t writes_since_global_refresh;
	bool is_imperial;
};

typedef enum {
	SLOT_BLANK = 0,
	SLOT_MINUS,
	SLOT_DIGIT
} alt_slot_type_t;

struct write_data {
	struct font_package_t font_package;
	uint8_t char_0_h_cord;
	uint8_t char_0_w_cord;
	bool unit_is_temp;
	int32_t value_to_write;
	int32_t old_value;
};

const uint8_t * const MainAltitude_digits[10] = {
    MainAltitude_0mainA,
    MainAltitude_1mainA,
    MainAltitude_2mainA,
    MainAltitude_3mainA,
    MainAltitude_4mainA,
    MainAltitude_5mainA,
    MainAltitude_6mainA,
    MainAltitude_7mainA,
    MainAltitude_8mainA,
    MainAltitude_9mainA,
};

const uint8_t * const SubAltitude_digits[10] = {
	_0SubA,
	_1SubA,
	_2SubA,
	_3SubA,
	_4SubA,
	_5SubA,
	_6SubA,
	_7SubA,
	_8SubA,
	_9SubA	
};

/* ---- Global variables---- */

struct font_package_t SubAltitude_font = {
	.height_px = 14,
	.width_px = 14,
	.meter_symbol_height_px = 1,
	.meter_symbol_width_px = 1,
	.meter_bmp = SubAltitude_meterSymbol,
	.foot_symbol_height_px = 1,
	.foot_symbol_width_px = 1,
	.foot_bmp = SubAltitude_footSymbol,
	.degree_symbol_height_px = 3,
	.degree_symbol_width_px = 5,
	.degree_bmp = SubAltitude_degreeSymbol, // this actually links to junk, not used
	.negative_bmp = SubAltitude_negativeSymbol,
	.font_link_list = SubAltitude_digits
};

struct font_package_t MainAltitude_font = {
	.height_px = 30,
	.width_px = 35,
	.meter_symbol_height_px = 17,
	.meter_symbol_width_px = 11,
	.meter_bmp = MainAltitude_meterSymbol,
	.foot_symbol_height_px = 5,
	.foot_symbol_width_px = 11,
	.foot_bmp = MainAltitude_footSymbol,
	.degree_symbol_height_px = 7,
	.degree_symbol_width_px = 7,
	.degree_bmp = MainAltitude_degreeSymbol, // this actually links to junk, not used
	.negative_bmp = MainAltitude_negativeSymbol,
	.font_link_list = MainAltitude_digits
};

struct screen_state_t screen_state = {
	.altitude = 0,
	.max_altitude = 0,
	.temperature = 0,
	.max_temperature = 0,
	.min_temperature = 0,
	.writes_since_global_refresh = 0,
	.is_imperial = true
};
uint8_t frameBuf[EPD_BUF_SIZE] = {0};
ePaperDisplay_t epd;

/* ---- Private Function Declares ---- */
void update_frame_buff(const uint8_t *digitBmp, uint8_t x, uint8_t y, uint8_t bmp_width, uint8_t bmp_height);
void write_over_value(struct write_data data, bool refresh);
/* ---- Public Function Definitions ---- */

void initialize_screen(SPI_HandleTypeDef *spi_handle)
{
	  epd.spi = spi_handle;

	  // CubeMX pin and port constants
	  epd.panelBusy.pin = EPD_BUSY_Pin;
	  epd.panelBusy.port = EPD_BUSY_GPIO_Port;
	  epd.panelCS.pin = EPD_CS_Pin;
	  epd.panelCS.port = EPD_CS_GPIO_Port;
	  epd.panelDC.pin = EPD_DC_Pin;
	  epd.panelDC.port = EPD_DC_GPIO_Port;
	  epd.panelReset.pin = EPD_RST_Pin;
	  epd.panelReset.port = EPD_RST_GPIO_Port;

	  // EPD type and PSR constants for each screen
	  epd.epdType = EPD_DISPLAY_097;
	  epd.displaySizeH = EPD_DISPLAY_097_SIZE_H;
	  epd.displaySizeV = EPD_DISPLAY_097_SIZE_V;

	  epd.epdPSR[0] = EPD_DISPLAY_097_PSR0;
	  epd.epdPSR[1] = EPD_DISPLAY_097_PSR1;


	  initializeGlobalUpdateCOG(&epd);
	  globalUpdateDisplay(&epd, BW_0x00Buffer, BW_0x00Buffer);
	  powerOffCOG(&epd);
}

void global_update(){ //this should be changed to 'redraw' the screen from the wiped screen
	initializeGlobalUpdateCOG(&epd);
	globalUpdateDisplay(&epd, frameBuf, frameBuf);
	powerOffCOG(&epd);
}

void seal()
{
	  initializeFastUpdateCOG(&epd);
	  fastUpdateDisplay(&epd, frameBuf, HUD_v2_Im);
	  memcpy(frameBuf, HUD_v2_Im, EPD_BUF_SIZE);
	  powerOffCOG(&epd);
}



void write_main_alt(int32_t altitude, bool refresh_alt)
{
	struct write_data main_alt_write_data = {
		.font_package = MainAltitude_font,
		.char_0_h_cord = screen_state.is_imperial ? 45 : 50,
		.char_0_w_cord =5,
		.unit_is_temp = false,
		.value_to_write = altitude,
		.old_value = screen_state.altitude
	};
	write_over_value(main_alt_write_data, refresh_alt);
	screen_state.altitude = altitude;
}

void write_sub_alt(int32_t sub_altitude, bool refresh_sub_alt)
{
	
	//x and y cords to place the top 'left' (screen direction) of x
	uint8_t max_x = 44;
	uint8_t max_y = 118;
	uint8_t max_height = 40; //length of the string 'Max'
	uint8_t space_between_col_and_max = 7;

	struct write_data sub_alt_write_data = {
			.font_package = SubAltitude_font,
			.char_0_h_cord = max_y - (5 * SubAltitude_font.width_px+(space_between_col_and_max*2)), //Needs work
			.char_0_w_cord = max_x,
			.unit_is_temp = false,
			.value_to_write = sub_altitude,
			.old_value = screen_state.max_altitude
		};

	//writes 'Max :' to screen
	if (refresh_sub_alt)
	{
		initializeFastUpdateCOG(&epd);
		fastUpdateDisplay_blit(&epd, frameBuf, BW_0x00Buffer, max_x, max_y, sub_alt_write_data.font_package.width_px, max_height);
		fastUpdateDisplay_blit(&epd, frameBuf, Max_subA, max_x, max_y, sub_alt_write_data.font_package.width_px, max_height);
		fastUpdateDisplay_blit(&epd, frameBuf, COLON_SubA, max_x+4, max_y - space_between_col_and_max, 10, 2);
		powerOffCOG(&epd);
	}

	write_over_value(sub_alt_write_data, refresh_sub_alt);
	screen_state.max_altitude = sub_altitude;
}

void write_over_value(struct write_data data, bool refresh)
{
	if ((data.value_to_write < 0) != (data.old_value < 0))
	{
		refresh = true;
	}

	initializeFastUpdateCOG(&epd);

	if (refresh)
	{
		// Clear full area
		fastUpdateDisplay_blit(&epd, frameBuf, BW_0x00Buffer, data.char_0_w_cord, data.char_0_h_cord, data.font_package.width_px, data.font_package.height_px * 5);

		if (screen_state.is_imperial) //(data.font_package.foot_symbol_width_px/2)
		{
			fastUpdateDisplay_blit(&epd, frameBuf, data.font_package.foot_bmp, data.char_0_w_cord - 5, data.char_0_h_cord - (data.font_package.foot_symbol_height_px/2), data.font_package.foot_symbol_width_px, data.font_package.foot_symbol_height_px);
		}
		else
		{
			fastUpdateDisplay_blit(&epd, frameBuf, data.font_package.meter_bmp, data.char_0_w_cord + data.font_package.width_px - data.font_package.meter_symbol_width_px, data.char_0_h_cord - (data.font_package.meter_symbol_height_px), data.font_package.meter_symbol_width_px, data.font_package.meter_symbol_height_px);
		}
	}

	int32_t old_val = abs(data.old_value);
	int32_t new_val = abs(data.value_to_write);
	bool new_is_neg = (data.value_to_write < 0);
	bool old_is_neg = (data.old_value < 0);

	bool new_minus_placed = false;
	bool old_minus_placed = false;
	uint8_t position = 0;

	while (position < 5)
	{
		uint16_t y =  data.char_0_h_cord + (data.font_package.height_px * position);

		// Determine what is CURRENTLY in this slot on screen
		alt_slot_type_t old_type = SLOT_BLANK;
		uint8_t old_digit = 0;
		if (!refresh)
		{
			if (position == 0 || old_val > 0)
			{
				old_type = SLOT_DIGIT;
				old_digit = old_val % 10;
			}
			else if (old_is_neg && !old_minus_placed)
			{
				old_type = SLOT_MINUS;
				old_minus_placed = true;
			}
		}

		// what SHOULD be in this slot now
		alt_slot_type_t new_type = SLOT_BLANK;
		uint8_t new_digit = 0;
		if (position == 0 || new_val > 0)
		{
			new_type = SLOT_DIGIT;
			new_digit = new_val % 10;
		}
		else if (new_is_neg && !new_minus_placed)
		{
			new_type = SLOT_MINUS;
			new_minus_placed = true;
		}

		//update the display if this slot actually changed
		if (new_type != old_type || (new_type == SLOT_DIGIT && new_digit != old_digit))
		{
			// Erase old symbol if switching between digit (x=10) and minus (x=5), or clearing to blank
			if (!refresh && old_type != SLOT_BLANK && !(old_type == SLOT_DIGIT && new_type == SLOT_DIGIT))
			{
				fastUpdateDisplay_blit(&epd, frameBuf, BW_0x00Buffer, data.char_0_w_cord, y, data.font_package.width_px, data.font_package.height_px);
			}

			if (new_type == SLOT_DIGIT)
			{
				fastUpdateDisplay_blit(&epd, frameBuf, data.font_package.font_link_list[new_digit], data.char_0_w_cord, y, data.font_package.width_px, data.font_package.height_px);
			}
			else if (new_type == SLOT_MINUS)
			{
				fastUpdateDisplay_blit(&epd, frameBuf, data.font_package.negative_bmp, data.char_0_w_cord, y, data.font_package.width_px, data.font_package.height_px);
			}
		}

		// Stop early once both old and new representations are completely finished
		if (new_type == SLOT_BLANK && old_type == SLOT_BLANK)
		{
			break;
		}

		new_val /= 10;
		old_val /= 10;
		position++;
	}

	powerOffCOG(&epd);
}


/* ---- Private Functions ---- */


