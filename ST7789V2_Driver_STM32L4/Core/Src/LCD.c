#include "LCD.h"


// Image buffer storing pixel data, 4 pixels per byte (2 bits per pixel)
static uint8_t image_buffer[BUFFER_LENGTH];

// Tracks which rows have changed and need to be refreshed
static uint8_t track_changes[ST7789V2_HEIGHT]; 

// Define multiple palettes. These must be kept in sync with the LCD_Palette enum
// and the LCD_Set_Palette function.
// Each palette is an array of 16 RGB565 colour values.
static const uint16_t palette_default[16] = {
    LCD_COLOUR_0,  LCD_COLOUR_1,  LCD_COLOUR_2,  LCD_COLOUR_3,
    LCD_COLOUR_4,  LCD_COLOUR_5,  LCD_COLOUR_6,  LCD_COLOUR_7,
    LCD_COLOUR_8,  LCD_COLOUR_9,  LCD_COLOUR_10, LCD_COLOUR_11,
    LCD_COLOUR_12, LCD_COLOUR_13, LCD_COLOUR_14, LCD_COLOUR_15
};

static const uint16_t palette_greyscale[16] = {
    RGB565_GREY_0,  RGB565_GREY_1,  RGB565_GREY_2,  RGB565_GREY_3,
    RGB565_GREY_4,  RGB565_GREY_5,  RGB565_GREY_6,  RGB565_GREY_7,
    RGB565_GREY_8,  RGB565_GREY_9,  RGB565_GREY_10, RGB565_GREY_11,
    RGB565_GREY_12, RGB565_GREY_13, RGB565_GREY_14, RGB565_GREY_15
};

static const uint16_t palette_vintage[16] = {
    RGB565_VINTAGE_0,  RGB565_VINTAGE_1,  RGB565_VINTAGE_2,  RGB565_VINTAGE_3,
    RGB565_VINTAGE_4,  RGB565_VINTAGE_5,  RGB565_VINTAGE_6,  RGB565_VINTAGE_7,
    RGB565_VINTAGE_8,  RGB565_VINTAGE_9,  RGB565_VINTAGE_10, RGB565_VINTAGE_11,
    RGB565_VINTAGE_12, RGB565_VINTAGE_13, RGB565_VINTAGE_14, RGB565_VINTAGE_15
};

// Cozy Miner custom palette.
// Values are RGB565 little-endian values for the ST7789V2 display.
static const uint16_t palette_custom[16] = {
    0xA218, 0x5AF7, 0xC7C1, 0x475C,   //  0 black   1 cream   2 red      3 leaf green
    0x1433, 0x48EC, 0xA9ED, 0x17F5,   //  4 denim   5 orange  6 amber    7 pink
    0x8E49, 0x2339, 0x07ED, 0xCB8D,   //  8 purple  9 drkbrwn 10 gold    11 sage
    0x457A, 0x1484, 0x1C76, 0xCDE3,   // 12 brown   13 stone  14 sky     15 coral
};

/*
 * Farm unified palette.
 *
 * Index usage:
 *  0 black / outline
 *  1 cream
 *  2 soft red
 *  3 leaf green
 *  4 denim blue
 *  5 warm orange
 *  6 soil brown
 *  7 dry soil
 *  8 honey gold
 *  9 panel blue
 * 10 stress orange
 * 11 grass green
 * 12 night blue
 * 13 stone grey
 * 14 water cyan
 * 15 sky cyan
 */
 
 static const uint16_t palette_farm_unified[16] = {
  /* 0  black / outline          */ 0xA218,
  /* 1  cream                    */ 0x5AF7,
  /* 2  soft red / dead tile     */ 0x26B9,
  /* 3  leaf green / dry crop    */ 0x475C,
  /* 4  denim blue               */ 0x1433,
  /* 5  warm orange              */ 0x48EC,
  /* 6  soil brown / empty tile  */ 0x457A,
  /* 7  dry soil / seeded tile   */ 0x2339,
  /* 8  honey gold / mature tile */ 0x07ED,
  /* 9  panel blue / HUD         */ 0x492A,
  /* 10 coral / stress orange    */ 0xCDE3,
  /* 11 grass green              */ 0xCB8D,
  /* 12 night blue               */ 0x2619,
  /* 13 stone grey / tile border */ 0x1484,
  /* 14 water cyan / watered     */ 0x1E35,
  /* 15 sky cyan                 */ 0xFDB6,
};

