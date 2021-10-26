CC=gcc
CCLINK=$(CC)
CFLAGS=-g -Wall -std=c99
OBJS=main.o file0.o file1.o file2.o file3.o file4.o
EXEC=prog
RM=rm -rf $(EXEC) $(OBJS)

$(EXEC): $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)
	
main.o: main.c file0.h file1.h file2.h file3.h file4.h
	$(CC) -c $(CFLAGS) main.c

file0.o: file0.c file0.h file2.h file3.h file4.h
	$(CC) -c $(CFLAGS) file0.c

file1.o: file1.c file1.h file0.h file3.h
	$(CC) -c $(CFLAGS) file1.c

file2.o: file2.c file2.h file3.h
	$(CC) -c $(CFLAGS) file2.c

file3.o: file3.c file3.h file1.h
	$(CC) -c $(CFLAGS) file3.c

file4.o: file4.c file4.h file3.h
	$(CC) -c $(CFLAGS) file4.c

clean:
	$(RM)



