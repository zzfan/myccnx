cc = gcc
RM = rm -f
LDFLAGS = -lccn -lcrypto
all: send
send:  myccnsend.o
	$(CC) -g -o send myccnsend.o $(LDFLAGS)
myccnsend.o: myccnsend.c
	$(CC) -c myccnsend.c $(LDFLAGS)

clean:
	$(RM) send
	$(RM) *.o

