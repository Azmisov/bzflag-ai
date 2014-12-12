SOURCE_DIR := source
HEADER_DIR := include
OBJECT_DIR := build
#LIBS := include/glfw3/libglfw3.a include/freeimage/libfreeimage.a -lGL -lX11 -lXxf86vm -lm -lpthread -lXrandr -lXi
LIBS := -lm -lpthread

CC := g++
CFLAGS := -std=c++11 -I$(HEADER_DIR)

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp) $(wildcard $(SOURCE_DIR)/Tanks/*.cpp)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

#TODO: fix this to use auto dependencies
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

make: $(OBJECTS)
	$(CC) -o agent $(OBJECTS) -I$(HEADER_DIR) $(LIBS) $(BIT) -O3 -march=native

clean:
	rm -rf $(OBJECT_DIR)/*
	mkdir -p build/Tanks
