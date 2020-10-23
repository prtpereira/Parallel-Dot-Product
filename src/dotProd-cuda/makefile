CC=nvcc
CFLAGS=-Wall -O3

BIN_NAME=dotProd-cuda

SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.cu)
OBJ = $(patsubst src/%.cu,build/%.o,$(SRC))
DEPS = $(patsubst build/%.o,build/%.d,$(OBJ))

vpath %.cu $(SRC_DIR)

################################################################################

ifeq ($(DEBUG),yes)
	CFLAGS += -ggdb3
endif

.DEFAULT_GOAL = all

$(BUILD_DIR)/%.d: %.cu
	$(CC) -M $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.cu
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

$(BIN_DIR)/$(BIN_NAME): $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJ) $(LIBS)

checkdirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

all: checkdirs $(BIN_DIR)/$(BIN_NAME)

clean:
	rm -f $(BUILD_DIR)/* $(BIN_DIR)/* 
