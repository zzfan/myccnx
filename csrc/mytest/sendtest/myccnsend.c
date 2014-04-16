/*************************************************************************
	> File Name: myccnsend.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Thu 10 Apr 2014 10:07:45 PM HKT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<ccn/ccn.h>
#include<ccn/uri.h>
#include<ccn/seqwriter.h>

struct ccn_charbuf *
make_template(int scope)
{
    struct ccn_charbuf *templ =NULL;
    templ = ccn_charbuf_create();
    ccn_charbuf_append_tt(templ,CCN_DTAG_Interest,CCN_DTAG);
    ccn_charbuf_append_tt(templ,CCN_DTAG_Name, CCN_DTAG);
    ccn_charbuf_append_closer(templ);
    if (0 <= scope && scope <= 2)
        ccn_charbuf_append_closer(templ);
    return(templ);

}

char *stringJoin(const char* first,const char* last)
{
    char *result;
    int len = strlen(first)+strlen(last)+1;
    result = (char *)malloc(len*sizeof(char));
    memset(result,0,len*sizeof(char));
    strcpy(result,first);
    strcat(result,last);
    return result;
}

int main(int argc,char **argv)
{
    const char *progname =argv[0];
    struct ccn *ccn = NULL;
    struct ccn_charbuf *name =NULL;
    struct ccn_seqwriter *w=NULL;
    int blocksize=1024;
    int torepo = 0;
    int scope =1;
    int i;
    int status =0;
    int res;
    ssize_t read_res;
    size_t blockread;
    struct ccn_charbuf *templ;
    char *output = "hello my send";
    torepo=1;
    char *statusUrl="ccnx:/server2";
    name = ccn_charbuf_create();
    res = ccn_name_from_uri(name,statusUrl);

    if (res < 0)
    {
        fprintf(stderr, "%s:bad CCN URI: %s/n", progname,statusUrl);
        exit(1);
    }
    ccn = ccn_create();
    if (ccn_connect(ccn,NULL)== -1){
        perror("could not connect to ccnd");
        exit(1);
    }
    while(1)
    {
        w = ccn_seqw_create(ccn, name);
        if (w ==NULL)
        {
            fprintf( stderr, "ccn_seqw_create failed\n" );
            exit(1);
        }
        ccn_seqw_set_block_limits(w, blocksize, blocksize);
        if (torepo){
            struct ccn_charbuf *name_v = ccn_charbuf_create();
            ccn_seqw_get_name(w, name_v);
            ccn_name_from_uri(name_v, "%C1.R.sw");
            ccn_name_append_nonce(name_v);
            templ = make_template(scope);
            res = ccn_get(ccn, name_v, templ, 60000, NULL, NULL, NULL,0);
            ccn_charbuf_destroy(&templ);
            ccn_charbuf_destroy(&name_v);

            if(res < 0)
            {
                fprintf(stderr, "no response from repository\n");
                exit(1);

            }
        }
        blockread = 0;
       // w = "hello my test";
        blockread = strlen(output);
        res = ccn_seqw_write(w, output, blockread);
        while(res == -1)
        {
            ccn_run(ccn, 100);
            res = ccn_seqw_write(w, output, blockread);

        }

        if(res != blockread)
        abort();

        blockread = 0;
        if (blockread > 0) {
            res = ccn_seqw_write(w, output, blockread);
            while (res == -1) {
                ccn_run(ccn, 100);
                res = ccn_seqw_write(w, output, blockread);
            }
        }

        ccn_seqw_close(w);
        ccn_run(ccn, 1);
        sleep(5);

    }

    ccn_charbuf_destroy(&name);
    ccn_destroy(&ccn);
    printf("%d", res);

}
