#ifndef MM_VIEW_H
#define MM_VIEW_H
#include "q_v.h"
#include "int_q_t.h"
#include "input.h"

#include "FCS152_KDU.h"
#include "lcd.h"

#define maxmenudepth 15
extern int _menustackstorage[maxmenudepth];
extern int_q_t menustack;
void view_init();
void ui_draw( menu * m);
extern menu root;
#endif
