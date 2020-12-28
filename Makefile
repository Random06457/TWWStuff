SRC := src
BUILD := build
TARGET := twwstuff


SRC_DIRS := $(shell find $(SRC) -type d)
SRC_CPP := $(shell find $(SRC) -name *.cpp)
SRC_C := $(shell find $(SRC) -name *.c)

ifndef ($(OPT_FLAGS))
	OPT_FLAGS := -g
endif

LIBS := 
CPP_FLAGS := -Werror

$(shell mkdir -p $(SRC_DIRS:$(SRC)%=$(BUILD)%))

OBJS = $(SRC_CPP:$(SRC)/%.cpp=$(BUILD)/%.o)


$(BUILD)/%.o : $(SRC)/%.cpp
	g++ -o $@ $(OPT_FLAGS) $(CPP_FLAGS) -c $<

all: $(OBJS)
	g++ -o $(TARGET) $(LIBS) $^

clean:
	rm -rf $(BUILD) $(TARGET)