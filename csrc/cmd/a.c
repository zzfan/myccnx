/*************************************************************************
	> File Name: a.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Mon 13 Jan 2014 07:45:14 PM HKT
 ************************************************************************/

#include<stdio.h>

#include<ccn/fetch.h>
struct MyParms
{
    struct ccn_fetch *f;
    int ccnFD;
    std::string src;
    std::string dst;
    FILE *debug;
    int resolveVersion;
    int appendOut;
    int assumeFixed;
    int maxSegs;
};
struct SelectDataStructure
{
    int fdLen;
    fd_set readFDS;
    fd_set writeFDS;
    fd_set errorFDS;
    struct timeval selectTimeout;
};

void run(struct MyParms *p)
{
    struct SelectDataStructure sds;
    int timeoutUsecs = 100;
    sds.selectTimeout.tv_sec = (timeoutUsecs/1000000);
    sds.selectTimeout.tv_usec = (timeoutUsecs%1000000);
    TestElem e = NewElem(p);
    for(;;) {
        FD_ZERO(&sds.readFDS);
        FD_ZERO(&sds.writeFDS);
        FD_ZERO(&sds.errorFDS);
        sds.fdLen = p->ccnFD+1;
        FD_SET(p->ccnFD, &sds.readFDS);
        FD_SET(p->ccnFD, &sds.writeFDS);
        FD_SET(p->ccnFD, &sds.errorFDS);
        int res = select(sds.fdLen,
                &sds.readFDS,
                &sds.writeFDS,
                &sds.errorFDS,
                &sds.selectTimeout
                );
    }
}

int main(int argc, char **argv)
{
    struct ccn *h = ccn_create();
    ccn_connect(h, NULL);
    struct ccn_fetch *f = ccn_fetch_new(h);
    int fd = ccn_get_connection_fd(h);
    struct MyParms p = {0};
    p.resolveVersion = CCN_V_HIGH;
    p.f = f;
    p.ccnFD = fd;
    p.maxSegs = 4;
    p.src = "Hello, CCNX";
    run(&p);
    f = ccn_fetch_destroy(f);
    ccn_disconnect(h);
    ccn_destroy(&h);
    return 0;
}
