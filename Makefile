# Remove the broken Makefile
rm Makefile

# Create a new one with proper tabs
cat > Makefile << 'EOF'
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = gatorAirTrafficScheduler
SOURCES = main.cpp Scheduler.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Scheduler.o: Scheduler.cpp
	$(CXX) $(CXXFLAGS) -c Scheduler.cpp

clean:
	rm -f $(OBJECTS) $(TARGET) *_output_file.txt

.PHONY: all clean
EOF