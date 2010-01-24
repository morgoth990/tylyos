/* Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+clearos@gmail.com>
 * Copyright (C) 2008 Luca Salmin
 *
 * This file is part of ClearOS.
 *
 * ClearOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ClearOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ClearOS.  If not, see <http://www.gnu.org/licenses/>.
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

static multiboot_info_t * multiBootInfo;

inline void greendot(void) {
  write(" * ");
  cputxy(1, row(), Light_Green);
}

void reboot(void) {
  on = 0;
}

void OK(int i) {
  writexy(COLUMNS - 6, i, "[ ok ]");
  cputxy(COLUMNS - 6, i, Blue);
  cputxy(COLUMNS - 4, i, Light_Green);
  cputxy(COLUMNS - 3, i, Light_Green);
  cputxy(COLUMNS - 1, i, Blue);
}

void NO(int i) {
  writexy(COLUMNS - 6, i, "[ NO ]");
  cputxy(COLUMNS - 6, i, Blue);
  cputxy(COLUMNS - 4, i, Light_Red);
  cputxy(COLUMNS - 3, i, Light_Red);
  cputxy(COLUMNS - 1, i, Blue);
}

void kwrite(const char * string) {
  static char * kpointer = (char *) 0xb8000;
  int k;
  for (k = 0;
       string[k] != 0 && (((int) kpointer - 0xb8000) / 2) < (COLUMNS*ROWS);
       k++) {
      * (kpointer++) = string[k];
      * (kpointer++) = 7;
    }
}

void logo(void) {
  gotoxy(25, row());
  writeline("________                ____      ");
  gotoxy(24, row() + 1);
  writeline("/ ____/ /__  ____  _____/ __ \\____");
  gotoxy(23, row() + 1);
  writeline("/ /   / / _ \\/ __ \\/ ___/ / / /  _/");
  gotoxy(22, row() + 1);
  writeline("/ /___/ /  __/ /_/ / /  / /_/ /\\  \\ ");
  gotoxy(22, row() + 1);
  writeline("\\____/_/\\___/\\__._/_/   \\____//___/ ");

  /*
      writeline("                       ___ _                  ___  __   ");
      writeline("                      / __\\ | ___  __ _ _ __ /___\\/ _\\  ");
      writeline("                     / /  | |/ _ \\/ _` | '__//  //\\ \\   ");
      writeline("                    / /___| |  __/ (_| | | / \\_// _\\ \\  ");
      writeline("                    \\____/|_|\\___|\\__,_|_| \\___/  \\__/  ");
  */
}

int check(const char * output, int offset) {
  unsigned int retval = 1, i;
  for (i = 0;i < strlen(output);i++)
    if (readxy(i + 3 + offset, row()) != output[i]) {
        retval = 0;
        cputxy(i + 3 + offset, row(), Light_Red);
      }
  return retval;
}

int magictest(void) {
  printf("Test magic number: %x", magicNumber);
  return magicNumber == 0x2BADB002;
}

int mbdtest(void) {
  int l = multiBootInfo->mem_lower;
  int u = multiBootInfo->mem_upper;
  if (multiBootInfo->flags & 1){
      printf("Lower memory:%dKb. ",l);
      printf("Upper memory:%dMb. ",u/1024);
      printf("Total memory:%dMb and %dKb.",(l + u) / 1024,(l + u) % 1024);
  }
  return l > 0;
}

void magic(void) {
  int t = row() + 1;
  NO(t);
  greendot();
  if (magictest())
    OK(t++);
  else
    t++;
  printf("\n");
  NO(t);
  greendot();
  if (mbdtest())
    OK(t++);
  else
    t++;
  printf("\n");
}

void _kmain(multiboot_info_t* mbd, unsigned int magicN) {
  int t = 0;/*test number*/


  magicNumber = magicN;
  multiBootInfo = mbd;

  memoriaFisica = multiBootInfo->mem_lower + multiBootInfo->mem_upper;
  memoriaFisica *= 1024;


  clearScreen();

  NO(t);
  kwrite("Kernel caricato.");
  OK(t++);
  writeline("");

  NO(t);
  writeline("Prova writeline.");
  OK(t++);

  logo();
  t += 4;

  NO(t);
  greendot();
  write("Inizializzazione:");

  write(" GDT");
  initGdt();

  write(" IDT");
  initIdt();

  write(" PIT");
  initTimer();

  writeline(" Paging");
  initPaging();
  OK(t++);

  NO(t);
  greendot();
  writeline("Inizializzazione gestore devices");
  initDeviceFsManager();
  OK(t++);

  /*here start the true tests*/

  doTests();

  magic();

  t = row() + 1;

  NO(t);
  greendot();
  printf("Kernel pronto!!!\n");
  OK(t++);

  /*drawRectangle(0,t,COLUMNS-1,ROWS-t-2,(char)(Yellow|Back_Blue));*/
  gotoxy(1, t);
  asm("sti");
  writexy(0, ROWS - 1, "[s][c][a][n][k] Time:");
  on = 1;

  /* test kernel panic
   while(1)
   {
      kmalloc(1024*1024);
   }*/
  /*kernelPanic("_kmain()","this is a test message");*/



  while (on);
  {
    /*wait 3 seconds before halting*/
    sleep(3000);

    asm("cli");
    clearIdt();
    asm("int $1");
  }
}


void kernelPanic(char *sender, char *message) {

  asm("cli");
  clearScreenAndColor(Yellow | Back_Red);
  setConsoleColor(Yellow | Back_Red);
  gotoxy(0, 3);
  printf("RED SCREEN OF DOOM");
  gotoxy(0, 7);
  printf("Dear user,\n");
  printf("I am very sorry I haven't written for so long. ");
  printf("I am writing to tell you that \n%s\n", message);
  printf("Much love, %s", sender);
  setCursorPos(100, 100);
  while (1);
}
