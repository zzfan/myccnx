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
typedef struct PACKED2
{
    char name[20];
    unsigned long total;
    char name2[20];
    unsigned long free;
    
}MEM_OCCUPY;

get_memoccupy(MEM_OCCUPY *mem)
{
    FILE *fd;
    char buff[256];
    MEM_OCCUPY *m;
    m=mem;
    fd=fopen("meminfo","r");
    fgets(buff,64,fd);
    sscanf(buff,"%s %lu",m->name,&m->total);
    fgets(buff,64,fd);
    sscanf(buff,"%s %lu",m->name2,&m->free);
    fclose(fd);

}

int main()
{
    MEM_OCCUPY m;
   // MEM_OCCUPY *p=m;
    get_memoccupy(&m);
    printf("%s %lu\n",m.name,m.total);
}


