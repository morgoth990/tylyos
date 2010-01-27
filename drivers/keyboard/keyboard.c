/* Copyright (C) 2008 Luca Salmin
 * Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+clearos@gmail.com>
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

#include "keyboard.h"
#include <memory/memory.h>
#include <kernel/stdio.h>
#include <kernel/kernel.h>
#include <drivers/screen/screen.h>
#include <lib/string.h>

unsigned char kmode = 0;

#include "keymaps.in"

unsigned short escape = 0;
unsigned short shift = 0;
unsigned short ctrl = 0;
unsigned short alt = 0;
unsigned short numlock = 0;
unsigned short capslock = 0;

#define KEYBUFSIZE COLUMNS

static char buffer[CONSOLE][KEYBUFSIZE+1];
static int inpointer[CONSOLE];
static int outpointer[CONSOLE];

//#define FREEROAMING

static int stop_output;

int modifier(char c, int released) {
  if (c == 0x36 || c == 0x2A) {
      if (released)
        shift = 0;
      else
        shift = 1;
      if (!stop_output)
        put_physical_xy(shift ? 'S' : 's', 1, ROWS - 1);
      return 1;
    }
  if (c == 0x1D) {
      if (released)
        ctrl = 0;
      else
        ctrl = 1;
      if (!stop_output)
        put_physical_xy(ctrl ? 'C' : 'c', 4, ROWS - 1);
      return 1;
    }
  if (c == 0x38) {
      if (released)
        alt = 0;
      else
        alt = 1;
      if (!stop_output)
        put_physical_xy(alt ? 'A' : 'a', 7, ROWS - 1);
      return 1;
    }
  if (c == 0x45) {
      if (!released) {
          numlock = 1 - numlock;
          if (!stop_output)
            put_physical_xy(numlock ? 'N' : 'n', 10, ROWS - 1);
        }
      return 1;
    }
  if (c == 0x3A) {
      if (!released) {
          capslock = 1 - capslock;
          if (!stop_output)
            put_physical_xy(capslock ? 'K' : 'k', 13, ROWS - 1);
        }
      return 1;
    }
  return 0;
}

/*void backspace(void) {
  gotoi(pos() - 1);
  puti(pos(), ' ');
}*/

#define cur current_console()

void input(char ch, int released) {
#ifdef FREEROAMING
  if (ch == '\b') {
      goto_x(col() - 1);
      put_x(' ', col());
      return;
    }
  if (ch == '\n') {
      nl();
      return;
    }
#endif

  if (!released) {
      if (((inpointer[cur] + 1) % KEYBUFSIZE) == outpointer[cur])
        return;//no space left for moar
      buffer[cur][inpointer[cur]] = ch;
      inpointer[cur]=(inpointer[cur]+1)%KEYBUFSIZE;
    }
#ifdef FREEROAMING
  put(ch);
#endif
}

#define inc() x++;if(x==COLUMNS){x=0;y++;}
#define slashwrite(a) write_xy("\\",console,x,y);inc();write_xy(a,console,x,y);inc();

//Write a string, with escaped characters
static void Swrite_xy(const char* string, unsigned int console, unsigned int x, unsigned int y) {
  int k;
  if (stop_output)
    return;
  for (k = 0;string[k] != 0;k++) {
      switch (string[k]) {
        case '\n':
          slashwrite("n");
          break;
        case '\b':
          slashwrite("b");
          break;
        case '\\':
          slashwrite("\\");
          break;
        default:
          put_xy(string[k],console, x, y);
          inc();
        }
    }
}

static inline int freeroaming(char ch) {
  switch (ch) {
    case '7':
#ifdef FREEROAMING
      goto_x(0);
#endif
      return 1;
    case '8': /* freccia su */
#ifdef FREEROAMING
      if (row() > 0)
        goto_y(row() - 1);
#endif
      return 1;
    case '9': /* pag su */
#ifdef FREEROAMING
      if (alt)
        scroll(-20);
#endif
      return 1;
    case '4': /* freccia sx */
#ifdef FREEROAMING
      if (col() > 0)
        goto_x(col() - 1);
#endif
      return 1;
    case '5':
      return 1;
    case '6': /* freccia dx */
#ifdef FREEROAMING
      if (col() < COLUMNS - 1)
        goto_x(col() + 1);
#endif
      return 1;
    case '1':
#ifdef FREEROAMING
      goto_x(COLUMNS - 1);
#endif
      return 1;
    case '2': /* freccia giu' */
#ifdef FREEROAMING
      if (row() < ROWS*PAGES - 1)
        goto_y(row() + 1);
#endif
      return 1;
    case '3': /* pag giu' */
#ifdef FREEROAMING
      if (alt)
        scroll(20);
#endif
      return 1;
    case '0':
      /*TODO*/
      return 1 ;
    case '.': /* canc */
#ifdef FREEROAMING
      put(' ');
      goto_x(col() - 1);
#endif
      return 1;
    }
  return 0;
}