// Active palette pointer
static const uint16_t *colour_map = palette_default;

void LCD_init(ST7789V2_cfg_t* cfg) {
  ST7789V2_Init(cfg);
}

void LCD_turnOff(ST7789V2_cfg_t* cfg) {
  // Backlight off
  gpio_write(cfg->BL, 0);

  // Display off
  ST7789V2_Send_Command(cfg, ST7789_DISPOFF);
}

void LCD_turnOn(ST7789V2_cfg_t* cfg) {
  // Backlight on
  gpio_write(cfg->BL, 1);

  // Display on
  ST7789V2_Send_Command(cfg, ST7789_DISPON);
}

void LCD_clear() {
  // Mark all rows as changed before clearing the frame buffer
  for (int y = 0; y < ST7789V2_HEIGHT; y++) {
    track_changes[y] = 1;
  }

  // Clear the frame buffer 32 bits at a time
  for (int i = 0; i < BUFFER_LENGTH >> 2; i++) {
    ((uint32_t*)image_buffer)[i] = 0;
  }
}

void LCD_Set_Palette(LCD_Palette palette) {
  switch(palette) {
    case PALETTE_GREYSCALE:
      colour_map = palette_greyscale;
      break;
    case PALETTE_VINTAGE:
      colour_map = palette_vintage;
      break;
    case PALETTE_CUSTOM:
      colour_map = palette_custom;
      break;
    case PALETTE_FARM_UNIFIED:
      colour_map = palette_farm_unified;
      break;
    case PALETTE_DEFAULT:
    default:
      colour_map = palette_default;
      break;
  }

  // Mark all rows as changed to force a full refresh
  for (int y = 0; y < ST7789V2_HEIGHT; y++) {
    track_changes[y] = 1;
  }
}

void LCD_normalMode(ST7789V2_cfg_t* cfg) {
  ST7789V2_Send_Command(cfg, ST7789_INVON);
}

void LCD_inverseMode(ST7789V2_cfg_t* cfg) {
  ST7789V2_Send_Command(cfg, ST7789_INVOFF);
}

void LCD_printString(char const *str, const uint16_t x, const uint16_t y, uint8_t colour, uint8_t font_size) {
  if (x < ST7789V2_WIDTH && y < ST7789V2_HEIGHT) {
    int n = 0 ; // Counter for number of characters in string

    // Loop through the string and draw each character into the frame buffer
    while(*str) {
      for (int i = 0; i < 5 ; i++ ) {
        int pixel_x = x+(i+n*6)*font_size;
        if (pixel_x > ST7789V2_WIDTH-1) // Ensure the pixel is inside the display width
          break;
        for (int j = 0; j < 7; j ++) {
          if (font5x7_[(*str - 32)*5 + i] & (1u << j)) {
            for (int l = 0; l < font_size; l++) {
              for (int m = 0; m < font_size; m++) {
                LCD_Set_Pixel(pixel_x+l, y+(j*font_size)+m, colour);
              }
            }
            
          }
        }
      }
      str++; // Go to next character in string
      n++;   // Increment character index
    }
  }
}

void LCD_printChar(char const c, const uint16_t x, const uint16_t y, uint8_t colour) {
  if (x < ST7789V2_WIDTH && y < ST7789V2_HEIGHT) {
    for (int i = 0; i < 5 ; i++ ) {
      int pixel_x = x+i;
      if (pixel_x > ST7789V2_WIDTH-1) // Ensure the pixel is inside the display width
        break;
      for (int j = 0; j < 7; j++) {
        if (font5x7_[(c - 32)*5 + i] & (1u << j)) {
          LCD_Set_Pixel(pixel_x, y+j, colour);
        }
      }
    }
  }
}

