#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ccn/ccn.h>
#include <ccn/uri.h>
#include <ccn/seqwriter.h>
typedef struct PACKED1      //cpu_occupy struct
{
   char name[20];
   unsigned int user;
   unsigned int nice;
   unsigned int system;
   unsigned int idle;
}CPU_OCCUPY;
typedef struct PACKED2      //mem_occupy struct
{
    char name[20];
    unsigned long total;
    char name2[20];
    unsigned long free;
}MEM_OCCUPY;
typedef struct PACKED3      //net occupy struct
{
    char lo[20];
    unsigned long lo_in;
unsigned long lo_out;
char eth[20];
unsigned long eth_in;
unsigned long eth_out;
}NET_OCCUPY;
get_memoccupy (MEM_OCCUPY *mem) //read total memory and unused memory from meminfo
{
FILE *fd;
char buff[256];
MEM_OCCUPY *m;
m=mem;
fd = fopen ("meminfo", "r");
fgets (buff, 64, fd);
sscanf (buff, "%s %lu", m->name, &m->total);
fgets (buff, 64, fd);
sscanf (buff, "%s %lu", m->name2, &m->free);
fclose(fd);
}
get_netoccupy (NET_OCCUPY *net) //get network throughput
{
FILE *fd;
char buff[256];
int a,b,c,d,e,f,g,h,i,j,k,l,m,n;
NET_OCCUPY *t;
t = net;
fd = fopen ("dev", "r");
//fgets (buff, 128, fd);
//fgets (buff, 128, fd);
fgets (buff, 128, fd);
sscanf (buff, "%s %lu %d %d %d %d %d %d %d %lu %d %d %d %d %d %d %d", t->lo,&t->lo_in,&a,&b,&c,&d,&e,&f,&g,&t->lo_out,&h,&i,&j,&k,&l,&m,&n);
fgets (buff, 128, fd);
sscanf (buff, "%s %lu %d %d %d %d %d %d %d %lu %d %d %d %d %d %d %d", t->eth,&t->eth_in,&a,&b,&c,&d,&e,&f,&g,&t->eth_out,&h,&i,&j,&k,&l,&m,&n);
fclose(fd);

}
int cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n)    //calculate cpu usage

{
unsigned long od, nd;
     unsigned long id, sd;
     int cpu_use = 0;
     od = (unsigned long) (o->user + o->nice + o->system +o->idle); 
     nd = (unsigned long) (n->user + n->nice + n->system +n->idle); 
     id = (unsigned long) (n->user - o->user);                  
     sd = (unsigned long) (n->system - o->system);              
     if((nd-od) != 0)
        cpu_use = (int)((sd+id)*10000)/(nd-od);             
     else cpu_use = 0;
     return cpu_use;
}
get_cpuoccupy (CPU_OCCUPY *cpust)       //cpu usage rate

{
        FILE *fd;
        int n;
        char buff[256];
        CPU_OCCUPY *cpu_occupy;
        cpu_occupy=cpust;
        fd = fopen ("stat", "r");
        fgets (buff, sizeof(buff), fd);

        sscanf (buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);
        fclose(fd);
}
static void
usage(const char *progname)
{
        fprintf(stderr,

                "%s [-h] [-b 0<blocksize<=4096] [-r] ccnx:/some/uri\n"

                "    Reads stdin, sending data under the given URI"
                " using ccn versioning and segmentation.\n"
                "    -h generate this help message.\n"
                "    -b specify the block (segment) size for content objects.  Default 1024.\n"
                "    -r generate start-write interest so a repository will"
                " store the content.\n"
                "    -s n set scope of start-write interest.\n"
                "       n = 1(local), 2(neighborhood), 3(everywhere) Default 1.\n",
                progname);
        exit(1);

}
/** make_template: construct an interest template containing the specified scope
 *     An unlimited scope is passed in as 3, and the omission of the scope
 *     field from the template indicates this.
 */
struct ccn_charbuf *
make_template(int scope)
{
    struct ccn_charbuf *templ = NULL;
    templ = ccn_charbuf_create();
    ccn_charbuf_append_tt(templ, CCN_DTAG_Interest, CCN_DTAG);
    ccn_charbuf_append_tt(templ, CCN_DTAG_Name, CCN_DTAG);
    ccn_charbuf_append_closer(templ); /* </Name> */
    if (0 <= scope && scope <= 2)
        ccnb_tagged_putf(templ, CCN_DTAG_Scope, "%d", scope);
    ccn_charbuf_append_closer(templ); /* </Interest> */
    return(templ);

}
char *stringJoin(const char*first, const char* last) //StringJoin函数负责连接两个字符串，将参数列表中的first字符串和last字符串连接起来

