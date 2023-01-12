#include "font.h"

//4列8行
const unsigned char ascii_0408[][4]=
{
    {0x00,0x00,0x00,0x00},//' '
    {0x00,0x00,0x2E,0x00},//'!'
    {0x00,0x06,0x00,0x06},//'"'
    {0x00,0x14,0x3E,0x14},//'#'
    {0x00,0x5C,0xFE,0x74},//'$'
    {0x00,0x3A,0x08,0x2E},//'%'
    {0x00,0xFC,0x54,0x7E},//'&'
    {0x00,0x00,0x06,0x00},//'''
    {0x00,0x1C,0x22,0x00},//'('
    {0x00,0x00,0x22,0x1C},//')'
    {0x00,0x14,0x08,0x14},//'*'
    {0x00,0x08,0x1C,0x08},//'+'
    {0x00,0x00,0x20,0x18},//','
    {0x00,0x08,0x08,0x08},//'-'
    {0x00,0x00,0x20,0x00},//'.'
    {0x00,0x30,0x0C,0x02},//'/'
    {0x00,0x3E,0x22,0x3E},//'0'
    {0x00,0x24,0x3E,0x20},//'1'
    {0x00,0x3A,0x2A,0x2E},//'2'
    {0x00,0x2A,0x2A,0x3E},//'3'
    {0x00,0x0E,0x08,0x3E},//'4'
    {0x00,0x2E,0x2A,0x3A},//'5'
    {0x00,0x3E,0x2A,0x3A},//'6'
    {0x00,0x02,0x02,0x3E},//'7'
    {0x00,0x3E,0x2A,0x3E},//'8'
    {0x00,0x2E,0x2A,0x3E},//'9'
    {0x00,0x00,0x14,0x00},//':'
    {0x00,0x20,0x14,0x00},//';'
    {0x00,0x08,0x14,0x22},//'<'
    {0x00,0x14,0x14,0x14},//'='
    {0x00,0x22,0x14,0x08},//'>'
    {0x00,0x02,0x2A,0x0E},//'?'
    {0x00,0x3E,0x32,0x3E},//'@'
    {0x00,0x3C,0x0A,0x3C},//'A'
    {0x00,0x3E,0x2A,0x14},//'B'
    {0x00,0x3E,0x22,0x22},//'C'
    {0x00,0x3E,0x22,0x1C},//'D'
    {0x00,0x3E,0x2A,0x22},//'E'
    {0x00,0x3E,0x0A,0x02},//'F'
    {0x00,0x3E,0x2A,0x3A},//'G'
    {0x00,0x3E,0x08,0x3E},//'H'
    {0x00,0x22,0x3E,0x22},//'I'
    {0x00,0x22,0x3E,0x02},//'J'
    {0x00,0x3E,0x08,0x34},//'K'
    {0x00,0x3E,0x20,0x20},//'L'
    {0x00,0x3E,0x0C,0x3E},//'M'
    {0x00,0x3E,0x02,0x3E},//'N'
    {0x00,0x1C,0x22,0x1C},//'O'
    {0x00,0x3E,0x0A,0x0E},//'P'
    {0x00,0x0E,0x0A,0x3E},//'Q'
    {0x00,0x3E,0x1A,0x24},//'R'
    {0x00,0x24,0x2A,0x12},//'S'
    {0x00,0x02,0x3E,0x02},//'T'
    {0x00,0x3E,0x20,0x3E},//'U'
    {0x00,0x1E,0x20,0x1E},//'V'
    {0x00,0x3E,0x38,0x3E},//'W'
    {0x00,0x36,0x08,0x36},//'X'
    {0x00,0x06,0x38,0x06},//'Y'
    {0x00,0x32,0x2A,0x26},//'Z'
    {0x00,0x3E,0x22,0x00},//'['
    {0x00,0x02,0x0C,0x30},//'\'
    {0x00,0x00,0x22,0x3E},//']'
    {0x00,0x04,0x02,0x04},//'^'
    {0x00,0x20,0x20,0x20},//'_'
    {0x00,0x00,0x02,0x04},//'`'
    {0x00,0x10,0x08,0x04},//'/'
};
//