void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint8_t colour) {
  track_changes[y] = 1;
  uint16_t index = (ST7789V2_WIDTH*y + x) >> 1;  // Bit shift instead of divide by 2
  if (x < ST7789V2_WIDTH && y < ST7789V2_HEIGHT) {
    if (x&1) {
      image_buffer[index] = (colour << 4) | (image_buffer[index] & 0x0F);
    }
    else {
      image_buffer[index] = colour | (image_buffer[index] & 0xF0);
    }
  }
}

uint8_t LCD_Get_Pixel(const uint16_t x, const uint16_t y) {
  uint16_t pixel = x * y;
  if (pixel & 0x1) {
    return (image_buffer[pixel >> 1] & 0xF0) >> 4;
  }
  else {
    return (image_buffer[pixel >> 1] & 0x0F);
  }
}

void LCD_Fill_Buffer(const uint8_t colour) {
  for (int y = 0; y < ST7789V2_HEIGHT; y++) {
    track_changes[y] = 1;
  }
  for (int i = 0; i < BUFFER_LENGTH; i++) {
    image_buffer[i] = colour | (colour << 4);
  }
}

#define lines_per_buffer 1
static uint16_t line_buffer0[lines_per_buffer*240]; // 240 * 2 Bytes * n rows
static uint16_t line_buffer1[lines_per_buffer*240]; // 240 * 2 Bytes * n rows

void LCD_Refresh(ST7789V2_cfg_t* cfg) {
  ST7789V2_Set_Address_Window(cfg, 0, 0, 239, 239); 
  ST7789V2_Send_Command(cfg, 0x2C);

  int buf = 0;
  const int loop_count = 120;  // Pre-calculated value for 240 rows
  const int pixels_per_line = 120 * lines_per_buffer;
  
  for (int i = 0; i < loop_count; i++) {
    // First line buffer
    if (track_changes[2*i]) {
      if (!buf) {
        while (cfg->spi->SR & SPI_SR_BSY);
      }
      buf = 0;
      track_changes[2*i] = 0;
      for (int j = 0; j < 120*lines_per_buffer; j++) {
        uint8_t double_pixel = image_buffer[120 * (2*i*lines_per_buffer) + j];
        line_buffer0[2*j] = colour_map[double_pixel & 0x0F];
        line_buffer0[2*j+1] = colour_map[double_pixel >> 4];
      }
      
      ST7789V2_Set_Address_Window(cfg, 0, 2*i, 239, 2*i); 
      ST7789V2_Send_Command(cfg, 0x2C);
      ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer0, 480*lines_per_buffer);
    }

    // Second line buffer
    if (track_changes[2*i + 1]) {
      if (buf) {
        while (cfg->spi->SR & SPI_SR_BSY);
      }
      buf = 1;
      track_changes[2*i + 1] = 0;
      
      // Pre-calculate buffer offset to avoid repeated multiplication
      const int buffer_offset = 120 * (2*i*lines_per_buffer + 1);
      for (int j = 0; j < pixels_per_line; j++) {
        uint8_t double_pixel = image_buffer[buffer_offset + j];
        line_buffer1[2*j] = colour_map[double_pixel & 0x0F];
        line_buffer1[2*j+1] = colour_map[double_pixel >> 4];
      }
      ST7789V2_Set_Address_Window(cfg, 0, 2*i+1, 239, 2*i+1); 
      ST7789V2_Send_Command(cfg, 0x2C);
      ST7789V2_Send_Data_Block(cfg, (uint8_t*) line_buffer1, 480*lines_per_buffer);
    }
  }
}

void LCD_randomiseBuffer() {
  for(int i = 0; i < BUFFER_LENGTH; i++) {
    image_buffer[i] = (uint8_t)rand();  // Cast truncates to byte
  }
}

void LCD_plotArray(float const array[], const uint8_t colour) {
  for (int i=0; i<ST7789V2_WIDTH; i++) {  // Loop through array
    // Elements are normalised from 0.0 to 1.0, then mapped to display height
    LCD_Set_Pixel(i, ST7789V2_HEIGHT - (int)(array[i]*(float)ST7789V2_HEIGHT), colour);
  }
}

