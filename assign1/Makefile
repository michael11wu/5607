CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall

all: raycast
clean:
	rm -f *.o *.h.gch raycast raycast.ppm
test: raycast
	./raycast inputfile.txt
.PHONY: all clean test

raycast: raycast.o
	$(CXX) $(LDFLAGS) -o $(@) $(^)

zip:
	

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $(@) $(<)