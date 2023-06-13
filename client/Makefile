# NAME = uchat.a
# NAMEBIN = uchat

# SRC_DIR = src
# OBJ_DIR = obj
# INC_DIR = inc
# LIB_DIR = libmx

# SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
# INC_FILES = $(wildcard $(INC_DIR)/*.h)
# LIB_FILES = $(wildcard $(LIB_DIR)/$(LIB_DIR).a)
# LIB_INC_FILES = $(wildcard $(LIB_DIR)/$(INC_DIR))

# CC = clang
# # CFLAGS = -std=c11 $(addprefix -W, all extra error pedantic) -g  # for mac and oracle
# CFLAGS = $(addprefix -W, all extra error pedantic) -g         # for linux and testing
# AR = ar
# AFLAGS = rcs
# MAKE = make -C

# MKDIR = mkdir -p
# RM = rm -rf

# all: $(NAMEBIN) clean

# $(NAMEBIN): $(OBJ_FILES)
# 	@$(MAKE) $(LIB_DIR)
# 	@$(CC) $(CFLAGS) $^ -L$(LIB_DIR) -lmx -lpthread -lcrypto -gdwarf-4 -rdynamic `pkg-config --cflags --libs libnotify gtk+-3.0` -o $@

# $(OBJ_FILES): | $(OBJ_DIR)

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
# 	@$(CC) $(CFLAGS)  `pkg-config --cflags gtk+-3.0 ` -c $< -o $@ -I $(INC_DIR) -I $(LIB_INC_FILES)

# $(OBJ_DIR):
# 	@$(MKDIR) $@

# clean:
# 	@$(RM) $(OBJ_DIR)
# 	@$(RM) $(LIB_FILES)

# uninstall:
# 	@$(RM) $(OBJ_DIR)
# 	@$(RM) $(NAMEBIN)

# reinstall: uninstall all

# .PHONY: all uninstall clean reinstall

CC = clang
# CFLAGS = -std=gnu11 -pthread -g 
# -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-unused-command-line-argument -Wno-unused-variable \
    -Wno-unused-function -Wno-unused-parameter

OBJDIR = obj
SRCDIR = src
UCHAT = ../uchat

SRC = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS = $(OBJDIR)/*.o

LIBMX_DIR = ../libraries/libmx
LIBMX_NAME = $(LIBMX_DIR)/libmx.a

GTK_FLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS_FLAGS = `pkg-config --cflags --libs gtk+-3.0`

SSL_DIR = ../libraries/openssl
SSL_FILES:= ../local_lib/lib
SSL_INC_FILES:= $(SSL_DIR)

LIB_FLAGS = -lmx -lsqlite3 -lssl -lcrypto\
			-L$(LIBMX_DIR) $(LIBMX_NAME)\
			-L$(SSL_FILES) -I$(SSL_INC_FILES)

all: $(UCHAT)

$(UCHAT):
	mkdir $(UTIL_DIRS) $(OBJDIR)
	$(CC) $(CFLAGS) $(GTK_FLAGS) -c $(SRC)
	mv *.o $(OBJDIR)
	$(CC) $(CFLAGS) $(GTK_LIBS_FLAGS) -o $(UCHAT) $(OBJS) $(LIB_FLAGS)


clean:
	rm -f $(OBJS)
	rm -rdf $(UTIL_DIRS) $(OBJDIR)

uninstall:
	make clean
	rm -f $(UCHAT)

reinstall:
	make uninstall
	make all