void LCD_Draw_Circle(const uint16_t x0, const uint16_t y0, const uint16_t radius, const uint8_t colour, const uint8_t fill) {

  // Midpoint circle algorithm
  int x = radius;
  int y = 0;
  int radiusError = 1-x;

  while(x >= y) {

    // Draw circle outline
    if (!fill) {
      LCD_Set_Pixel( x + x0,  y + y0, colour);
      LCD_Set_Pixel(-x + x0,  y + y0, colour);
      LCD_Set_Pixel( y + x0,  x + y0, colour);
      LCD_Set_Pixel(-y + x0,  x + y0, colour);
      LCD_Set_Pixel(-y + x0, -x + y0, colour);
      LCD_Set_Pixel( y + x0, -x + y0, colour);
      LCD_Set_Pixel( x + x0, -y + y0, colour);
      LCD_Set_Pixel(-x + x0, -y + y0, colour);
    } 
    else {  
      // Draw a filled circle by connecting points with horizontal lines
      LCD_Draw_Line(x+x0,y+y0, -x+x0,y+y0, colour);
      LCD_Draw_Line(y+x0,x+y0, -y+x0,x+y0, colour);
      LCD_Draw_Line(y+x0,-x+y0, -y+x0,-x+y0, colour);
      LCD_Draw_Line(x+x0,-y+y0, -x+x0,-y+y0, colour);
    }

    y++;
    if (radiusError<0) {
      radiusError += 2 * y + 1;
    } 
    else {
      x--;
      radiusError += 2 * (y - x) + 1;
    }
  }
}

void LCD_Draw_Line(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint8_t colour) {
  // Convert coordinates to signed values because ranges can be negative
  const int16_t y_range = (int)y1 - (int)y0;
  const int16_t x_range = (int)x1 - (int)x0;;

  // Handle single point case
  if (x_range == 0 && y_range == 0) {
    LCD_Set_Pixel(x0, y0, colour);
    return;
  }

  uint16_t step = 1;

  // Iterate across the larger axis so the line has enough pixels
  if ( abs(x_range) > abs(y_range) ) {
    // Loop from the smaller X value to the larger X value
    const uint16_t start = x_range > 0 ? x0:x1;
    const uint16_t stop =  x_range > 0 ? x1:x0;

    // Draw pixels using linear interpolation
    for (unsigned int x = start; x<= stop ; x+=step) {
        const int16_t dx = (int)x - (int)x0;
        const uint16_t y = y0 + y_range * dx / x_range;

        LCD_Set_Pixel(x, y, colour);
    }
  } else {
    // Loop from the smaller Y value to the larger Y value
    const uint16_t start = y_range > 0 ? y0:y1;
    const uint16_t stop =  y_range > 0 ? y1:y0;

    // Draw pixels using linear interpolation
    for (unsigned int y = start; y<= stop ; y+=step) {
        const int16_t dy = (int)y - (int)y0;;
        const uint16_t x = x0 + x_range * dy / y_range;

        LCD_Set_Pixel(x, y, colour);
    }
  }
}

void LCD_Draw_Rect(const uint16_t x0, const uint16_t y0, const uint16_t width, const uint16_t height, const uint8_t colour, const uint8_t fill) {
    if (fill) {
        for (int y = y0; y<y0+height; y++) {
            LCD_Draw_Line(x0, y, x0+(width-1), y, colour);
        }
    }
    else {
        LCD_Draw_Line(x0, y0, x0+(width-1), y0, colour);
        LCD_Draw_Line(x0, y0+(height-1), x0+(width-1), y0+(height-1), colour);
        LCD_Draw_Line(x0, y0, x0, y0+(height-1), colour);
        LCD_Draw_Line(x0+(width-1), y0, x0+(width-1), y0+(height-1), colour);
    }
}

void LCD_Draw_Sprite_Scaled(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t scale){
  if (scale == 0) {
    return;
  }

  for (int i = 0; i < nrows; i++) {
    for (int j = 0 ; j < ncols ; j++) {
      const int pixel = *((sprite + i * ncols) + j);
      if (pixel != 255) {  // 255 is transparent
        const uint16_t base_x = x0 + j * scale;
        const uint16_t base_y = y0 + i * scale;
        for (uint8_t dy = 0; dy < scale; dy++) {
          for (uint8_t dx = 0; dx < scale; dx++) {
            LCD_Set_Pixel(base_x + dx, base_y + dy, pixel);
          }
        }
      }
    }
  }
}

