CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

SRC = src/main.cpp src/LogEntry.cpp src/LogLinkedList.cpp src/LogBST.cpp src/LogHashTable.cpp src/LogManager.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = log_monitoring

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(TARGET).exe
