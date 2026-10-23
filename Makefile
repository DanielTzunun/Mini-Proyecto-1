CC         := gcc
FLEX       := flex
BISON      := bison
BISONFLAGS := -d

BUILD_DIR   := build
SRC_DIR     := src
INCLUDE_DIR := include
TARGET      := nova
EXAMPLE     := ejemplos/correcto.nv

CFLAGS  := -std=gnu11 -Wall -Wextra -O2 -I$(INCLUDE_DIR) -I$(BUILD_DIR) -I$(SRC_DIR)/parser
LDFLAGS :=

SOURCES := $(wildcard $(SRC_DIR)/*.c \
                     $(SRC_DIR)/ast/*.c \
                     $(SRC_DIR)/lexer/*.c \
                     $(SRC_DIR)/parser/*.c \
                     $(SRC_DIR)/semantico/*.c)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h $(SRC_DIR)/*/*.h)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

YACC_SRC := $(SRC_DIR)/parser/parser.y
LEX_SRC  := $(SRC_DIR)/lexer/lexer.l
BISON_C  := $(BUILD_DIR)/parser.tab.c
BISON_H  := $(BUILD_DIR)/parser.tab.h
LEX_C    := $(BUILD_DIR)/lex.yy.c

OBJECTS += $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lex.yy.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) $(BISON_H) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.tab.o: $(BISON_C) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lex.yy.o: $(LEX_C) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-sign-compare -c $< -o $@

$(BISON_C): $(YACC_SRC) | $(BUILD_DIR)
	$(BISON) $(BISONFLAGS) -o $(BISON_C) $(YACC_SRC)

$(BISON_H): $(BISON_C)
	@test -f $@ || $(BISON) $(BISONFLAGS) -o $(BISON_C) $(YACC_SRC)

$(LEX_C): $(LEX_SRC) $(BISON_H) | $(BUILD_DIR)
	$(FLEX) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET) $(EXAMPLE)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
