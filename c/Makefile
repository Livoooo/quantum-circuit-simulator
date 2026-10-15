CC      := gcc
CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
LDLIBS  := -lm
# Debug par défaut (sanitizers) ; "make MODE=release" pour une version optimisée
MODE ?= debug
ifeq ($(MODE),release)
    CFLAGS  += -O2 -DNDEBUG
else
    CFLAGS  += -g -O0 -fsanitize=address,undefined
    LDFLAGS += -fsanitize=address,undefined
endif

BUILD := build
SRCS  := src/statevector.c main.c
OBJS  := $(SRCS:%.c=$(BUILD)/%.o)
BIN   := test_sv

.PHONY: all test clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(BIN)sanitizers
	./$(BIN)

clean:
	rm -rf $(BUILD)

-include $(OBJS:.o=.d)
