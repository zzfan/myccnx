CC=gcc
RM=rm -f
LDFLAGS=-lccn -lcrypto
all:ccnsend ccnget myccnsend
ccnsend: ccnseqwriter.o
	$(CC) -g -o ccnsend ccnseqwriter.o $(LDFLAGS)
ccnseqwriter.o:ccnseqwriter.c
	$(CC) -c ccnseqwriter.c $(LDFLAGS)
ccnget: ccncat.o
	$(CC) -g -o ccnget ccncat.o $(LDFLAGS)
ccncat.o:ccncat.c
	$(CC) -c ccncat.c $(LDFLAGS)
myccnsend: myccnseqwriter.o
	$(CC) -g -o myccnsend myccnseqwriter.o $(LDFLAGS)
myccnseqwriter.o:myccnseqwriter.c
	$(CC) -c myccnseqwriter.c $(LDFLAGS)

clean:
	$(RM) ccnget
	$(RM) ccnsend
	$(RM) myccnsend
	$(RM) *.o
