# Makefile for the ttftps program

CXX      =  g++
CXXFLAGS = -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG 
CXXLINK  = $(CXXFLAGS)
OBJS     = ttftps.o Client.o
RM 		 = rm -f

# Creating the  executable
ttftps: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ttftps $(OBJS) -lpthread

# Creating the object files
ttftps.o: ttftps.cpp Client.h 
Client.o: Client.cpp Client.h 
#something1.o: something1.cpp something1.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.txt *.o *~ "#"* core.* ttftps

