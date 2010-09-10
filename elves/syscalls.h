#include <kernel/syscall.h>

int openFile(char *path,char mode)
{
    int ret;
    syscalltwo(252,path,mode);
    asm( "mov %%eax, %0;"
          :"=r"(ret)
       );
    return ret;
}

void closeFile(int file)
{
    syscallone(253,file);
}

unsigned int readFile(int file,char *buffer,unsigned int byteCount)
{
    int ret;
    syscallthree(254,file,buffer,byteCount);
    asm( "mov %%eax, %0;"
          :"=r"(ret)
       );
    return ret;
}

unsigned int writeFile(int file,char *buffer,unsigned int byteCount)
{   
    int ret;
    syscallthree(201,file,buffer,byteCount);
    asm( "mov %%eax, %0;"
          :"=r"(ret)
       );
    return ret;
}

unsigned int strlen(char *str)
{
    unsigned int ret=0;
    while(str[ret]!=0)
        ret++;
    return ret;
}

void printf(char *buffer)
{
    writeFile(2,buffer,strlen(buffer));
}

unsigned int get(char *buffer,unsigned int bufferSize)
{
    return readFile(1,buffer,bufferSize);
}


void pipe(int descriptors[2])
{
    syscallone(200,descriptors);
}
