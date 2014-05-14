/*************************************************************************
	> File Name: fgets.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Tue 08 Apr 2014 08:24:42 PM HKT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<unistd.h>

typedef struct PACKED
{
    char name[40];
    char srcname;
   // unsigned long total;
   // char name2[20];
   // unsigned long free;
    
}MEM_OCCUPY;

typedef struct SRCTABLE
{
    MEM_OCCUPY mem[4];
    int size;

}srctable;


get_memoccupy(MEM_OCCUPY *mem)
{
    FILE *fd;
    char buff[256];
    MEM_OCCUPY *m;
    m=mem;
    fd=fopen("meminfo","r");
    fgets(buff,64,fd);
    sscanf(buff,"%s %c",m->name,&m->srcname);
//    fgets(buff,64,fd);
//    sscanf(buff,"%s %lu",m->name2,&m->free);
    fclose(fd);

}

int main()
{
    srctable src;
    srctable *m;
    m=&src;
   // MEM_OCCUPY *p=m;
    get_memoccupy(m->mem);
    printf("%s %c\n",m->mem[0].name,m->mem[0].srcname);
}