void keypress(void) {
  unsigned char c = inb(0x60);
  unsigned char ch = 0;
  unsigned char released = 0;
  static int init = 0;
  if (!init) {
      for (int c = 0; c < CONSOLE;c++) {
          for (init = 0;init <= KEYBUFSIZE;init++)
            buffer[c][init] = 0;
          inpointer[c] = 0;
          outpointer[c] = 0;
        }
      stop_output = 0;
    }
  /*identifica l'inizio di uno scancode a due byte*/
  if (c == 0xE0) {
      escape = 1;
      return;
    }
  /*se e' un evento: tasto rilasciato*/
  if (c&0x80) {
      c &= ~0x80;
      released = 1;
      ch = 0;
    }
  if (c > 0x80)
    printf(current_console(),"(!)|%d|", c);   /*should NEVER happen*/
  /*se e' un tasto modificatore es: shitf*/
  if (modifier(c, released)) {
      escape = 0;/*HACK: in modo da gestire ugualemente sia alt destro che sinistro*/
      return;
    }
  /*
    se e' stato notificato una scancode a due byte precedentemente
    viene letto il secondo scancode ed interpretato
  */
  if (escape) {
      escape = 0;
      if (released)
        return;
      ch = key_map[c];
      //checks wheter a special key was pressed
      if (freeroaming(ch))
        return;
    }
  else {
      if (ctrl && alt) {
          ch = altgr_map[c];
          switch (c) {
            case 0x13: /* R for restart */
              ch = 0;
              write_physical_xy("Rebooting", (COLUMNS - 9) / 2, 0);
              reboot();
              break;
            case 0x23: /* H for halt */
              ch = 0;
              write_physical_xy("Halting", (COLUMNS - 7) / 2, 0);
              reboot();//TODO: implement halt
              break;
            case 0x20:/*D for doom */
              ch = 0;
              write_physical_xy("Dooming", (COLUMNS - 7) / 2, 0);
              kernelPanic("your system", "an invalid operation has happened at unknown address!!! PEBKAC!!!");
              break;
            case 0x2C:/*Z for life*/
              ch = 0;
              write_physical_xy("r for \"randomize\", p for pause", 0, ROWS - 1);
              stop_output = 1;
              do_life();
              break;
              /*default:
                printf("(%x]",c);
                break;*/
            }
        }
      else {
          if (alt) {
              ch = altgr_map[c];
            }
          else {
              /* se e' premuto shift */
              if (shift) {
                  ch = shift_map[c];
                  if (capslock && ch >= 'A' && ch <= 'Z')
                    ch -= 'A' -'a';
                }
              else {
                  /*se non e' attivo nessun modificatore*/
                  ch = key_map[c];
                  if (capslock && ch >= 'a' && ch <= 'z')
                    ch += 'A' -'a';
                }
            }
        }
    }
  if (!released) {
      if (ch != 0)
        input(ch, released);
      else {
          if (c >= 59 && c <= 68)
            switch_console(c - 59);
          else if (c == 87)
            switch_console(10);
          else if (c == 88)
            switch_console(11);
#ifdef FREEROAMING
          else if (escape)
            printf("(E%d)", c);
          else
            printf("(%d)", c);
#endif
        }
    }
#ifdef PUT_ON_KEY_RELEASE
  else
    printf("(R%d)", ch);
#endif
#ifndef FREEROAMING
  Swrite_xy(buffer[cur],cur, 0, ROWS - 3);
#endif
}

char getch() {
  if (inpointer[cur] == outpointer[cur])
    return '\0';
  char toret = buffer[cur][outpointer[cur]];
  buffer[cur][outpointer[cur]] = ' ';
  outpointer[cur] = ((outpointer[cur]) + 1) % KEYBUFSIZE;
#ifndef FREEROAMING
  Swrite_xy(buffer[cur],cur, 0, ROWS - 3);
#endif
  return toret;
}

char fetchch() {
  if (inpointer[cur] == outpointer[cur])
    return '\0';
  return buffer[cur][outpointer[cur]];
}