const unsigned char ascii_0608[][6]=//每个字符使用6个数据表示
{
    {0x00,0x00,0x00,0x00,0x00,0x00},//' '
    {0x00,0x00,0xDE,0xDE,0x00,0x00},//'!'
    {0x00,0x00,0x06,0x00,0x06,0x00},//'"'
    {0x00,0x24,0xFE,0x24,0xFE,0x24},//'#'
    {0x00,0x48,0x54,0xFE,0x54,0x24},//'$'
    {0x00,0x46,0x26,0x10,0xC8,0xC4},//'%'
    {0x00,0xF0,0x9E,0x9A,0xFE,0x90},//'&'
    {0x00,0x00,0x00,0x00,0x06,0x00},//'''
    {0x00,0x00,0x7C,0x82,0x00,0x00},//'('
    {0x00,0x00,0x82,0x7C,0x00,0x00},//')'
    {0x00,0x22,0x14,0x3E,0x14,0x22},//'*'
    {0x00,0x08,0x08,0x3E,0x08,0x08},//'+'
    {0x00,0x00,0x00,0xB0,0x70,0x00},//','
    {0x00,0x08,0x08,0x08,0x08,0x08},//'-'
    {0x00,0x00,0x00,0xC0,0xC0,0x00},//'.'
    {0x00,0xC0,0x30,0x0C,0x02,0x00},//'/'
    {0x00,0x7C,0xA2,0x92,0x8A,0x7C},//'0'	16原ASCII：48-》48-0x20=16
    {0x00,0x00,0x84,0xFE,0x80,0x00},//'1'
    {0x00,0xCC,0xA2,0x92,0x92,0xCC},//'2'
    {0x00,0x44,0x92,0x92,0x92,0x6C},//'3'
    {0x00,0x30,0x28,0xA4,0xFE,0xA0},//'4'
    {0x00,0x5E,0x92,0x92,0x92,0x62},//'5'
    {0x00,0x7C,0x92,0x92,0x92,0x64},//'6'
    {0x00,0x06,0x02,0xF2,0x0A,0x06},//'7'
    {0x00,0x6C,0x92,0x92,0x92,0x6C},//'8'
    {0x00,0x5C,0x92,0x92,0x92,0x7C},//'9'
    {0x00,0x00,0x00,0xD8,0xD8,0x00},//':'冒号
    {0x00,0x00,0x00,0xB6,0x76,0x00},//';'
    {0x00,0x10,0x28,0x44,0x82,0x00},//'<'
    {0x00,0x50,0x50,0x50,0x50,0x50},//'='
    {0x00,0x00,0x82,0x44,0x28,0x10},//'>'
    {0x00,0x04,0x02,0xA2,0x12,0x0C},//'?'
    {0x00,0x7C,0xFA,0xEA,0xFA,0x3C},//'@'
    {0x00,0xFC,0x12,0x12,0x12,0xFC},//'A'
    {0x00,0xFE,0x92,0x92,0x92,0x6C},//'B'
    {0x00,0x7C,0x82,0x82,0x82,0x44},//'C'
    {0x00,0xFE,0x82,0x82,0x44,0x38},//'D'
    {0x00,0xFE,0x92,0x92,0x92,0x82},//'E'
    {0x00,0xFE,0x12,0x12,0x12,0x02},//'F'
    {0x00,0x7C,0x82,0x92,0x92,0xF4},//'G'
    {0x00,0xFE,0x10,0x10,0x10,0xFE},//'H'
    {0x00,0x00,0x82,0xFE,0x82,0x00},//'I'
    {0x00,0x40,0x82,0x7E,0x02,0x00},//'J'
    {0x00,0xFE,0x10,0x28,0x44,0x82},//'K'
    {0x00,0xFE,0x80,0x80,0x80,0x80},//'L'
    {0x00,0xFE,0x04,0x18,0x04,0xFE},//'M'
    {0x00,0xFE,0x08,0x10,0x20,0xFE},//'N'
    {0x00,0x7C,0x82,0x82,0x82,0x7C},//'O'
    {0x00,0xFE,0x12,0x12,0x12,0x0C},//'P'
    {0x00,0x7C,0x82,0xA2,0xC2,0x7C},//'Q'
    {0x00,0xFE,0x12,0x32,0x52,0x8C},//'R'
    {0x00,0x8C,0x92,0x92,0x92,0x62},//'S'
    {0x00,0x02,0x02,0xFE,0x02,0x02},//'T'
    {0x00,0x7E,0x80,0x80,0x80,0x7E},//'U'
    {0x00,0x3E,0x40,0x80,0x40,0x3E},//'V'
    {0x00,0x7E,0x80,0xF0,0x80,0x7E},//'W'
    {0x00,0xC6,0x28,0x10,0x28,0xC6},//'X'
    {0x00,0x06,0x08,0xF0,0x08,0x06},//'Y'
    {0x00,0xC2,0xA2,0x92,0x8A,0x86},//'Z'
    {0x00,0x00,0xFE,0x82,0x00,0x00},//'['
    {0x00,0x02,0x0C,0x30,0xC0,0x00},//'\'
    {0x00,0x00,0x82,0xFE,0x00,0x00},//']'
    {0x00,0x04,0x02,0x02,0x04,0x00},//'^'
    {0x00,0x80,0x80,0x80,0x80,0x80},//'_'
    {0x00,0x00,0x02,0x04,0x00,0x00},//'`'
    {0x00,0x50,0xA8,0xA8,0x70,0x80},//'a'
    {0x00,0x02,0xFE,0x88,0x88,0x70},//'b'
    {0x00,0x70,0x88,0x88,0x88,0x50},//'c'
    {0x00,0x70,0x88,0x88,0x7E,0x82},//'d'
    {0x00,0x70,0xA8,0xA8,0xA8,0xB0},//'e'
    {0x00,0x00,0x90,0xFC,0x92,0x04},//'f'
    {0x00,0x54,0xAA,0xAA,0xA6,0x42},//'g'
    {0x00,0x82,0xFE,0x08,0xF0,0x80},//'h'
    {0x00,0x00,0x88,0xFA,0x80,0x00},//'i'
    {0x00,0x80,0x88,0x7A,0x00,0x00},//'j'
    {0x00,0x82,0xFE,0x20,0xD8,0x88},//'k'
    {0x00,0x00,0x82,0xFE,0x80,0x00},//'l'
    {0x00,0xF8,0x08,0xF8,0x08,0xF0},//'m'
    {0x00,0xF8,0x10,0x08,0xF0,0x00},//'n'
    {0x00,0x70,0x88,0x88,0x88,0x70},//'o'
    {0x00,0x80,0xF8,0xA8,0x28,0x10},//'p'
    {0x00,0x10,0x28,0xA8,0xF8,0x80},//'q'
    {0x00,0x00,0x88,0xF8,0x88,0x10},//'r'
    {0x00,0x90,0xA8,0xA8,0xA8,0x48},//'s'
    {0x00,0x00,0x08,0x7C,0x88,0x40},//'t'
    {0x00,0x78,0x80,0x88,0x78,0x80},//'u'
    {0x00,0x38,0x40,0x80,0x40,0x38},//'v'
    {0x00,0x38,0x40,0xF0,0x40,0x38},//'w'
    {0x00,0x88,0x50,0x20,0x50,0x88},//'x'
    {0x00,0x88,0x50,0x20,0x10,0x08},//'y'
    {0x00,0x98,0xC8,0xA8,0x98,0xC8},//'z'
    {0x00,0x00,0x10,0x6C,0x82,0x82},//'{'
    {0x00,0x00,0x00,0xFE,0x00,0x00},//'|'
    {0x00,0x00,0x82,0x82,0x6C,0x10},//'}'
    {0x00,0x04,0x02,0x04,0x04,0x02},//'~'
};

