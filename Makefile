SRC := src
BUILD := build
TARGET := twwstuff


SRC_DIRS := $(shell find $(SRC) -type d)
SRC_CPP := $(shell find $(SRC) -name *.cpp)
SRC_C := $(shell find $(SRC) -name *.c)

$(shell mkdir -p $(SRC_DIRS:$(SRC)%=$(BUILD)%))

OBJS = $(SRC_CPP:$(SRC)/%.cpp=$(BUILD)/%.o)


$(BUILD)/%.o : $(SRC)/%.cpp
	g++ -o $@ -g -c $<

all: $(OBJS)
	g++ -o $(TARGET) $^

clean:
	rm -rf $(BUILD) $(TARGET)