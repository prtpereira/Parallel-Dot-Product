CC=icc
CFLAGS=-Wall -O3 -std=c99
OMP=-openmp

BIN_NAME:=dotProd

SRC_DIR=src
BUILD_DIR:=build
BIN_DIR=bin
BIN_DIR_PAPI=bin_papi

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
ASS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.s,$(SRC))
DEPS=$(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(SRC))

vpath %.c $(SRC_DIR)

################################################################################

ifeq ($(DEBUG),yes)
	CFLAGS+=-DDEBUG
	#CFLAGS+=-ggdb3
endif

ifeq ($(DOT_PROD_KNL),yes)
	CFLAGS+=-DPHI
	CFLAGS+=$(OMP)
	CFLAGS+=-qoffload-arch=mic
	CFLAGS+=-march=knl
	BIN_NAME:=$(BIN_NAME)-knl
endif

ifeq ($(DOT_PROD_KNC),yes)
	CFLAGS+=-DPHI
	CFLAGS+=$(OMP)
	CFLAGS+=-qoffload-arch=mic
	CFLAGS+=-march=knc
	BIN_NAME:=$(BIN_NAME)-knc
endif

ifeq ($(DOT_PROD_PAR),yes)
	CFLAGS+=-DPAR
	CFLAGS+=-qopt-report-phase=vec -qopt-report=5 -qopt-report-file=par.optr
	CFLAGS+=$(OMP)
	BIN_NAME:=$(BIN_NAME)-par
endif

ifeq ($(DOT_PROD_BLOCK),yes)
	CFLAGS+=-DBLOCK
	CFLAGS+=-qopt-report-phase=vec -qopt-report=5 -qopt-report-file=vec.optr
	BIN_NAME:=$(BIN_NAME)-block
endif

ifdef DOT_PROD
	CFLAGS+=-D$(DOT_PROD)
	BIN_NAME:=$(BIN_NAME)-$(DOT_PROD)
endif

ifeq ($(PAPI),yes)
    CFLAGS += -DD_PAPI
    BIN_DIR = $(BIN_DIR_PAPI)
    INCLUDES += -I/share/apps/papi/5.4.1/include
    LIBS += -L/share/apps/papi/5.4.1/lib
    CFLAGS += -lpapi
endif

################################################################################

.DEFAULT_GOAL = all

$(BUILD_DIR)/%.d: %.c
	$(CC) -M $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

$(BUILD_DIR)/%.s: %.c
	$(CC) -S $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

$(BIN_DIR)/$(BIN_NAME): $(DEPS) $(OBJ) $(ASS)
	$(CC) $(CFLAGS) $(OBJ) $(INCLUDES) -o $@ $(LIBS)

checkdirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BIN_DIR_PAPI)

all: checkdirs $(BIN_DIR)/$(BIN_NAME)

################################################################################

cleanBuild:
	rm -rf $(BUILD_DIR) 

cleanBin:
	rm -rf $(BIN_DIR) 
	rm -rf $(BIN_DIR)_papi

clean: cleanBuild cleanBin