void LCD_Draw_Sprite(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite){
  LCD_Draw_Sprite_Scaled(x0, y0, nrows, ncols, sprite, 1);
}

void LCD_Draw_Sprite_Colour(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t colour){
  for (int i = 0; i < nrows; i++) {
    for (int j = 0 ; j < ncols ; j++) {
      int pixel = *((sprite+i*ncols)+j);
      if (pixel != 255) {  // 255 is transparent
        LCD_Set_Pixel(x0+j,y0+i, colour);
      }
    }
  }
}

void LCD_Draw_Sprite_Colour_Scaled(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t colour, const uint8_t scale){
  if (scale == 0) {
    return;
  }

  for (int i = 0; i < nrows; i++) {
    for (int j = 0 ; j < ncols ; j++) {
      const int pixel = *((sprite + i * ncols) + j);
      if (pixel != 255) {  // 255 is transparent
        const uint16_t base_x = x0 + j * scale;
        const uint16_t base_y = y0 + i * scale;
        for (uint8_t dy = 0; dy < scale; dy++) {
          for (uint8_t dx = 0; dx < scale; dx++) {
            LCD_Set_Pixel(base_x + dx, base_y + dy, colour);
          }
        }
      }
    }
  }
}

uint16_t colour_ = 0x001F;

void LCD_Fill(ST7789V2_cfg_t* cfg, const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint16_t colour) {
  // Wait until SPI is ready
  while (cfg->spi->SR & SPI_SR_BSY);

  // Set address window
  ST7789V2_Set_Address_Window(cfg, x0, y0, x1, y1);
  colour_ = colour;

  uint32_t len = (x1-x0 + 1) * (y1-y0 + 1);
  ST7789V2_Fill(cfg, &colour_, len);
}

