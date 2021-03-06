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

#ifndef GUI_H
#define GUI_H

char gui_background;

struct bmpfile_magic {
    unsigned char magic[2];
};

struct bmpfile_header {
    unsigned int filesz;
    unsigned int  creator1;
    unsigned int  creator2;
    unsigned int  bmp_offset;
};

void gui_writeChar(char ch,unsigned int _x,unsigned int _y,unsigned char color);
void gui_writeString(char *s,unsigned int x,unsigned int y,unsigned char color);

void gui_printImageFromFile(char *path,int x,int y);

#endif


