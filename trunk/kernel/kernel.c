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

#include "kernel.h"
#include "tests.h"
#include "multiboot.h"
#include <lib/string.h>
#include <memory/memory.h>
#include <task/task.h>
#include <interrupt/interrupt.h>
#include <drivers/timer/timer.h>
#include <fs/fs.h>

#include <gui/gui.h>

static int on = 1;

static int magicNumber = 0;

static int live=0;

void do_life(void){
  live=1-live;
}

static multiboot_info_t * multiBootInfo;

inline void greendot(void) {
  write(" * ",0);
  put_color_xy(Light_Green,0,1,row(0));
}

void reboot(void) {
  on = 0;
}

void OK(int i) {
  write_xy("[ ok ]",0,COLUMNS - 6, i);
  put_color_xy(Blue,0,COLUMNS - 6, i);
  put_color_xy(Light_Green,0,COLUMNS - 4, i);
  put_color_xy(Light_Green,0,COLUMNS - 3, i);
  put_color_xy(Blue,0,COLUMNS - 1, i);
}

void NO(int i) {
  write_xy("[ NO ]",0,COLUMNS - 6, i);
  put_color_xy(Blue,0,COLUMNS - 6, i);
  put_color_xy(Light_Red,0,COLUMNS - 4, i);
  put_color_xy(Light_Red,0,COLUMNS - 3, i);
  put_color_xy(Blue,0,COLUMNS - 1, i);
}

void kwrite(const char * string) {
  static char * kpointer = (char *) 0xb8000;
  int k;
  for (k = 0;
       string[k] != 0 && k < (COLUMNS*ROWS);
       k++) {
      * (kpointer++) = string[k];
      * (kpointer++) = 7;
    }
}

void logo(void) {
    write("                           ___              _    _ \n",0);
    write("                            |       |      / \\  (  \n",0);
    write("                            |   \\/  |  \\/  \\_/  _) \n",0);
    write("                                /      /           \n",0);
    write("\n",0);
}

int check(const char * output, int offset) {
  unsigned int retval = 1, i;
  for (i = 0;i < strlen(output);i++)
    if (read_x(i + 3 + offset,0) != output[i]) {
        retval = 0;
        put_color_x(Light_Red,0,i + 3 + offset);
      }
  return retval;
}

int magictest(void) {
  printf(0,"Test magic number: %x", magicNumber);
  return magicNumber == 0x2BADB002;
}

int mbdtest(void) {
  int l = multiBootInfo->mem_lower;
  int u = multiBootInfo->mem_upper;
  if (multiBootInfo->flags & 1){
      printf(0,"Lower memory:%dKb. ",l);
      printf(0,"Upper memory:%dMb. ",u/1024);
      printf(0,"Total memory:%dMb and %dKb.",(l + u) / 1024,(l + u) % 1024);
  }
  return l > 0;
}

void magic(void) {
  int t = row(0);
  NO(t);
  greendot();
  if (magictest())
    OK(t++);
  else
    t++;
  printf(0,"\n");
  NO(t);
  greendot();
  if (mbdtest())
    OK(t++);
  else
    t++;
  printf(0,"\n");
}

void _kmain(multiboot_info_t* mbd, unsigned int magicN) {
  int t = 0;/*test number*/


  magicNumber = magicN;
  multiBootInfo = mbd;

  memoriaFisica = multiBootInfo->mem_lower + multiBootInfo->mem_upper;
  memoriaFisica *= 1024;

  set_physical_color(White|Back_Black);
  clear_all();

  NO(t);
  kwrite("Kernel caricato.");
  OK(t++);
  write("\n",0);

  NO(t);
  write("Prova writeline.\n",0);
  OK(t++);

  logo();
  t += 5;

  NO(t);
  greendot();
  write("Inizializzazione:",0);

  write(" GDT",0);
  initGdt();

  write(" IDT",0);
  initIdt();

  write(" PIT",0);
  initTimer();

  write(" Paging\n",0);
  initPaging();
  OK(t++);

  /*here start the true tests*/

  doTests(); //from now on printf is ok

  magic();

  t = row(0);

  NO(t);
  greendot();
  printf(0,"Inizializzazione gestore devices\n");
  initDeviceFsManager();
  OK(t++);

  NO(t);
  greendot();
  printf(0,"Kernel pronto!!!\n");
  OK(t++);

  /*drawRectangle(0,t,COLUMNS-1,ROWS-t-2,(char)(Yellow|Back_Blue));*/
  goto_xy(0, 1, t);
  asm("sti");
  write_physical_xy("[s][c][a][n][k] Time:",0, ROWS - 1);
  on = 1;

  /* test kernel panic
   while(1)
   {
      kmalloc(1024*1024);
   }*/
  /*kernelPanic("_kmain()","this is a test message");*/
  void life();
  while (on){if(live)life();}
  
  /*wait 3 seconds before halting*/
  sleep(3000);

  asm("cli");
  clearIdt();
  asm("int $1");
}


void kernelPanic(char *sender, char *message) {

  asm("cli");
  set_physical_color(Yellow | Back_Red);
  clear_physical();
  goto_current_xy(0,3);
  write_current("RED SCREEN OF DOOM");
  goto_current_xy(0, 7);
  write_current("Dear user,\n");
  write_current("I am very sorry I haven't written for so long. ");
  write_current("I am writing to tell you that \n");
  write_current(message);
  write_current("\nMuch love, ");
  write_current(sender);
  set_cursor(100, 100);
  while (1);
}
