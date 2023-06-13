
# # NAME = uchat_server.a
# # NAMEBIN = uchat_server

# # SRC_DIR = src
# # OBJ_DIR = obj
# # INC_DIR = inc
# # LIB_DIR = libmx

# # SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# # OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
# # INC_FILES = $(wildcard $(INC_DIR)/*.h)
# # LIB_FILES = $(wildcard $(LIB_DIR)/$(LIB_DIR).a)
# # LIB_INC_FILES = $(wildcard $(LIB_DIR)/$(INC_DIR))

# # CC = clang
# # # CFLAGS = -std=c11 $(addprefix -W, all extra error pedantic) -g  # for mac and oracle
# # CFLAGS =  $(addprefix -W, all extra error pedantic) -g        # for linux and testing
# # AR = ar
# # AFLAGS = rcs
# # MAKE = make -C

# # MKDIR = mkdir -p
# # RM = rm -rf

# # all: $(NAMEBIN) clean

# # $(NAMEBIN): $(OBJ_FILES)
# # 	@$(MAKE) $(LIB_DIR)
# # 	@$(CC) $(CFLAGS) -lpthread $^ -L$(LIB_DIR) -lmx -lsqlite3 -lcrypto -o $@

# # $(OBJ_FILES): | $(OBJ_DIR)

# # $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
# # 	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(LIB_INC_FILES)

# # $(OBJ_DIR):
# # 	@$(MKDIR) $@

# # clean:
# # 	@$(RM) $(OBJ_DIR)
# # 	@$(RM) $(LIB_FILES)

# # uninstall:
# # 	@$(RM) $(OBJ_DIR)
# # 	@$(RM) $(NAMEBIN)

# # reinstall: uninstall all

# # .PHONY: all uninstall clean reinstall

# NAME_SERV	= uchat_server
# NAME_CLIENT = uchat

# CFLAGS 	= -std=c11 -Wall -Wextra -Werror -Wpedantic -g
# COMP 	= clang

# SERV_DIR 	= server/
# CLIENT_DIR 	= client/

# all: 
# 	@brew install gtk+3
# 	@brew install openssl@1.1 
# 	@brew install json-c 
# 	@brew install sqlite3 
# 	@make -sC $(SERV_DIR)
# 	@make -sC $(CLIENT_DIR)

# userver: 
# 	@make -sC $(SERV_DIR)

# uclient: 
# 	@make -sC $(CLIENT_DIR)
	
# clean:
# 	@make clean -sC $(SERV_DIR)
# 	@make clean -sC $(CLIENT_DIR)
	

# uninstall: clean
# 	@make uninstall -sC $(SERV_DIR)
# 	@make uninstall -sC $(CLIENT_DIR)
# 	rm -rf $(NAME_CLIENT)
# 	rm -rf $(NAME_SERV)
# 	rm -rf *.txt

# reinstall: uninstall all
CC = clang
# CFLAGS = -std=gnu11 -pthread -g 
# -Wall -Wextra -Werror -Wpedantic
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -pthread -g

OBJDIR = obj
SRCDIR = src
UCHAT = ../uchat_server

SRC = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS = $(OBJDIR)/*.o

LIBMX_DIR = ../libraries/libmx
LIBMX_NAME = $(LIBMX_DIR)/libmx.a

SSL_DIR = ../libraries/openssl
SSL_FILES:= ../local_lib/lib
SSL_INC_FILES:= $(SSL_DIR)

LIB_FLAGS = -lmx -lssl -lsqlite3 -lcrypto \
			-L$(LIBMX_DIR) $(LIBMX_NAME) \
			-L$(SSL_FILES) -I$(SSL_INC_FILES)

all: $(UCHAT)

$(UCHAT):
	mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $(SRC)
	mv *.o $(OBJDIR)
	$(CC) $(CFLAGS) -o $(UCHAT) $(OBJS) $(LIB_FLAGS)

clean:
	rm -f $(OBJS)
	rm -df $(OBJDIR)
	
uninstall:
	make clean
	rm -f $(UCHAT)

reinstall:
	make uninstall
	make all