const unsigned char font5x7_[480] = {
    0x00, 0x00, 0x00, 0x00, 0x00,// (space)
    0x00, 0x00, 0x5F, 0x00, 0x00,// !
    0x00, 0x07, 0x00, 0x07, 0x00,// "
    0x14, 0x7F, 0x14, 0x7F, 0x14,// #
    0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
    0x23, 0x13, 0x08, 0x64, 0x62,// %
    0x36, 0x49, 0x55, 0x22, 0x50,// &
    0x00, 0x05, 0x03, 0x00, 0x00,// '
    0x00, 0x1C, 0x22, 0x41, 0x00,// (
    0x00, 0x41, 0x22, 0x1C, 0x00,// )
    0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
    0x08, 0x08, 0x3E, 0x08, 0x08,// +
    0x00, 0x50, 0x30, 0x00, 0x00,// ,
    0x08, 0x08, 0x08, 0x08, 0x08,// -
    0x00, 0x60, 0x60, 0x00, 0x00,// .
    0x20, 0x10, 0x08, 0x04, 0x02,// /
    0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
    0x00, 0x42, 0x7F, 0x40, 0x00,// 1
    0x42, 0x61, 0x51, 0x49, 0x46,// 2
    0x21, 0x41, 0x45, 0x4B, 0x31,// 3
    0x18, 0x14, 0x12, 0x7F, 0x10,// 4
    0x27, 0x45, 0x45, 0x45, 0x39,// 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
    0x01, 0x71, 0x09, 0x05, 0x03,// 7
    0x36, 0x49, 0x49, 0x49, 0x36,// 8
    0x06, 0x49, 0x49, 0x29, 0x1E,// 9
    0x00, 0x36, 0x36, 0x00, 0x00,// :
    0x00, 0x56, 0x36, 0x00, 0x00,// ;
    0x00, 0x08, 0x14, 0x22, 0x41,// <
    0x14, 0x14, 0x14, 0x14, 0x14,// =
    0x41, 0x22, 0x14, 0x08, 0x00,// >
    0x02, 0x01, 0x51, 0x09, 0x06,// ?
    0x32, 0x49, 0x79, 0x41, 0x3E,// @
    0x7E, 0x11, 0x11, 0x11, 0x7E,// A
    0x7F, 0x49, 0x49, 0x49, 0x36,// B
    0x3E, 0x41, 0x41, 0x41, 0x22,// C
    0x7F, 0x41, 0x41, 0x22, 0x1C,// D
    0x7F, 0x49, 0x49, 0x49, 0x41,// E
    0x7F, 0x09, 0x09, 0x01, 0x01,// F
    0x3E, 0x41, 0x41, 0x51, 0x32,// G
    0x7F, 0x08, 0x08, 0x08, 0x7F,// H
    0x00, 0x41, 0x7F, 0x41, 0x00,// I
    0x20, 0x40, 0x41, 0x3F, 0x01,// J
    0x7F, 0x08, 0x14, 0x22, 0x41,// K
    0x7F, 0x40, 0x40, 0x40, 0x40,// L
    0x7F, 0x02, 0x04, 0x02, 0x7F,// M
    0x7F, 0x04, 0x08, 0x10, 0x7F,// N
    0x3E, 0x41, 0x41, 0x41, 0x3E,// O
    0x7F, 0x09, 0x09, 0x09, 0x06,// P
    0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
    0x7F, 0x09, 0x19, 0x29, 0x46,// R
    0x46, 0x49, 0x49, 0x49, 0x31,// S
    0x01, 0x01, 0x7F, 0x01, 0x01,// T
    0x3F, 0x40, 0x40, 0x40, 0x3F,// U
    0x1F, 0x20, 0x40, 0x20, 0x1F,// V
    0x7F, 0x20, 0x18, 0x20, 0x7F,// W
    0x63, 0x14, 0x08, 0x14, 0x63,// X
    0x03, 0x04, 0x78, 0x04, 0x03,// Y
    0x61, 0x51, 0x49, 0x45, 0x43,// Z
    0x00, 0x00, 0x7F, 0x41, 0x41,// [
    0x02, 0x04, 0x08, 0x10, 0x20,// "\"
    0x41, 0x41, 0x7F, 0x00, 0x00,// ]
    0x04, 0x02, 0x01, 0x02, 0x04,// ^
    0x40, 0x40, 0x40, 0x40, 0x40,// _
    0x00, 0x01, 0x02, 0x04, 0x00,// `
    0x20, 0x54, 0x54, 0x54, 0x78,// a
    0x7F, 0x48, 0x44, 0x44, 0x38,// b
    0x38, 0x44, 0x44, 0x44, 0x20,// c
    0x38, 0x44, 0x44, 0x48, 0x7F,// d
    0x38, 0x54, 0x54, 0x54, 0x18,// e
    0x08, 0x7E, 0x09, 0x01, 0x02,// f
    0x08, 0x14, 0x54, 0x54, 0x3C,// g
    0x7F, 0x08, 0x04, 0x04, 0x78,// h
    0x00, 0x44, 0x7D, 0x40, 0x00,// i
    0x20, 0x40, 0x44, 0x3D, 0x00,// j
    0x00, 0x7F, 0x10, 0x28, 0x44,// k
    0x00, 0x41, 0x7F, 0x40, 0x00,// l
    0x7C, 0x04, 0x18, 0x04, 0x78,// m
    0x7C, 0x08, 0x04, 0x04, 0x78,// n
    0x38, 0x44, 0x44, 0x44, 0x38,// o
    0x7C, 0x14, 0x14, 0x14, 0x08,// p
    0x08, 0x14, 0x14, 0x18, 0x7C,// q
    0x7C, 0x08, 0x04, 0x04, 0x08,// r
    0x48, 0x54, 0x54, 0x54, 0x20,// s
    0x04, 0x3F, 0x44, 0x40, 0x20,// t
    0x3C, 0x40, 0x40, 0x20, 0x7C,// u
    0x1C, 0x20, 0x40, 0x20, 0x1C,// v
    0x3C, 0x40, 0x30, 0x40, 0x3C,// w
    0x44, 0x28, 0x10, 0x28, 0x44,// x
    0x0C, 0x50, 0x50, 0x50, 0x3C,// y
    0x44, 0x64, 0x54, 0x4C, 0x44,// z
    0x00, 0x08, 0x36, 0x41, 0x00,// {
    0x00, 0x00, 0x7F, 0x00, 0x00,// |
    0x00, 0x41, 0x36, 0x08, 0x00,// }
    0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
    0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};