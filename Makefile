SOURCE_DIR := source
HEADER_DIR := include
OBJECT_DIR := build
LIBS := include/glfw3/libglfw3.a include/freeimage/libfreeimage.a -lGL -lX11 -lXxf86vm -lm -lpthread -lXrandr -lXi

CC := g++
CFLAGS := -std=c++11 -I$(HEADER_DIR)

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
HEADERS := $(wildcard $(HEADER_DIR)/*.h)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

#TODO: fix this to use auto dependencies
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: $(OBJECTS)
	$(CC) -o agent $(OBJECTS) -I$(HEADER_DIR) $(LIBS) -O3 -march=native

clean:
	rm -f $(OBJECT_DIR)/*
