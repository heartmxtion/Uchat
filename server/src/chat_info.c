#include "../inc/server.h"

void mx_clear_chat_info(Chat_info *info) {
    free(info->chat_name);
    free(info);
    info = NULL;
}

char *mx_chat_info_to_str(char *command, Chat_info *info) {
    char *str = mx_strjoin(command, "|");
    str = mx_strjoin(str, mx_itoa(info->chat_id));
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, mx_itoa(info->users_number));
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, info->chat_name);
    str = mx_strjoin(str, "|");
    str = mx_strjoin(str, mx_itoa(info->admin_id));
    return str;
}

Chat_info *mx_get_chat_info(sqlite3 *db, int chat_id) {
    sqlite3_stmt *stmt;
    Chat_info *chat_info = NULL;
    const char *query = "SELECT Chat_id, Users_number, Admin_id, Chat_name FROM Chats WHERE Chat_id = ?;"; // SQL-запрос
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, chat_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        chat_info = (Chat_info *)malloc(sizeof(Chat_info));
        chat_info->chat_id = sqlite3_column_int(stmt, 0);
        chat_info->users_number = sqlite3_column_int(stmt, 1);
        chat_info->admin_id = sqlite3_column_int(stmt, 2);
        chat_info->chat_name = mx_strdup((const char *)sqlite3_column_text(stmt, 3));
    }
    sqlite3_finalize(stmt);

    return chat_info;
}

void mx_get_chats(sqlite3 *db, Thread_data *data) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT Chat_id FROM Chats_users WHERE User_id = ?;";
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, (*data).id);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Chat_info *info = mx_get_chat_info(db, sqlite3_column_int(stmt, 0));
        char *ch_inf = mx_chat_info_to_str("cc", info);
        printf("%s\n", ch_inf);
        char *encrypted_mess = mx_encrypting_data(ch_inf, (*data).client_public_key);
        send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
        usleep(100);
        free(encrypted_mess);
        free(ch_inf);
        mx_clear_chat_info(info);
        if (!mx_wait_answer(db, data)) return;
        // if (read((*data).socket, buffer, BUFFER_SIZE) == 0) return;
    }
    char *encrypted_mess = mx_encrypting_data("end", (*data).client_public_key);
    send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    free(encrypted_mess);
    sqlite3_finalize(stmt);
}