{
    char *result;
    int len = strlen(first)+strlen(last)+1;
    result = (char*)malloc(len*sizeof(char));
    memset(result,0,len*sizeof(char));
    strcpy(result,first);
    strcat(result,last);
    return result;
}
int main(int argc, char **argv)
{
    const char *progname = argv[0];
    struct ccn *ccn = NULL;
    struct ccn_charbuf *name = NULL;
    struct ccn_seqwriter *w = NULL;
    int blocksize = 1024;
    int torepo = 0;
    int scope = 1;
    int i;
    int status = 0;
    int res;
    ssize_t read_res;
    size_t blockread;
    char *cpuTemp = malloc(20);
    char *memTemp = malloc(20);
    char *netTemp = malloc(20);
    char *output = malloc(100);
    struct ccn_charbuf *templ;
    torepo = 1; //flag标志位，torepo为1表明系统输出要输出到repositary中
    char *statusUrl = "ccnx:/server8"; //repository输出路径
    name = ccn_charbuf_create();
    res = ccn_name_from_uri(name, statusUrl);

if (res < 0) 
{
        fprintf(stderr, "%s: bad CCN URI: %s\n", progname, statusUrl);
        exit(1);
    }
    ccn = ccn_create();
    if (ccn_connect(ccn, NULL) == -1) {
        perror("Could not connect to ccnd");
        exit(1);
    }
            CPU_OCCUPY cpu_stat1;
 //声明存储cpu占用率、内存占用率、网络连接占用率变量
            CPU_OCCUPY cpu_stat2;
            MEM_OCCUPY mem_stat;
            NET_OCCUPY net_stat;
            int cpu = 0;
           // while(1)
           // {
                 w = ccn_seqw_create(ccn, name);
                    if (w == NULL) {
                        fprintf(stderr, "ccn_seqw_create failed\n");
                       exit(1);
                    }
                    ccn_seqw_set_block_limits(w, blocksize, blocksize);
                    if (torepo) {
                        struct ccn_charbuf *name_v = ccn_charbuf_create();
                        ccn_seqw_get_name(w, name_v);
                        ccn_name_from_uri(name_v, "%C1.R.sw");
                        ccn_name_append_nonce(name_v);
                        templ = make_template(scope);
                        res = ccn_get(ccn, name_v, templ, 60000, NULL, NULL, NULL, 0);
                        ccn_charbuf_destroy(&templ);
                        ccn_charbuf_destroy(&name_v);

                        if (res < 0)
 {
                        fprintf(stderr, "No response from repository\n");
                            exit(1);
                        }
                    }
            blockread = 0;
            get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
 //通过get_cpuoccupy函数，调用linux系统函数获得cpu使用情况
                sleep(10); 
//令进程睡眠10秒
                get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2); 
//获得十秒后的cpu使用情况
                cpu = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2); 
//借助于两组cpu使用情况，计算cpu利用率
                sprintf(cpuTemp,"%d \t",cpu);
 //将cpu这个整形变量赋给cpuTemp，并转换为string类型
            get_memoccupy ((MEM_OCCUPY *)&mem_stat);
  //获得内存使用情况
            sprintf(memTemp,"%lu\t%lu\t", mem_stat.total, mem_stat.free);
//将内存使用情况int值转为memTemp，sting类型
        get_netoccupy ((NET_OCCUPY *)&net_stat);
   //获得网络数据包流量

        sprintf(netTemp,"%lu\t%lu\t%lu\t%lu",net_stat.lo_in, net_stat.lo_out, net_stat.eth_in, net_stat.eth_out); 
//将数据包int值转换为string，赋给netTemp
            output = stringJoin(cpuTemp,memTemp); 
            output = stringJoin(output,netTemp);
 //通过两次字符串连接，将所有数据转换为一个string类型。
            blockread = strlen(output); //计算最终输出string变量长度
            res = ccn_seqw_write(w, output, blockread); //将字符串append到ccn_seqwriter结构体w的buffer上 
         while (res == -1) {
                         ccn_run(ccn, 100);

                             res = ccn_seqw_write(w, output, blockread); 

                       }    

            if (res != blockread)
            abort();  //hmm, ccn_seqw_write did a short write or something
            blockread = 0;
    // flush out any remaining data and close
    if (blockread > 0) {
        res = ccn_seqw_write(w, output, blockread);
        while (res == -1) {
            ccn_run(ccn, 100);
            res = ccn_seqw_write(w, output, blockread);
        }
    }
    ccn_seqw_close(w); //将ccn_seqwriter结构体w中的buffer中数据写入到repositary中
     ccn_run(ccn, 1);
    sleep(5);
           // }
    free(output); //释放一系列资源
    free(cpuTemp);
    free(memTemp);
    free(netTemp);
    output = NULL;
    cpuTemp = NULL;
    memTemp = NULL;
    netTemp = NULL;
    ccn_charbuf_destroy(&name);
    ccn_destroy(&ccn);
         printf("%d",res);
    //exit(status);
}
