# Makefile for the bank program

CXX      =  g++
CXXFLAGS = -std=c++11 -g -Wall -Werror -pedantic-errors -DNDEBUG 
CXXLINK  = $(CXXFLAGS)
OBJS     = Account.o ATMs.o Bank.o
RM 		 = rm -f


# Creating the  executable
Bank: $(OBJS)
	$(CXX) $(CXXFLAGS) -o Bank $(OBJS) -lpthread

# Creating the object files
Bank.o: Bank.cpp Bank.h ATMs.o # ?? Do we need the Accounts? cuz ATMs got 'em
ATMs.o: ATMs.cpp ATMs.h Account.o
Account.o: Account.cpp Account.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* Bank log.txt

