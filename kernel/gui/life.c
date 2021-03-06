/* Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+tylyos@gmail.com>
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

#include "life.h"
#include <config.txt>
#include <drivers/keyboard/keyboard.h>
#include <drivers/screen/vga.h>

#define HISTORY 10

static char Grid[HISTORY][200][320];
static int round=0;

#define by 100
#define bx 160

static void init(void){
  Grid[0][by][bx]=1;
  Grid[0][by][bx+1]=1;
  Grid[0][by][bx+2]=1;
  Grid[0][by][bx+4]=1;
  Grid[0][by+1][bx]=1;
  Grid[0][by+2][bx+3]=1;
  Grid[0][by+2][bx+4]=1;
  Grid[0][by+3][bx+1]=1;
  Grid[0][by+3][bx+2]=1;
  Grid[0][by+3][bx+4]=1;
  Grid[0][by+4][bx]=1;
  Grid[0][by+4][bx+2]=1;
  Grid[0][by+4][bx+4]=1;
}

int R(int b){
  while(b>=HISTORY)
    b-=HISTORY;
  while(b<0)
    b+=HISTORY;
  return b;
}

static char AG(int a,int b){
  if(a<0 || b<0 || a==200 || b==320)
    return 0;
  return Grid[R(round-1)][a][b];
}

static int G(int a,int b){
  if(a<0 || b<0 || a==200 || b==320)
    return 0;
  return Grid[round][a][b];
}

static char nei[MANDELBROT_COLORS]={0};

#define S(a) if(a>0)nei[a-1]++

//maps neigh. color to new color
static inline char gnu(void){
  #if MANDELBROT_COLORS == 4
  for(int i=0;i<MANDELBROT_COLORS;i++)
    if(nei[i]==2 || nei[i]==3)
      return i+1;
  for(int i=0;i<MANDELBROT_COLORS;i++)
    if(nei[i]==0)
      return i+1;
  #else
    #if MANDELBROT_COLORS == 2
  return nei[0]>nei[1]?1:2;
    #else
  return 1;
    #endif
  #endif
  return 0;
}

static void step(void){
  for(int y=0;y<200;y++)
    for(int x=0;x<320;x++){
      for(int i=0;i<MANDELBROT_COLORS;i++)
        nei[i]=0;
      S(G(y-1,x-1));
      S(G(y-1,x));
      S(G(y-1,x+1));
      S(G(y,x-1));
      S(G(y,x+1));
      S(G(y+1,x-1));
      S(G(y+1,x));
      S(G(y+1,x+1));
      char n=G(y,x);
      char neigh=0;
      for(int i=0;i<MANDELBROT_COLORS;i++)
        neigh+=nei[i];
      if(n){
        if(neigh!=2 && neigh!=3)
          n=0;
      }
      else if(neigh==3)
        n=gnu();
      Grid[R(round+1)][y][x]=n;
    }
    if(round==HISTORY-1)
      round=0;
    else
      round++;
}

//maps memory color to vga color
static inline unsigned char map(char color){
  switch(color){
    case 1:
      return 3;
    case 2:
      return 224;
    case 3:
      return 28;
    case 4:
      return 252;
    case 5:
    case 6:
    case 7:
    case 8:
      return 255;
    default:
      return 1;
  }
}

static void printforce(){
  for(int y=0;y<200;y++)
    for(int x=0;x<320;x++)
      set_pixel(VGA_width*y+x,map(G(y,x)));
}

static void print(){
  for(int y=0;y<200;y++)
    for(int x=0;x<320;x++)
      if(G(y,x)!=AG(y,x))//only if different!!!
        set_pixel(VGA_width*y+x,map(G(y,x)));
}

static int t;

static void randomize(void){
  int a=0;
  t+=2;
  char osc=0;
  for(int y=0;y<200;y++)
    for(int x=0;x<320;x++,a++){
      if(a==t)
        a=0;
      Grid[round][y][x]=(x+y+a)%2;
      if(Grid[round][y][x]){
        Grid[round][y][x]+=osc;
        Grid[round][y][x]%=MANDELBROT_COLORS+1;
        osc++;
        if(osc==MANDELBROT_COLORS)
          osc=0;
      }
    }
}

void editor(void){
  static int x=160;
  static int y=100;
  int ox,oy;
  char g;
  while(1){
    ox=x;oy=y;
    g=getch();
    switch(g){
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
        Grid[round][y][x]=g-'0';
        break;
      case 'w':
        if(y>0)
          y--;
        break;
      case 'a':
        if(x>0)
          x--;
        break;
      case 's':
        if(y<199)
          y++;
        break;
      case 'd':
        if(x<319)
          x++;
        break;
      case 'W':
        if(y>=10)
          y-=10;
        break;
      case 'A':
        if(x>=10)
          x-=10;
        break;
      case 'S':
        if(y<189)
          y+=10;
        break;
      case 'D':
        if(x<309)
          x+=10;
        break;
      case 'q':
        printforce();
        return;
    }
    set_pixel(VGA_width*oy+ox,map(G(oy,ox)));
    set_pixel(VGA_width*y+x,map(G(y,x)+5));
  }
}

void gui_life(void){
  for(int i=0;i<HISTORY;i++)
    for(int y=0;y<200;y++)
      for(int x=0;x<320;x++)
        Grid[i][y][x]=0;
  init();
  t=-1;
  int p=0;
  int frameskip=1;
  int frame=0;
  char g;
  int nostep;
  while(1){
    frame++;
    if(frame==frameskip){
      if(frameskip==1)
        print();
      else
        printforce();
      frame=0;
    }
    do{
      nostep=0;
      g=getch();
      switch(g){
        case 0:
          break;
        case 'a':
          editor();
          break;
        case '*':
          frameskip=1;
          break;
        case '+':
          frameskip++;
          break;
        case '-':
          if(frameskip>1)
            frameskip--;
          break;
        case 'c':
          for(int i=0;i<HISTORY;i++)
            for(int y=0;y<200;y++)
              for(int x=0;x<320;x++)
                Grid[i][y][x]=0;
        case 'p':
          p=1-p;
          printforce();
          break;
        case 'f':
          step();
          nostep=1;
          break;
        case 's':
          if(round==0)
            round=HISTORY-1;
          else
            round--;
          nostep=1;
          break;
        case 'R':
          t=-1;
          break;
        case 'e':
          t-=2;
          randomize();
          nostep=1;
          break;
        case 'w':
          t-=4;
          randomize();
          nostep=1;
          break;
        case 'r':
          randomize();
          nostep=1;
          break;
      }
      if(nostep)
        printforce();
    }while(p);
    if(!nostep)
      step();
  }
}

