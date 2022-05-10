# Makefile for the smash program

CXX      =  g++
CXXFLAGS = -g -Wall 
CXXLINK = $(CXXFLAGS)
OBJS     = commands.o signals.o smash.o job.o
RM = rm -f


# Creating the  executable
smash: $(OBJS)
	g++ -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG -o smash $(OBJS)

# Creating the object files
job.o: job.cpp job.h
commands.o: commands.cpp commands.h job.o
smash.o: smash.cpp commands.h
signals.o: signals.cpp signals.h


# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash

