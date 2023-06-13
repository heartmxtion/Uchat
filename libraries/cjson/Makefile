NAME = libcjson.a

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
INC_FILES = $(wildcard $(INC_DIR)/*.h)

CC = clang

# CFLAGS = -std=gnu11 -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: $(NAME)

$(NAME): $(OBJ_FILES)
	ar rcs $@ $^

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

uninstall:
	rm -rf $(OBJ_DIR)
	rm -rf $(NAME)

reinstall: uninstall all
