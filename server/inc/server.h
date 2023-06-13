#ifndef SERVER_H
#define SERVER_H

#include "../libmx/inc/libmx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../../libraries/openssl/openssl/pem.h"
#include "../../libraries/openssl/openssl/rsa.h"
#include <time.h>
#include <signal.h>
#include <syslog.h>

#include <sqlite3.h>

#define SUCCESS  0
#define ALREADY  1
#define ERROR    2
#define BUFFER_SIZE 1024

#define MAX_THREAD 5
#define PORT 8080

typedef struct thread_data_t Thread_data;

// Структура для хранения публичного и приватного ключей
typedef struct keypair_t {
    RSA *public_key;
    RSA *private_key;
}              KeyPair;

typedef struct message_t {
    int message_id;
    int chat_id;
    int sender_id;
    char *sender_name;
    char *message_text;
    char *message_time;
}              Message;

typedef struct thread_data_t {
    int id;
    int socket;
    int current_chat;
    bool thread_work;
    RSA *client_public_key;
    KeyPair *server_key_pair;
    Thread_data **thread_data;
}              Thread_data;

//user table
int mx_get_current_chat(sqlite3 *db, int user_id);
int mx_get_id(sqlite3 *db, char *username, char *password);
char *mx_get_username_by_id(sqlite3 *db, int user_id);
void mx_add_user_to_users_tables(sqlite3 *db, char *username, char *password);
void mx_delete_user_by_id(sqlite3 *db, int user_id);
bool mx_user_exists(sqlite3* db, const char* username);
void renumber_users_table(sqlite3 *db); // возможно не нужно
int mx_sign_in(sqlite3 *db, char *username, char *password);
int mx_sign_up(sqlite3 *db, char *username, char *password);

typedef struct chat_info_l {
    int chat_id;
    int users_number;
    int admin_id;
    char *chat_name;
}              Chat_info;

//server
int mx_get_count_of_chats(sqlite3 *db, int user_id);
// void mx_send_user_info_by_username(sqlite3 *db, const char *username_fragment, Thread_data data, char *dest);
// void mx_handle_request(int socket, Thread_data *data, Thread_data **lisst);
void mx_send_user_info_by_username(sqlite3 *db, const char *username_fragment, Thread_data *data, char *dest);
void mx_handle_request(int socket, Thread_data *data);
void mx_socket_bind(int server_fd, char *ip, int port);

//socket
int mx_create_socket();
void mx_socket_listen(int server_fd, KeyPair server_key_pair);

//key work
char* mx_key_request_check(int socket);
RSA* handle_key_exchange(int socket, KeyPair server_key_pair, char *command);
KeyPair mx_generate_key_pair();

//chat control
bool mx_is_admin(sqlite3 *db, int chat_id, int user_id);
bool mx_is_user_in_chat(sqlite3 *db, int chat_id, int user_id);
void mx_increase_user_number(sqlite3 *db, int chat_id);
void mx_decrease_user_number(sqlite3 *db, int chat_id);
int mx_add_user_in_chat(sqlite3 *db, int chat_id, int user_id);
int mx_create_chat(sqlite3 *db, const char *chat_name, int admin_id);
void mx_add_users_in_chat(sqlite3 *db, char *data, int current_chat);
bool mx_change_chat_name(sqlite3 *db, int chat_id, const char* chat_name);
void mx_del_users_from_chat(sqlite3 *db, char *data, int current_chat);
int mx_del_user_from_chat(sqlite3 *db, int chat_id, int user_id);

//chat info
void mx_clear_chat_info(Chat_info *info);
char *mx_chat_info_to_str(char *command, Chat_info *info);
void mx_get_chats(sqlite3 *db, Thread_data *data);
Chat_info *mx_get_chat_info(sqlite3 *db, int chat_id);

//messages control
// void mx_delete_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id);
// void mx_edit_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id, char *text);
int mx_get_sender_id(sqlite3 *db, int chat_id, int message_id);
Message *mx_get_message(sqlite3 *db, int chat_id, int message_id);
void mx_clear_message(Message *message);
int mx_get_message_ids_by_chat_id(sqlite3* db, int chat_id, int** message_ids);
// bool mx_create_message(sqlite3 *db, int chat_id, int sender_id, char *text);
bool mx_edit_message(sqlite3 *db, int chat_id, int message_id, char *text);
bool mx_delete_message(sqlite3 *db, int chat_id, int message_id);
int get_last_local_id(sqlite3* db, int chat_id, int user_id, const char* message_text);
bool mx_delete_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id);
bool mx_edit_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id, char *text);
int mx_create_message(sqlite3 *db, Thread_data *data, char *text);

//message work
// void mx_send_all_last_messages(sqlite3 *db, int user_id, Thread_data *data);
void mx_send_all_last_messages(sqlite3 *db,Thread_data *data);
void mx_send_message(sqlite3 *db, int local_id, char *command, Thread_data *data);
void mx_get_messages(sqlite3 *db, Thread_data *data);
Message *mx_get_last_message(sqlite3 *db, int chat_id);

//ecryption data
char *mx_decrypting_data(char *buffer, RSA* private_key);
char* mx_encrypting_data(const char *data, RSA* client_key);

//foo work
void* foo(void* p);
// void mx_foo(sqlite3 *db, int id, char *data, int *current_chat, Thread_data thr_data, Thread_data **lisst);
void mx_foo(sqlite3 *db, char *data, Thread_data *thr_data);
bool mx_wait_answer(sqlite3 *db, Thread_data *data);

//secondary func
char *mx_ljoin(char *s1, char const *s2);

// Макрос для определения опции SO_REUSEADDR или SO_REUSEPORT в зависимости от платформы
#ifdef __APPLE__
#define REUSE_OPT SO_REUSEADDR
#else
#define REUSE_OPT SO_REUSEPORT
#endif

#endif
