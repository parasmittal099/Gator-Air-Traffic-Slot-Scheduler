CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = gatorAirTrafficScheduler
SOURCES = main.cpp Scheduler.cpp
HEADERS = Flight.h PairingHeap.h MinHeap.h Scheduler.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *_output_file.txt

.PHONY: all clean