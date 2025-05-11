PROGRAM = spreadsheet
HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
CXX = g++
CXXFLAGS = -pedantic -Wall -Werror -DMEMTRACE -g -MMD

.PHONY: all
all: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(PROGRAM)

$(OBJECTS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o $(PROGRAM) *.d *.csv

.PHONY: run
run:
	./$(PROGRAM)
