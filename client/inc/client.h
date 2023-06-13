#ifndef CLIENT_H
#define CLIENT_H

#include "../libmx/inc/libmx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include "../../libraries/openssl/openssl/pem.h"
#include "../../libraries/openssl/openssl/rsa.h"
#include "../../libraries/openssl/openssl/sha.h"
// #include <openssl/pem.h>
// #include <openssl/rsa.h>
// #include <openssl/sha.h>
#include <libnotify/notify.h>

#define SUCCESS  0
#define ALREADY  1
#define ERROR    2

#define BUFFER_SIZE 1024
#define PORT 8080

// Структура для хранения публичного и приватного ключей
typedef struct keypair_t{
    RSA *public_key;
    RSA *private_key;
} KeyPair;

typedef struct connection_t {
    int socket;
    struct sockaddr_in serv_addr;
}              Connection;

typedef struct dataforchat_t {
    GtkBuilder* builder;
    GtkWidget* this_window;
    Connection conn;
    KeyPair client_pair;
    RSA* server_key;
    int row_id;
    int current_chat;
    int current_reconn_try;
    guint handler_id;
    pthread_t thread_chat_listening;
    bool is_reconnecting;
    int *new_users;
    int *ban_users;
    int del_menu;
}              DataForChat;

typedef struct {
    DataForChat *data;
    char *decrypted_mess;
} Data_for_idle;

//socket control
Connection mx_init_socket(int port, char *ip);
void mx_disconnect(Connection conn);

//messages conrtol
void mx_add_last_message(DataForChat *data, char *decrypted_mess);
void mx_create_message(DataForChat *data, char *message_data);
void mx_edit_message(DataForChat *data, char *message_data);
void mx_read_messages(DataForChat *data);
void mx_get_old_messages(DataForChat *data, int chat);
void show_notification();

//auth screen control
void on_continue_clicked(GtkButton *b, gpointer data_chat);
int mx_auth(DataForChat *data, char *request);

//chat control
void on_message_row_selected(GtkWidget *w, GdkEventButton *event, gpointer data_chat);
void on_scroll_changed(GtkWidget *w, gpointer data_chat);
int mx_strlen_until_char(const char *str, char delim);
char *mx_time_to_local(char *time_m);
int create_chat(void* data_chat);

//chat create control
void on_cancel_clicked(GtkButton *b, gpointer data_chat);
void on_create_chat_clicked(GtkButton *b, gpointer data_chat);
void on_add_chat_clicked(GtkButton *b, gpointer data_chat);
void on_chat_create_user_search_changed(GtkSearchEntry *entry, gpointer user_data);
void mx_show_users(DataForChat *data, char *decrypted_mess);

//chat work
void mx_get_chats(DataForChat *data);
int mx_get_current_chat(DataForChat *data);
void on_chat_row_selected(GtkListBox *listbox, GtkListBoxRow *row, gpointer data_chat);
void mx_create_chat_w(DataForChat *data, char *chat_data);
void mx_edit_chat_name(char *chat_name, DataForChat *data);
void mx_edit_chat_w(DataForChat *data, char *chat_data);


//clicked work
void on_chat_send_clicked(GtkButton *b, gpointer data_chat);
void on_back_settings_clicked(GtkButton *b, gpointer data_chat);
void on_apply_settings_clicked(GtkButton *b, gpointer data_chat);
void on_settings_clicked(GtkButton *b, gpointer data_chat);
void on_emoji_clicked(GtkButton *button, DataForChat* data_chat);
void on_emoji_send_clicked(GtkButton *button, DataForChat *data_chat);
void on_arrow_clicked(GtkButton *arrow, DataForChat *data_chat);
void mx_del_chat_w(DataForChat *data, char *chat_data);

//client
RSA *mx_key_exchange(RSA* client_public_key, int socket);

//menu search
void on_search_changed(GtkSearchEntry *entry, gpointer user_data);
void on_search_chats_changed(GtkSearchEntry *entry, gpointer chat_list);
void on_menuitem_tag_activate(GtkMenuItem *menuitem, gpointer data_chat);
void on_menuitem_edit_activate(GtkMenuItem *menuitem, gpointer data_chat);
void on_menuitem_del_activate(GtkMenuItem *menuitem, gpointer data_chat);

// chat settings control
void on_user_row_selected(GtkWidget *w, GdkEventButton *event, gpointer data_chat);
void on_back_settings_clicked(GtkButton *b, gpointer data_chat);
void on_apply_settings_clicked(GtkButton *b, gpointer data_chat);
void on_settings_clicked(GtkButton *b, gpointer data_chat);
void mx_add_user_in_list(DataForChat *data, char *decrypted_mess);
void on_search_member_name_changed(GtkSearchEntry *entry, gpointer user_data);
void del_user_butt_activate(GtkMenuItem *menuitem, gpointer data_chat);

//secondary func
void on_window_closed(GtkWidget* widget, gpointer data);
char *mx_log_and_pass_to_string(char *command, const char *login, const char *password);
char* mx_encrypting_data(const char *data, RSA* server_key);
char *mx_decrypting_data(char *buffer, RSA* private_key);
char *mx_ljoin(char *s1, const char *s2);
char *mx_rjoin(const char *s1, char *s2);
void clear_widget(GtkWidget *widget);
GtkWidget* mx_find_widget_by_name(GtkWidget *box, const gchar *widget_name);
GtkWidget* mx_find_label_in_box(GtkWidget *box, const gchar *label_id);
bool is_user_in_list(GtkWidget *list, char *text);
int *mx_add_in_list(int *list, int user_id);
bool remove_item_from_list(int* list, int num);

#endif
