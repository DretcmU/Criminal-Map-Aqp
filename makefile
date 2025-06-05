CXX = g++
CXXFLAGS = -std=c++17 -pthread
TARGET = servidor
SRC = templates/servidor.cpp templates/datastruct/BTree.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
