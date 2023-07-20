TARGET	= $(shell basename $(CURDIR))
CC		= clang++
CFLAGS	= -Wall -Wextra -Werror -Wshadow -g -std=c++11
DIR		= /Users/tomf/dev
#LIBS	= -L$(DIR)/lib
#INCL	= -I$(DIR)/include
LINK	= -lSDL2
SRC		= $(wildcard *.cc)
OBJ_DIR = ./obj
OBJ		= $(SRC:%.cc=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS) $(LINK)

$(OBJ_DIR)/%.o: %.cc *.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf $(TARGET) $(OBJ_DIR)
