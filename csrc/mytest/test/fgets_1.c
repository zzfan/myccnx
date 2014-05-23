/*************************************************************************
	> File Name: fgets_1.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Wed 16 Apr 2014 10:54:56 AM HKT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<unistd.h>

typedef struct packed
{
    char name[40];
    char srcname;
}MEM_OCCUPY;

typedef struct SRCTABLE
{
    MEM_OCCUPY mem[4];
    int size;
}srctable;

int get_memoccupy(srctable *mem)
{
    FILE *fd;
    char buff[256];
    int i;
    int len;
    srctable *m;
    m=mem;
    fd=fopen("meminfo", "r");
    for(i=0; i < 4; i++){
        fgets(buff, 64, fd);
        len = strlen(buff);
        printf("%c", buff[len-1]);
        sscanf(buff, "%s %c", m->mem[i].name, &m->mem[i].srcname);
    }
   // fgets(buff, 64, fd);
   // sscanf(buff, "%s %c", m->name, &m->srcname);
    fclose(fd);
    return i;
}

int main()
{
    srctable src;
    srctable *m;
    m = &src;
    m->size = get_memoccupy(m);
    printf(" %s %c\n", m->mem[2].name, m->mem[2].srcname);

}


