# Makefile
CC = xtensa-lx106-elf-gcc
CFLAGS = -Os -Wall -Wpointer-arith -Wno-implicit-function-declaration \
         -mlongcalls -nostdlib -ffunction-sections -fdata-sections
LDFLAGS = -nostdlib -Wl,--gc-sections -Wl,-Map=rtos.map

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.S)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
          $(SOURCES:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o)

TARGET = rtos

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -T eagle.app.v6.ld $^ -o $@

all: $(TARGET).elf

clean:
	rm -rf $(OBJ_DIR) $(TARGET).elf $(TARGET).map