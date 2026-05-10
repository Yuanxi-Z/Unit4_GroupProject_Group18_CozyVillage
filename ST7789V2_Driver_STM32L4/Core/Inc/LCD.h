#ifndef LCD_h
#define LCD_h

#include "ST7789V2_Driver.h"
#include <stdlib.h>

/* Basic RGB565 colours */
#define RGB565_BLACK      0x0000
#define RGB565_WHITE      0xFFFF
#define RGB565_RED        0x00F8
#define RGB565_GREEN      0xE007
#define RGB565_BLUE       0x1F00
#define RGB565_YELLOW     0xE0FF
#define RGB565_CYAN       0xFF07
#define RGB565_MAGENTA    0x1FF8
#define RGB565_GREY       0x1084
#define RGB565_LIGHT_GREY 0x18C6
#define RGB565_DARK_GREY  0x0842
#define RGB565_ORANGE     0x20FD
#define RGB565_BROWN      0x45A1
#define RGB565_PINK       0x18FC
#define RGB565_PURPLE     0x0F78
#define RGB565_TEAL       0x3804
#define RGB565_NAVY       0x0F00
#define RGB565_MAROON     0x0080
#define RGB565_OLIVE      0x0084
#define RGB565_SKY_BLUE   0x7D86
#define RGB565_GOLD       0xA0FE
#define RGB565_VIOLET     0x5C91
#define RGB565_RED_BRIGHT      0xC9E0
#define RGB565_GREEN_BRIGHT    0xA93D
#define RGB565_BLUE_BRIGHT     0x0B42
#define RGB565_APRICOT         0xB6F5
#define RGB565_LAVENDER        0x5FD8
#define RGB565_MINT            0xF8A7
#define RGB565_BEIGE           0xD9FD
#define RGB565_LIME_BRIGHT     0xE8B9
#define RGB565_MAGENTA_BRIGHT  0xBCF1
#define RGB565_CYAN_BRIGHT     0xBE44
#define RGB565_ORANGE_BRIGHT   0x06F4
#define RGB565_PURPLE_BRIGHT   0xDE90
#define RGB565_PINK_BRIGHT     0x5AF8
#define RGB565_TEAL_BRIGHT     0xD244
#define RGB565_GREY_STANDARD   0xB5A8

/* Default palette */
#define LCD_COLOUR_0  RGB565_BLACK
#define LCD_COLOUR_1  RGB565_WHITE
#define LCD_COLOUR_2  RGB565_RED
#define LCD_COLOUR_3  RGB565_GREEN
#define LCD_COLOUR_4  RGB565_BLUE
#define LCD_COLOUR_5  RGB565_ORANGE
#define LCD_COLOUR_6  RGB565_YELLOW
#define LCD_COLOUR_7  RGB565_PINK
#define LCD_COLOUR_8  RGB565_PURPLE
#define LCD_COLOUR_9  RGB565_NAVY
#define LCD_COLOUR_10 RGB565_GOLD
#define LCD_COLOUR_11 RGB565_VIOLET
#define LCD_COLOUR_12 RGB565_BROWN
#define LCD_COLOUR_13 RGB565_GREY
#define LCD_COLOUR_14 RGB565_CYAN
#define LCD_COLOUR_15 RGB565_MAGENTA

/* Vintage palette */
#define RGB565_VINTAGE_0   0x0000
#define RGB565_VINTAGE_1   0xF39C
#define RGB565_VINTAGE_2   0xFFFF
#define RGB565_VINTAGE_3   0x26B9
#define RGB565_VINTAGE_4   0x71E3
#define RGB565_VINTAGE_5   0xE549
#define RGB565_VINTAGE_6   0x24A3
#define RGB565_VINTAGE_7   0x46EC
#define RGB565_VINTAGE_8   0x0DF7
#define RGB565_VINTAGE_9   0x492A
#define RGB565_VINTAGE_10  0x4344
#define RGB565_VINTAGE_11  0x64A6
#define RGB565_VINTAGE_12  0x2619
#define RGB565_VINTAGE_13  0xB002
#define RGB565_VINTAGE_14  0x1E35
#define RGB565_VINTAGE_15  0xFDB6

/* Greyscale palette */
#define RGB565_GREY_0   0x0000
#define RGB565_GREY_1   0xC318
#define RGB565_GREY_2   0x4529
#define RGB565_GREY_3   0xC739
#define RGB565_GREY_4   0x2842
#define RGB565_GREY_5   0xAA52
#define RGB565_GREY_6   0x2C63
#define RGB565_GREY_7   0x8E73
#define RGB565_GREY_8   0xEF7B
#define RGB565_GREY_9   0x718C
#define RGB565_GREY_10  0xD39C
#define RGB565_GREY_11  0x55AD
#define RGB565_GREY_12  0xF7BD
#define RGB565_GREY_13  0x9AD6
#define RGB565_GREY_14  0x3CE7
#define RGB565_GREY_15  0xFFFF

#define BUFFER_LENGTH ST7789V2_HEIGHT*ST7789V2_WIDTH/2

typedef enum {
    PALETTE_DEFAULT = 0,
    PALETTE_GREYSCALE,
    PALETTE_VINTAGE,
    PALETTE_CUSTOM,
    PALETTE_FARM_UNIFIED
} LCD_Palette;

void LCD_Set_Palette(LCD_Palette palette);
void LCD_init(ST7789V2_cfg_t* cfg);
void LCD_turnOff(ST7789V2_cfg_t* cfg);
void LCD_turnOn(ST7789V2_cfg_t* cfg);
void LCD_clear();
void LCD_normalMode(ST7789V2_cfg_t* cfg);
void LCD_inverseMode(ST7789V2_cfg_t* cfg);
void LCD_printString(char const *str, const uint16_t x, const uint16_t y, uint8_t colour, uint8_t font_size);
void LCD_printChar(char const c, const uint16_t x, const uint16_t y, uint8_t colour);
void LCD_Set_Pixel(const uint16_t x, const uint16_t y, uint8_t colour);
uint8_t LCD_Get_Pixel(const uint16_t x, const uint16_t y);
void LCD_Refresh(ST7789V2_cfg_t* cfg);
void LCD_randomiseBuffer();
void LCD_plotArray(float const array[], const uint8_t colour);
void LCD_Draw_Circle(const uint16_t x0, const uint16_t y0, const uint16_t radius, const uint8_t colour, const uint8_t fill);
void LCD_Draw_Line(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint8_t colour);
void LCD_Draw_Rect(const uint16_t x0, const uint16_t y0, const uint16_t width, const uint16_t height, const uint8_t colour, const uint8_t fill);
void LCD_Draw_Sprite(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite);
void LCD_Draw_Sprite_Colour(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t colour);
void LCD_Draw_Sprite_Scaled(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t scale);
void LCD_Draw_Sprite_Colour_Scaled(const uint16_t x0, const uint16_t y0, const uint16_t nrows, const uint16_t ncols, const uint8_t *sprite, const uint8_t colour, const uint8_t scale);
void LCD_Fill_Buffer(const uint8_t colour);
void LCD_Fill(ST7789V2_cfg_t* cfg, const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint16_t colour);

extern const unsigned char font5x7_[480];

#endif