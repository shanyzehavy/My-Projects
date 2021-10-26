# Welcomre to Mamat, 044101
#
# This is the project's Makefile. It contains the instructions for compiling
# the projects C files, and then link the generated objects into an executable.
# This simple file will compile "main.c" to "main.o" and then link it (with C
# runtime library) to the executable "prog.exe".
#
# To compile, open the terminal in the folder that contains
# this file and run "make".
#
# To clean, open the terminal in the folder that contains
# this file and run "make clean".
#
# All eclipse projects in this machine are under /home/mamat/Documents/

CC     = gcc
CFLAGS = -Wall -g -std=c99
OBJS   = grades.o
SHARED = -llinked-list


libgrades.so: grades.o
	$(CC) -shared grades.o -o libgrades.so $(SHARED) -L.

grades.o: grades.c
	$(CC) $(CFLAGS) -c -fpic grades.c

clean:
	rm -fr $(OBJS) libgrades.so

  
