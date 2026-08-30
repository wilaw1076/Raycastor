CXX = g++

CXXFLAGS = -std=c++17 -Iinclude
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32

SOURCES = main.cpp map.cpp player.cpp raycaster.cpp render3d.cpp src/glad.c

TARGET = main.exe

all:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)