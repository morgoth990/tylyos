/* Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+clearos@gmail.com>
 * Copyright (C) 2008 Roberto Domenella
 * Copyright (C) 2008 Giordano Cristini
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

#include "string.h"
#include <drivers/screen/screen.h>
#include <memory/memory.h>

unsigned int strlen ( const char * string )
{
    unsigned int ret = 0;
    while ( string[ret] != 0 )
        ret++;
    return ret;
}

void strcpy ( char *source, char *dest )
{
    unsigned int c;
    for ( c = 0;c < strlen ( source );c++ )
        dest[c] = source[c];
    dest[c] = 0;
}

void itoa ( int a, char buff[11] )
{
    itobase ( a, 10, buff );
}

void itobase ( int a, unsigned short base, char * buff )
{
    if ( a == 0 )
    {
        buff[0] = '0';
        buff[1] = 0;
    }
    else
    {
        int i, l;
        char temp[MAXN] = {0};
        if ( a < 0 )
        {
            a = -a;
            buff[0] = '-';
        }
        else
            buff[0] = 0;
        for ( l = 0;a && ( l < MAXN ) ; a /= base, l++ )
            temp[l] = "0123456789ABCDEF"[a % base];
        for ( i = l - 1, l = buff[0] == '-';i >= 0;i-- )
            buff[l++] = temp[i];
        buff[l] = 0;

    }
}

void Swrite( const char* string){
  int k;
  for (k = 0;string[k] != 0;k++){
    switch(string[k]){
      case '\n':
        write("\\n");
        break;
      case '\b':
        write("\\b");
        break;
      case '\\':
        write("\\\\");
        break;
      default:
        put(string[k]);
    }
  }
}

int printf ( const char* format, ... )
{
    int size = 0;
    char ** arg = ( char** ) & format;
    char buf[33];
    unsigned int i;
    arg++;/*jump "format"*/
    for ( i = 0;i < strlen ( format );i++ )
    {
        if ( format[i] == '%' )
        {
            switch ( format[i+1] )
            {
            case 'd':
            {
                itoa ( * ( ( int * ) arg++ ), buf );
                goto number;
                break;
            }
            case 'x':
            {
                itobase ( * ( ( int * ) arg++ ), 16, buf );
                goto number;
                break;
            }
            case 'o':
            {
                itobase ( * ( ( int * ) arg++ ), 8, buf );
                goto number;
                break;
            }
            case 'b':
            {
                itobase ( * ( ( int * ) arg++ ), 2, buf );
                goto number;
                break;
            }
            case 'c':
                put ( * ( ( char * ) arg++ ) );
                size++;
                break;
            case 's':
                write ( ( char* ) *arg );/*watch out: Deep Magic*/
                size += strlen ( ( char * ) * arg++ );
                break;
            case 'S':
                Swrite( ( char* ) *arg );/*some moar Deep Magic*/
                size += strlen ( ( char * ) * arg++ );
            number:
                write ( buf );
                size += strlen ( buf );
                break;
            default:
                itoa ( * ( ( int * ) arg++ ), buf );
                write ( "Malformed format string!" );
                goto number;
                break;
            }

            i++;
        }
        else
        {
            if ( format[i] != '\n' )
                put ( format[i] );
            else
                nl();
            size++;
        }
    }
    return size;
}

