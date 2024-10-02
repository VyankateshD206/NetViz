# defining compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -Iinclude

# Defining the target executable
TARGET = NetViz_executable

# Defining the source files
SRCS = src/main.cpp src/Device.cpp src/Network.cpp

# Defining the object files
OBJS = main.o Device.o Network.o

# Rule to build the target executable

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object file from source files

main.o: src/main.cpp include/Network.h include/Device.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp

Device.o: src/Device.cpp include/Device.h
	$(CXX) $(CXXFLAGS) -c src/Device.cpp

Network.o: src/Network.cpp include/Network.h
	$(CXX) $(CXXFLAGS) -c src/Network.cpp

# Clean rule to remove the object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
