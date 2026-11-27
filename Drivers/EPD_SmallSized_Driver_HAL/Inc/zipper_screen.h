/*
Driver for E-ink display screen on smart zipper
Created by Isaac Rounds
*/


/* ---- Includes ---- */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "epd_driver.h"
#include "epd_screens.h"

/* ---- Enums/Defines ---- */

/* ---- Global Variables ---- */


/* ---- Public Function Declerations ---- */

void initialize_screen(SPI_HandleTypeDef *spi_handle);
void update_altitude(int32_t current_alt);
void update_max_alt(int32_t max_alt);
void update_temp(int16_t current_temp);
void update_min_temp(int16_t min_temp);
void update_max_temp(int16_t max_temp);
void global_update();
void power_off_cog();
void seal();
void write_main_alt(int32_t altitude, bool refresh_alt);


