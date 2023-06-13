#include "../inc/server.h"

Message *mx_get_last_message(sqlite3 *db, int chat_id) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT Sender_id, Message_text, Message_time FROM Messages WHERE Chat_id = ? ORDER BY Message_time DESC LIMIT 1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, chat_id);

    Message *message = NULL;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        message = (Message *)malloc(sizeof(Message));
        message->sender_id = sqlite3_column_int(stmt, 0);
        message->message_text = mx_strdup((const char*)sqlite3_column_text(stmt, 1));
        message->message_time = mx_strdup((const char*)sqlite3_column_text(stmt, 2));
        message->sender_name = mx_get_username_by_id(db, message->sender_id);
    }
    sqlite3_finalize(stmt);
    return message;
}

char *mx_message_to_str(char *command, Message *message) {
    // command|chat_id|message_id|sender_id|sender_name|message_text|message_time
    char *str = mx_strjoin(command, "|");
    str = mx_strjoin(str, mx_itoa(message->chat_id));
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, mx_itoa(message->message_id));
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, mx_itoa(message->sender_id));
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, message->sender_name);
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, message->message_text);
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, message->message_time);
    return str;
}

int  mx_send_last_message(sqlite3 *db, int chat_id, Thread_data *data) {
    Message *mess = mx_get_last_message(db, chat_id);
    if (mess == NULL) {
        printf("NULL\n");
        return 0;
    }
    mess->chat_id = chat_id;
    mess->message_id = 0;
    char *mess_str = mx_message_to_str("lm", mess);
    printf("mess: %s\n", mess_str);
    char *enc_mess = mx_encrypting_data(mess_str, (*data).client_public_key);
    printf("mess enc: %s\n", enc_mess);

    send((*data).socket, enc_mess, mx_strlen(enc_mess), 0);

    free(enc_mess);
    free(mess_str);
    mx_clear_message(mess);
    return 1;
}

void mx_send_all_last_messages(sqlite3 *db,Thread_data *data) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT Chat_id FROM Chats_users WHERE User_id = ?;";
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    int size = mx_get_count_of_chats(db, (*data).id);
    int *list = (int *)malloc(sizeof(int) * size);
    int i = 0;
    // char buffer[BUFFER_SIZE] = {0};
    printf("size: %i\n", size);

    sqlite3_bind_int(stmt, 1, (*data).id);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        list[i++] = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    for (int j = 0; j < size; j++) {
        printf("size: %i, %i\n", j, list[j]);
        if (mx_send_last_message(db, list[j], data))
            if (!mx_wait_answer(db, data)) return;
        // if ((int size_mess = read(data.socket, buffer, BUFFER_SIZE)) == 0) return;
    }
    free(list);
}

void mx_get_messages(sqlite3 *db, Thread_data *data) {
    int *message_ids = NULL;
    // char buffer[BUFFER_SIZE] = {0};
    // mx_memset(buffer, '\0', BUFFER_SIZE);
    int message_count = mx_get_message_ids_by_chat_id(db, (*data).current_chat, &message_ids);

    for (int i = 0; i < message_count; i++) {
        if (!mx_wait_answer(db, data)) return;
        // if (read((*data).socket, buffer, BUFFER_SIZE) == 0) return;
        Message *message = mx_get_message(db, (*data).current_chat, message_ids[i]);
        char *message_string = mx_message_to_str("ms", message);
        char *encrypted_mess = mx_encrypting_data(message_string, (*data).client_public_key);
        
        send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
        free(encrypted_mess);
        free(message_string);
        mx_clear_message(message);
    }
    if (!mx_wait_answer(db, data)) return;
    char *encrypted_mess = mx_encrypting_data("end", (*data).client_public_key);
    send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    free(encrypted_mess);
    if (!mx_wait_answer(db, data)) return;
}

void mx_send_message(sqlite3 *db, int local_id, char *command, Thread_data *data) {
    for (int i = 0; i < MAX_THREAD; i++) {
        if ((*(*data).thread_data)[i].thread_work) {
            if (mx_is_user_in_chat(db , (*data).current_chat, (*(*data).thread_data)[i].id)) {
                printf("user id:%i\n", (*(*data).thread_data)[i].id);
                Message *message = mx_get_message(db, (*data).current_chat, local_id);
                char *message_string = mx_message_to_str(command, message);
                char *encrypted_mess = mx_encrypting_data(message_string, (*(*data).thread_data)[i].client_public_key);

                send((*(*data).thread_data)[i].socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
                free(message_string);
                free(encrypted_mess);
                mx_clear_message(message);
                printf("message was sended!\n");
            }
        }
    }
}