//
//10列16行
const unsigned char ascii_1016[][20]=
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//' '
{0x00,0x00,0x00,0x00,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDF,0xDF,0x00,0x00,0x00,0x00},//'!'
{0x00,0x00,0x0C,0x06,0x00,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//'"'
{0x00,0x30,0xFE,0xFE,0x30,0x30,0xFE,0xFE,0x30,0x00,0x00,0x06,0xFF,0xFF,0x06,0x06,0xFF,0xFF,0x06,0x00},//'#'
{0x00,0xF8,0xFC,0x8C,0xFE,0x8C,0xFE,0x8C,0x8C,0x0C,0x00,0x30,0x31,0x31,0xFF,0x31,0xFF,0x31,0x3F,0x1F},//'$'
{0x00,0x0C,0x12,0x12,0x8C,0xE0,0x78,0x1C,0x06,0x00,0x00,0x60,0x38,0x1E,0x07,0x31,0x48,0x48,0x30,0x00},//'%'
{0x00,0x00,0x1C,0xBE,0xE6,0xE6,0xBE,0x1C,0x00,0x00,0x00,0x3E,0x7F,0x61,0x61,0x61,0x63,0x3F,0x1C,0x67},//'&'
{0x00,0x00,0x00,0x00,0x00,0x20,0x1E,0x0E,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//'''
{0x00,0x00,0xC0,0xF0,0xF8,0x1C,0x0E,0x06,0x00,0x00,0x00,0x00,0x07,0x1F,0x3F,0x70,0xE0,0xC0,0x00,0x00},//'('
{0x00,0x00,0x00,0x06,0x0E,0x1C,0xF8,0xF0,0xC0,0x00,0x00,0x00,0x00,0xC0,0xE0,0x70,0x3F,0x1F,0x07,0x00},//')'
{0x00,0x3C,0x70,0xE0,0xFC,0xFC,0xE0,0x70,0x3C,0x00,0x00,0x3C,0x0E,0x07,0x3F,0x3F,0x07,0x0E,0x3C,0x00},//'*'
{0x00,0x80,0x80,0x80,0xF0,0xF0,0x80,0x80,0x80,0x00,0x00,0x01,0x01,0x01,0x0F,0x0F,0x01,0x01,0x01,0x00},//'+'
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4E,0x3E,0x1E,0x00,0x00,0x00,0x00,0x00},//','
{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00},//'-'
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00},//'.'
{0x00,0x00,0x00,0x00,0x80,0xE0,0x78,0x1C,0x0C,0x00,0x00,0x30,0x38,0x1E,0x07,0x01,0x00,0x00,0x00,0x00},//'/'
{0x00,0xFC,0xFE,0x06,0x06,0x86,0x86,0xFE,0xFE,0xFC,0x00,0x7F,0xFF,0xFE,0xC3,0xC3,0xC1,0xC0,0xFF,0x7F},//'0'     //16原ASCII：48-》48-0x20=16
{0x00,0x0C,0x0C,0x0C,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xFF,0xFF,0xC0,0xC0,0xC0,0x00},//'1'
{0x00,0x0C,0x0E,0x06,0x06,0x06,0x86,0xC6,0xFE,0x7C,0x00,0xF0,0xF8,0xDC,0xCE,0xC7,0xC3,0xC1,0xE0,0xE0},//'2'
{0x00,0x0C,0x0E,0x06,0x86,0x86,0x86,0x86,0xFE,0x7C,0x00,0x60,0xE0,0xC0,0xC1,0xC1,0xC1,0xC1,0xFF,0x7E},/*3*/
{0x00,0xE0,0xF0,0x18,0x0C,0x06,0xFE,0xFE,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0xFF,0xFF,0x03,0x03},/*4*/     //20
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0x86,0x06,0x00,0x61,0xE1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F},/*5*/
{0x00,0xE0,0xF0,0x38,0x1C,0x0E,0x06,0x06,0x00,0x00,0x00,0x7F,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xFF,0x7E},/*6*/
{0x00,0x0E,0x0E,0x06,0x86,0xC6,0xE6,0x76,0x3E,0x1E,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00},/*7*/
{0x00,0x7C,0x7E,0x86,0x86,0x86,0x86,0x86,0x7E,0x7C,0x00,0x7E,0xFE,0xC1,0xC1,0xC1,0xC1,0xC1,0xFE,0x7E},/*8*/
{0x00,0xFC,0xFE,0x86,0x86,0x86,0x86,0x86,0xFE,0xFC,0x00,0x60,0xE1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F},/*9*/
{0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C,0x00,0x00,0x00,0x00},//':'
{0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4C,0x3C,0x1C,0x00,0x00,0x00,0x00},//';'
{0x00,0x80,0xC0,0x60,0x30,0x18,0x0C,0x04,0x00,0x00,0x00,0x01,0x03,0x06,0x0C,0x18,0x30,0x20,0x00,0x00},//'<'
{0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00},//'='
{0x00,0x00,0x04,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x20,0x30,0x18,0x0C,0x06,0x03,0x01,0x00},//'>'     //30
{0x00,0x18,0x3C,0x26,0x06,0x86,0xC6,0x7C,0x38,0x00,0x00,0x00,0x00,0x00,0x6F,0x6F,0x00,0x00,0x00,0x00},//'?'
{0x00,0xF8,0xFC,0x86,0xC6,0x86,0x06,0xFC,0xF8,0x00,0x00,0x1F,0x3F,0x63,0x66,0x67,0x66,0x47,0x03,0x00},//'@'
{0x00,0xF0,0xF8,0x9C,0x8E,0x86,0x8E,0x9C,0xF8,0xF0,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF},/*A*/
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0xFE,0xFC,0x00,0xFF,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F},/*B*/
{0x00,0xFC,0xFE,0x06,0x06,0x06,0x06,0x06,0x1E,0x1C,0x00,0x7F,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xF0,0x70},/*C*/
{0x00,0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0xFE,0xFC,0x00,0xFF,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0x7F},/*D*/
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0x86,0x06,0x00,0xFF,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC0},/*E*/
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0x86,0x06,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x00},/*F*/
{0x00,0xFC,0xFE,0x06,0x06,0x86,0x86,0x86,0x8E,0x8C,0x00,0x7F,0xFF,0xC0,0xC0,0xC1,0xC1,0xC1,0xFF,0x7F},/*G*/
{0x00,0xFE,0xFE,0x80,0x80,0x80,0x80,0x80,0xFE,0xFE,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF},/*H*/     //40
{0x00,0x00,0x06,0x06,0xFE,0xFE,0x06,0x06,0x00,0x00,0x00,0x00,0xC0,0xC0,0xFF,0xFF,0xC0,0xC0,0x00,0x00},/*I*/
{0x00,0x00,0x00,0x06,0x06,0xFE,0xFE,0x06,0x06,0x00,0x00,0x00,0x60,0xE0,0xC0,0xFF,0x7F,0x00,0x00,0x00},/*J*/
{0x00,0xFE,0xFE,0x80,0x80,0x80,0xE0,0x78,0x1E,0x06,0x00,0xFF,0xFF,0x01,0x01,0x01,0x07,0x1E,0xF8,0xE0},/*K*/
{0x00,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xF0,0xF0},/*L*/
{0x00,0xFE,0xFE,0x0C,0x18,0x30,0x18,0x0C,0xFE,0xFE,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF},/*M*/
{0x00,0xFE,0xFE,0x1E,0xF8,0xC0,0x00,0x00,0xFE,0xFE,0x00,0xFF,0xFF,0x00,0x01,0x0F,0x3F,0xF0,0xFF,0xFF},/*N*/
{0x00,0xFC,0xFE,0x06,0x06,0x06,0x06,0x06,0xFE,0xFC,0x00,0x7F,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0x7F},/*O*/
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0xFE,0xFC,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x00},/*P*/
{0x00,0xFC,0xFE,0x06,0x06,0x06,0x06,0x06,0xFE,0xFC,0x00,0x7F,0xFF,0xC0,0xC0,0xD8,0xD8,0xE0,0xFF,0x7F},/*Q*/
{0x00,0xFE,0xFE,0x86,0x86,0x86,0x86,0x86,0xFE,0xFC,0x00,0xFF,0xFF,0x01,0x03,0x07,0x1D,0x39,0xE1,0xC0},/*R*/     //50
{0x00,0xFC,0xFE,0x86,0x86,0x86,0x86,0x86,0x9E,0x1C,0x00,0x70,0xF1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F},/*S*/
{0x00,0x06,0x06,0x06,0xFE,0xFE,0x06,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00},/*T*/
{0x00,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0x00,0x7F,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0x7F},/*U*/
{0x00,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0x00,0x1F,0x3F,0x60,0xC0,0x80,0xC0,0x60,0x3F,0x1F},/*V*/
{0x00,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0x00,0x1F,0x3F,0x60,0xC0,0xFF,0xC0,0x60,0x3F,0x1F},/*W*/
{0x00,0x3E,0x7E,0xC0,0x80,0x00,0x80,0xC0,0x7E,0x3E,0x00,0xF8,0xFC,0x06,0x03,0x01,0x03,0x06,0xFC,0xF8},/*X*/
{0x00,0x7E,0xFE,0x80,0x00,0x00,0x80,0xFE,0x7E,0x00,0x00,0x00,0x01,0x03,0xFF,0xFF,0x03,0x01,0x00,0x00},/*Y*/
{0x00,0x06,0x06,0x06,0x06,0x86,0xC6,0xE6,0x7E,0x3E,0x00,0xF8,0xFC,0xCE,0xC7,0xC3,0xC1,0xC0,0xC0,0xC0},/*Z*/
{0x00,0x00,0x00,0xFC,0xFC,0x0C,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x30,0x30,0x00,0x00,0x00},//'['
{0x00,0x0C,0x1C,0x78,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x1E,0x38,0x30,0x00},//'\'     //60
{0x00,0x00,0x00,0x0C,0x0C,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3F,0x3F,0x00,0x00,0x00},//']'
{0x00,0x60,0x30,0x18,0x0C,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//'^'
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00},//'_'
{0x00,0x00,0x04,0x1C,0x18,0x30,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//'`'
{0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x79,0xFD,0xCC,0xCC,0xCC,0xCC,0x7F,0xFF,0x80},/*a*/
{0x00,0xFE,0xFE,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0xFF,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F},/*b*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x7F,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xE3,0x63},/*c*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0xFE,0xFE,0x00,0x7F,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0xFF},/*d*/
{0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x7F,0xFF,0xCC,0xCC,0xCC,0xCC,0xCC,0xEF,0x6F},/*e*/
{0x00,0x00,0x80,0x80,0x80,0xFC,0xFE,0x86,0x8E,0x8C,0x00,0x00,0x01,0x01,0x01,0xFF,0xFF,0x01,0x01,0x01},/*f*/     //70
{0x00,0x38,0x78,0xC6,0xC6,0xC6,0xC6,0x7C,0x3E,0x06,0x00,0x78,0xFF,0xCF,0xCC,0xCC,0xCC,0xCC,0xFC,0x78},/*g*/
{0x00,0xFE,0xFE,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF},/*h*/
{0x00,0x00,0x60,0x60,0xE6,0xE6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00},/*i*/
{0x00,0x00,0x30,0x30,0xF6,0xF6,0x00,0x00,0x00,0x00,0x00,0x60,0xE0,0xC0,0xFF,0x7F,0x00,0x00,0x00,0x00},/*j*/
{0x00,0x04,0xFE,0xFE,0x00,0x40,0xC0,0xC0,0x40,0x40,0x00,0x80,0xFF,0xFF,0x86,0x1F,0x39,0x70,0xC0,0x80},/*k*/
{0x00,0x00,0x06,0x06,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00},/*l*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0x01,0xFF,0xFF,0x01,0xFF,0xFF},/*m*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0xFF,0xFF},/*n*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x7F,0xFF,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F,0x00},/*o*/
{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0x19,0x19,0x19,0x19,0x1F,0x0F,0x00},/*p*/     //80
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x0F,0x1F,0x19,0x19,0x19,0x19,0xFF,0xFF,0x00},/*q*/
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x01,0x01,0xFF,0xFF,0x01,0x07,0x07,0x00},/*r*/
{0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x67,0xEF,0xCC,0xCC,0xCC,0xCC,0xFD,0x79},/*s*/
{0x00,0x80,0x80,0x80,0xF0,0xF0,0x80,0x80,0x80,0x00,0x00,0x01,0x01,0x01,0x7F,0xFF,0xC1,0xC1,0xE1,0x60},/*t*/
{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x7F,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0x7F},/*u*/
{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x0F,0x1F,0x30,0xE0,0xC0,0xE0,0x30,0x1F,0x0F},/*v*/
{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x0F,0x1F,0x30,0xE0,0xFF,0xE0,0x30,0x1F,0x0F},/*w*/
{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0xE3,0xE3,0x14,0x1C,0x08,0x1C,0x14,0xE3,0xE3},/*x*/
{0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x03,0xC3,0xCC,0xFC,0x78,0x0C,0x0C,0x03,0x03},//'y'
{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0xE1,0xE1,0xD1,0xD9,0xC9,0xCD,0xC5,0xC3,0xC3},//'z'     //90
{0x00,0x80,0xC0,0x78,0x7C,0x06,0x02,0x00,0x00,0x00,0x00,0x01,0x03,0x1E,0x3E,0x60,0x40,0x00,0x00,0x00},//'{'
{0x00,0x00,0x00,0x00,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00},//'|'
{0x00,0x00,0x00,0x02,0x06,0x7C,0x78,0xC0,0x80,0x00,0x00,0x00,0x00,0x40,0x60,0x3E,0x1E,0x03,0x01,0x00},//'}'
{0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0x00},//向左箭头
{0x00,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00},//向右箭头
};
//
const unsigned char pic_0408[][4]=
{
    {0x00,0x18,0x1C,0x18},//箭头上
    {0x00,0x0C,0x1C,0x0C},//箭头下
    {0x00,0x1C,0x1C,0x1C},//音量
};
//
//左右箭头
const char pic_0608[][6]=
{
    {0x00,0x10,0x38,0x7C,0xFE,0x00},//向左箭头
    {0x00,0xFE,0x7C,0x38,0x10,0x00},//向右箭头
    {0x0c,0x06,0x03,0x0c,0x30,0xc0},//打勾
    {0x00,0x40,0x60,0x70,0x60,0x40},//向上箭头
    {0x00,0x02,0x06,0x0E,0x06,0x02},//向下箭头
    {0x00,0x00,0x00,0x00,0x00,0x00},//' '
};
//
//
const char pic_0808[][8]=
{
    {0x00,0x74,0x32,0x52,0x4A,0x4C,0x2E,0x00},//循环标志
    {0x18,0x38,0x70,0x70,0x38,0x1c,0x0e,0x06},//打勾
};
//
const char pic_1616[]=
{
0xFF,0x01,0xFD,0xFD,0xFD,0x1D,0xFF,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x00,0x00,
0x01,0x01,0x1F,0x1F,0x1F,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//0

0x00,0x00,0xF8,0x04,0xE0,0x10,0x00,0xC0,0xC0,0x00,0x10,0xE0,0x04,0xF8,0x00,0x00,
0x00,0x00,0x03,0x24,0x20,0x31,0x30,0x3F,0x3F,0x30,0x31,0x20,0x24,0x03,0x00,0x00,//1,信号标志(未被选中)
    
0xFF,0x01,0xF9,0x05,0xE1,0x11,0x01,0xC1,0xC1,0x01,0x11,0xE1,0x05,0xF9,0x01,0xFF,
0xFF,0x80,0x83,0xA4,0xA0,0xB1,0xB0,0xBF,0xBF,0xB0,0xB1,0xA0,0xA4,0x83,0x80,0xFF,//2,信号标志（被选中）
    
0x00,0x00,0x7C,0x14,0x00,0x7C,0x18,0x7C,0x80,0x40,0x20,0x10,0x08,0x04,0x00,0x00,
0x00,0x00,0x20,0x10,0x08,0x04,0x1E,0x21,0x1C,0x00,0x3C,0x20,0x3C,0x00,0x00,0x00,//3.A/B模式（未被选中）
    
0xFF,0x01,0x7D,0x15,0x01,0x7D,0x19,0x7D,0x81,0x41,0x21,0x11,0x09,0x05,0x01,0xFF,
0xFF,0x80,0xA0,0x90,0x88,0x84,0x9E,0xA1,0x9C,0x80,0xBC,0xA0,0xBC,0x80,0x80,0xFF,//4.A/B模式（被选中）
    
0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x08,0x04,0x04,0x04,0x04,0x08,0xF0,0x00,0x00,
0x00,0x00,0x30,0x38,0x1C,0x0E,0x07,0x03,0x02,0x02,0x02,0x02,0x01,0x00,0x00,0x00,//5.搜索模式（未被选中）
    
0xFF,0x01,0x01,0x01,0x01,0x01,0xF1,0x09,0x05,0x05,0x05,0x05,0x09,0xF1,0x01,0xFF,
0xFF,0x80,0xB0,0xB8,0x9C,0x8E,0x87,0x83,0x82,0x82,0x82,0x82,0x81,0x80,0x80,0xFF,//6.搜索模式（被选中）
    
0x00,0x00,0x70,0x38,0x3C,0x7C,0xE4,0xC0,0x80,0xD8,0x24,0x40,0x40,0x20,0x00,0x00,
0x00,0x00,0x04,0x02,0x02,0x24,0x1A,0x01,0x03,0x07,0x0E,0x1C,0x38,0x30,0x00,0x00,//7.设置（未被选中）

0xFF,0x01,0x71,0x39,0x3D,0x7D,0xE5,0xC1,0x81,0xD9,0x25,0x41,0x41,0x21,0x01,0xFF,
0xFF,0x80,0x84,0x82,0x82,0xA4,0x9A,0x81,0x83,0x87,0x8E,0x9C,0xB8,0xB0,0x80,0xFF,//8.设置（被选中）

0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x78,0x04,0x04,0x04,0x04,0x04,0x78,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,//9.上锁

0x00,0x18,0x0C,0x04,0x04,0x04,0x8C,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,//10.开锁

0x00,0xFC,0x02,0x02,0x82,0xC2,0xE2,0xF2,0x02,0x02,0x82,0xC2,0xE2,0xF2,0x02,0xFC,
0x00,0x7F,0x80,0x81,0x83,0x87,0x8F,0x9F,0x80,0x81,0x83,0x87,0x8F,0x9F,0x80,0x7F,//11.向前搜台

0x00,0xFC,0x02,0xF2,0xE2,0xC2,0x82,0x02,0x02,0xF2,0xE2,0xC2,0x82,0x02,0x02,0xFC,
0x00,0x7F,0x80,0x9F,0x8F,0x87,0x83,0x81,0x80,0x9F,0x8F,0x87,0x83,0x81,0x80,0x7F,//12.向后搜台

0x00,0xFC,0x02,0x02,0x02,0x82,0x42,0x42,0x42,0x42,0x4A,0x52,0xE2,0x42,0x02,0xFC,
0x00,0x7F,0x80,0x80,0xBF,0x80,0x80,0x80,0x80,0x80,0x82,0x81,0x80,0x80,0x80,0x7F,//13.返回标志

0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xF0,0xFC,0x3E,0x0E,
0x00,0x03,0x07,0x0F,0x1E,0x3C,0x78,0x78,0x7C,0x3F,0x0F,0x07,0x03,0x00,0x00,0x00,//14.勾


};


