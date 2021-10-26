
CXX      =  g++
CXXFLAGS = -g -Wall -std=c++11
OBJS     =  string.o ip.o port.o field.o 
SHARED 	 =  -linput
EXEC     =  firewall.exe


firewall.exe: main.o libfirewall.so libinput.so
	$(CXX) $(CXXFLAGS) main.o -o $(EXEC) -lfirewall -linput -L.

libfirewall.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o libfirewall.so -L.

ip.o: ip.cpp ip.h #string.h
	$(CXX) -c $(CXXFLAGS) -fpic ip.cpp

port.o: port.cpp port.h #string.h
	$(CXX) -c $(CXXFLAGS) -fpic port.cpp 

field.o: field.cpp field.h #string.h
	$(CXX) -c $(CXXFLAGS) -fpic field.cpp 

string.o: string.cpp string.h
	$(CXX) -c $(CXXFLAGS) -fpic string.cpp 

main.o: main.cpp ip.h port.h field.h string.h
	$(CXX) -c $(CXXFLAGS) -fpic main.cpp 

clean:
	rm -fr *.exe $(OBJS) main.o libfirewall.so

