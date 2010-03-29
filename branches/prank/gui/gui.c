/* Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+tylyos@gmail.com>
 * Copyright (C) 2008 Luca Salmin
 *
 * This file is part of TylyOS.
 *
 * TylyOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TylyOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TylyOS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <drivers/screen/screen.h>
#include <lib/string.h>
#include <kernel/stdio.h>
#include <memory/memory.h>
#include <lib/string.h>
#include <kernel/stdio.h>
#include <kernel/kernel.h>

#include <drivers/screen/vga.h>

#include "mandelbrot.h"
#include "gui.h"
#include "font.h"



void gui_writeChar(char ch, unsigned int _x, unsigned int _y, unsigned char color) {
  unsigned int c = 0;
  #ifdef EIGHT
  for (unsigned int y = 0; y < 8; y++)
    for (unsigned int x = 0; x < 8; x++){
      if (_x + (8 - x) < VGA_width && _y + y < VGA_height){
        if (getBitExt((unsigned int*)g_8x8_font, ((8*8)*ch) + c) == 1)
           VGA_address[VGA_width*(_y+y)+(_x+(8-x))] = color;
        else
            VGA_address[VGA_width*(_y+y)+(_x+(8-x))] = gui_background;
        c++;
      }
    }
  #else
   #ifdef FOUR
  for (unsigned int y = 0; y < 4; y++)
    for (unsigned int x = 0; x < 4; x++){
      if (_x + (4 - x) < VGA_width && _y + y < VGA_height){
        if(g_4x4_font[(int)ch] & 1<<c)
          VGA_address[VGA_width*(_y+y)+(_x+(4-x))] = color;
        else
            VGA_address[VGA_width*(_y+y)+(_x+(4-x))] = gui_background;
      }
      c++;
    }
    #else
    c=4;
  for (unsigned int y = 0; y < 5; y++)
    for (unsigned int x = 0; x < 4; x++){
      if (_x + (4 - x) < VGA_width && _y + y < VGA_height){
        if(g_4x5_font[(int)ch*3+y/2] & 1<<c)
          VGA_address[VGA_width*(_y+y)+(_x+(4-x))] = color;
        else
            VGA_address[VGA_width*(_y+y)+(_x+(4-x))] = gui_background;
      }
      c=(c+1)%8;
    }
    #endif
  #endif
}

void gui_writeString(char *s, unsigned int x, unsigned int y,unsigned char color) {
  unsigned int count = strlen(s);

  for(unsigned int c = 0;c < count;c++)
    gui_writeChar(s[c], x + VGA_dx*c, y,color);
}