const char pic2516[][50]=
{
{0xFF,0xFF,0x07,0x03,0xF3,0xF3,0x03,0x07,0xFF,0xFF,0xFF,0xFF,0x3F,0xDF,0xEF,0xF7,
0xC1,0xE3,0xF7,0xFF,0xF7,0xEF,0xDF,0x3F,0xFF,0xFF,0xFF,0xE0,0xC0,0xCF,0xCF,0xC0,
0xE0,0xFF,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB,0xF7,0xFF,0xF7,0xE3,0xC1,0xF7,0xFB,0xFD,
0xFE,0xFF,},//0
{0xFF,0xFF,0xFF,0xF7,0x03,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0x87,0xEB,0x87,0xFF,0x83,
0xAB,0xD7,0xFF,0xC7,0xBB,0xBB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xC0,0xC0,0xDF,
0xFF,0xE3,0xDD,0xDD,0xFF,0xC3,0xF5,0xC3,0xFF,0xC1,0xDF,0xDF,0xFF,0xC1,0xDF,0xDF,
0xFF,0xFF,},//1
{0xFF,0xFF,0xFF,0xF3,0x73,0x73,0x73,0x03,0x07,0xFF,0xFF,0xFF,0x07,0x77,0x8F,0xFF,
0x07,0x57,0x77,0xFF,0x07,0xD7,0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0xC0,0xCE,0xCE,
0xCE,0xCF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC1,0xDF,0xDF,0xFF,0xFD,0xC1,0xFD,
0xFF,0xFF,},//2
{0xFF,0xFF,0xF3,0xF3,0x73,0x73,0x07,0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,0x77,0x17,
0xFF,0x07,0xDF,0x07,0xFF,0x77,0x07,0x77,0xFF,0xFF,0xFF,0xCF,0xCF,0xCE,0xCE,0xE0,
0xE1,0xFF,0xC1,0xF3,0xC1,0xFF,0xE3,0xDD,0xE3,0xFF,0xC1,0xDD,0xE3,0xFF,0xC1,0xD5,
0xDD,0xFF,},//3

{0xFF,0xFF,0x03,0x03,0x3F,0x3F,0x03,0x03,0xFF,0xFF,0xFF,0x7F,0x7F,0x87,0xFF,0x07,
0xDF,0x27,0xFF,0x07,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,
0xC0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD3,0xD5,0xE5,0xFF,0xE3,0xDD,0xCD,0xD3,0xFF,
0xFF,0xFF,},//4

{0xFF,0xFF,0x03,0x03,0x33,0x33,0x33,0x73,0xFF,0xFF,0x83,0xF3,0xE7,0xF3,0x83,0xFF,
0x83,0xF3,0xCF,0x83,0xFF,0xC7,0xBB,0xC7,0xFF,0xFF,0xFF,0xCF,0xCF,0xCF,0xCF,0xC0,
0xE0,0xFF,0xCD,0xD5,0xD9,0xFF,0xC1,0xD5,0xDD,0xFF,0xC1,0xF5,0xC9,0xFF,0xE3,0xDD,
0xE3,0xFF,},//5
{0xFF,0xFF,0x07,0x03,0x73,0x73,0x73,0xFF,0xFF,0xFF,0xFF,0x83,0xEB,0xE3,0xFF,0xC7,
0xBB,0x9B,0xA7,0xFF,0x83,0xEB,0x93,0xFF,0xFF,0xFF,0xFF,0xE0,0xC0,0xCE,0xCE,0xC0,
0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xCF,0xC7,0xC3,0xC7,0xCF,0xDF,0xFF,0xFF,
0xFF,0xFF,},//6

{0xFF,0xFF,0xF3,0xF3,0xF3,0x33,0x03,0x83,0xFF,0xFF,0xFF,0xFF,0xA7,0xAB,0xCB,0xFF,
0xFB,0x83,0xFB,0xFF,0xC3,0xBF,0xC3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0xC0,0xFE,
0xFF,0xFF,0xFF,0xFF,0xFF,0xE3,0xDD,0xE3,0xFF,0xC1,0xF5,0xF1,0xFF,0xFD,0xC1,0xFD,
0xFF,0xFF,},//7
{0xFF,0xFF,0xFF,0x87,0x03,0x33,0x33,0x03,0x87,0xFF,0xFF,0xFF,0xFF,0xE3,0xDF,0x83,
0xFF,0x83,0xDF,0x83,0xFF,0x93,0xEF,0x93,0xFF,0xFF,0xFF,0xFF,0xE0,0xC0,0xCE,0xCE,
0xC0,0xE0,0xFF,0xFF,0xFF,0xFF,0xC1,0xF5,0xF1,0xFF,0xE3,0xDD,0xC5,0xFF,0xC1,0xF3,
0xC1,0xFF,},//8
{0xFF,0xFF,0x07,0x03,0x73,0x73,0x03,0x07,0xFF,0xFF,0xFF,0xF3,0x8F,0xF3,0xFF,0x9B,
0xAB,0xB3,0xFF,0xFD,0x85,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCE,0xCE,0xCE,0xC0,
0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFB,0xF3,0xE3,0xC3,0xE3,0xF2,0xFB,0xFF,
0xFF,0xFF,},//9
};
//
const unsigned char pic_KDUClear[]=
{
0xFF,0xFF,0x07,0x03,0xF3,0xF3,0x03,0x07,0xFF,0xFF,0xFF,0xFF,0x3F,0xDF,0xEF,0xF7,
0xC1,0xE3,0xF7,0xFF,0xF7,0xEF,0xDF,0x3F,0xFF,0x00,0xFF,0xFF,0xFF,0xF7,0x03,0x03,
0xFF,0xFF,0xFF,0xFF,0xFF,0x87,0xEB,0x87,0xFF,0x83,0xAB,0xD7,0xFF,0xC7,0xBB,0xBB,
0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xF3,0x73,0x73,0x73,0x03,0x07,0xFF,0xFF,0xFF,
0x07,0x77,0x8F,0xFF,0x07,0x57,0x77,0xFF,0x07,0xD7,0xF7,0xFF,0xFF,0x00,0xFF,0xFF,
0xF3,0xF3,0x73,0x73,0x07,0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,0x77,0x17,0xFF,0x07,
0xDF,0x07,0xFF,0x77,0x07,0x77,0xFF,0x00,0xFF,0xFF,0x03,0x03,0x3F,0x3F,0x03,0x03,
0xFF,0xFF,0xFF,0x7F,0x7F,0x87,0xFF,0x07,0xDF,0x27,0xFF,0x07,0x7F,0x7F,0xFF,0xFF,
0xFF,0xFF,0xE0,0xC0,0xCF,0xCF,0xC0,0xE0,0xFF,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB,0xF7,
0xFF,0xF7,0xE3,0xC1,0xF7,0xFB,0xFD,0xFE,0xFF,0x00,0xFF,0xFF,0xFF,0xDF,0xC0,0xC0,
0xDF,0xFF,0xE3,0xDD,0xDD,0xFF,0xC3,0xF5,0xC3,0xFF,0xC1,0xDF,0xDF,0xFF,0xC1,0xDF,
0xDF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xC0,0xC0,0xCE,0xCE,0xCE,0xCF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xC1,0xDF,0xDF,0xFF,0xFD,0xC1,0xFD,0xFF,0xFF,0x00,0xFF,0xFF,
0xCF,0xCF,0xCE,0xCE,0xE0,0xE1,0xFF,0xC1,0xF3,0xC1,0xFF,0xE3,0xDD,0xE3,0xFF,0xC1,
0xDD,0xE3,0xFF,0xC1,0xD5,0xDD,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0xC0,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD3,0xD5,0xE5,0xFF,0xE3,0xDD,0xCD,0xD3,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0x03,0x03,0x33,0x33,0x33,0x73,0xFF,0xFF,0x83,0xF3,0xE7,0xF3,0x83,0xFF,
0x83,0xF3,0xCF,0x83,0xFF,0xC7,0xBB,0xC7,0xFF,0x00,0xFF,0xFF,0x07,0x03,0x73,0x73,
0x73,0xFF,0xFF,0xFF,0xFF,0x83,0xEB,0xE3,0xFF,0xC7,0xBB,0x9B,0xA7,0xFF,0x83,0xEB,
0x93,0xFF,0xFF,0x00,0xFF,0xFF,0xF3,0xF3,0xF3,0x33,0x03,0x83,0xFF,0xFF,0xFF,0xFF,
0xA7,0xAB,0xCB,0xFF,0xFB,0x83,0xFB,0xFF,0xC3,0xBF,0xC3,0xFF,0xFF,0x00,0xFF,0xFF,
0xFF,0x87,0x03,0x33,0x33,0x03,0x87,0xFF,0xFF,0xFF,0xFF,0xE3,0xDF,0x83,0xFF,0x83,
0xDF,0x83,0xFF,0x93,0xEF,0x93,0xFF,0x00,0xFF,0xFF,0x07,0x03,0x73,0x73,0x03,0x07,
0xFF,0xFF,0xFF,0xF3,0x8F,0xF3,0xFF,0x9B,0xAB,0xB3,0xFF,0xFD,0x85,0xF3,0xFF,0xFF,
0xFF,0xFF,0xCF,0xCF,0xCF,0xCF,0xC0,0xE0,0xFF,0xCD,0xD5,0xD9,0xFF,0xC1,0xD5,0xDD,
0xFF,0xC1,0xF5,0xC9,0xFF,0xE3,0xDD,0xE3,0xFF,0x00,0xFF,0xFF,0xE0,0xC0,0xCE,0xCE,
0xC0,0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xCF,0xC7,0xC3,0xC7,0xCF,0xDF,0xFF,
0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xC0,0xC0,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,
0xE3,0xDD,0xE3,0xFF,0xC1,0xF5,0xF1,0xFF,0xFD,0xC1,0xFD,0xFF,0xFF,0x00,0xFF,0xFF,
0xFF,0xE0,0xC0,0xCE,0xCE,0xC0,0xE0,0xFF,0xFF,0xFF,0xFF,0xC1,0xF5,0xF1,0xFF,0xE3,
0xDD,0xC5,0xFF,0xC1,0xF3,0xC1,0xFF,0x00,0xFF,0xFF,0xFF,0xCE,0xCE,0xCE,0xC0,0xE0,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFB,0xF3,0xE3,0xC3,0xE3,0xF2,0xFB,0xFF,0xFF,
};


//

