/* Copyright (C) 2008 Luca Salmin
 * Copyright (C) 2008 Leonardo Taglialegne <leonardotaglialegne+clearos@gmail.com>
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

#include <memory/memory.h>
#include <lib/string.h>
#include "ramFs.h"

struct deviceFs *newRamFs()
{
    struct deviceFs *pointer=kmalloc(sizeof(struct deviceFs));
    
    strcpy("ramFs",pointer->name);
    
    pointer->getNodeDescriptor = ramFs_getNodeDescriptor;
    pointer->getNodeInfo = ramFs_getNodeInfo;
    pointer->readFile = ramFs_readFile;
    pointer->writeFile = ramFs_writeFile;
    pointer->seek = ramFs_seek;
    pointer->readDir = ramFs_readDir;
    pointer->createFile = ramFs_createFile;
    pointer->deleteFile = ramFs_deleteFile;
    pointer->createDir = ramFs_createDir;
    pointer->deleteDir = ramFs_deleteDir;
    pointer->freeInodeInfoPointer = ramFs_freeInodeInfoPointer;
    
    
    return pointer;
}

void ramFs_getNodeDescriptor(struct deviceFs *device,struct fs_node_descriptor *descriptor,char *path)
{
    descriptor->device=device;
    descriptor->inodeInfo=0;/*TODO: cercare il path, allocare la struttura e passarne il puntatore*/
}
fs_returnCode ramFs_getNodeInfo(struct fs_node_descriptor *descriptor,struct fs_node_info *out)
{
    /*TODO: usare il puntatore contenuto in descriptor per trovare il nodo e caricare le informazioni in out*/
}
unsigned int ramFs_readFile(struct fs_node_descriptor *descriptor,char *buffer,unsigned int byteCount)
{
    /*TODO: leggere i dati contenuti nel nodo, scriverli in buffer e ritornare il numero di bytes scritti*/
}
unsigned int ramFs_writeFile(struct fs_node_descriptor *descriptor,char *buffer,unsigned int byteCount)
{
    /*TODO: scrivere i dati contenuti in buffer nell' area dati del nodo e ritornare il numero di bytes scritti*/
}
unsigned int ramFs_seek(struct fs_node_descriptor *descriptor,int offset)
{
    /*TODO: spostare il puntatore al byte da leggere/scrivere contenuto nella struttura inodeInfo*/
}
fs_returnCode ramFs_readDir(struct fs_node_descriptor *descriptor,struct fs_node_info *out)
{
    /*TODO: scrivere in out i dati del prossimo nodo figlio del nodo descriptor*/
}
fs_returnCode ramFs_createFile(char *name,struct fs_node_descriptor *output,struct fs_node_descriptor fatherNodeDescriptor)
{
}
fs_returnCode ramFs_deleteFile(struct fs_node_descriptor descriptor)
{
}
fs_returnCode ramFs_createDir(char *name,struct fs_node_descriptor *output,struct fs_node_descriptor fatherNodeDescriptor)
{
}
fs_returnCode ramFs_deleteDir(struct fs_node_descriptor descriptor)
{
}
void ramFs_freeInodeInfoPointer(void *inodeInfo)
{
}