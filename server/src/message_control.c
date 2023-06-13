#include "../inc/server.h"

bool mx_delete_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id) {
    if (mx_is_user_in_chat(db, chat_id, sender_id) 
        && (mx_get_sender_id(db, chat_id, message_id) == sender_id || mx_is_admin(db, chat_id, sender_id))) {
            mx_delete_message(db, chat_id, message_id);
            return true;
    }
    return false;
}

bool mx_edit_message_safe(sqlite3 *db, int chat_id, int message_id, int sender_id, char *text) {
    if (mx_is_user_in_chat(db, chat_id, sender_id) 
        && mx_get_sender_id(db, chat_id, message_id) == sender_id) {
            mx_edit_message(db, chat_id, message_id, text);
            return true;
    }
    return false;
}

int mx_get_sender_id(sqlite3 *db, int chat_id, int message_id) {
    sqlite3_stmt *stmt;
    const char *tail;
    int sender_id;

    int rc = sqlite3_prepare_v2(db, "SELECT Sender_id FROM Messages WHERE Chat_id = ? AND Local_id = ?;", -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, message_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        sender_id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return sender_id;
}

int mx_get_last_local_id(sqlite3* db, int chat_id, int user_id, const char* message_text) {
    sqlite3_stmt* stmt;
    int last_local_id = -1;

    const char* sql = "SELECT Local_id FROM Messages WHERE Chat_id = ? AND Sender_id = ? AND Message_text = ? ORDER BY Message_id DESC LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, chat_id);
        sqlite3_bind_int(stmt, 2, user_id);
        sqlite3_bind_text(stmt, 3, message_text, -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            last_local_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return last_local_id;
}

bool mx_delete_message(sqlite3 *db, int chat_id, int message_id) {
    sqlite3_stmt *stmt;
    const char *tail;
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    int rc = sqlite3_prepare_v2(db, "UPDATE Messages SET Message_text = '*message deleted*' WHERE Local_id = ? AND Chat_id = ?;", -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_int(stmt, 1, message_id);
    sqlite3_bind_int(stmt, 2, chat_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    return true;
}

bool mx_edit_message(sqlite3 *db, int chat_id, int message_id, char *text) {
    sqlite3_stmt *stmt;
    const char *tail;

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    int rc = sqlite3_prepare_v2(db, "UPDATE Messages SET Message_text = ? WHERE Local_id = ? AND Chat_id = ?;", -1, &stmt, &tail);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    sqlite3_bind_text(stmt, 1, text, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, message_id);
    sqlite3_bind_int(stmt, 3, chat_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    return 1;
}

int mx_create_message(sqlite3 *db, Thread_data *data, char *text) {
    sqlite3_stmt *stmt;
    const char *tail;
   
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    int rc = sqlite3_prepare_v2(db, "INSERT INTO Messages (Chat_id, Sender_id, Message_text, Message_time) VALUES (?, ?, ?, datetime('now'));", -1, &stmt, &tail);
    
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    sqlite3_bind_int(stmt, 1, (*data).current_chat);
    sqlite3_bind_int(stmt, 2, (*data).id);
    sqlite3_bind_text(stmt, 3, text, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    return mx_get_last_local_id(db, (*data).current_chat, (*data).id, text);
}

Message *mx_get_message(sqlite3 *db, int chat_id, int message_id) {
    Message *message = (Message *)malloc(sizeof(Message));
    const char* tail;
    sqlite3_stmt *stmt;

    message->chat_id = chat_id;
    message->message_id = message_id;

    sqlite3_prepare_v2(db, "SELECT Sender_id, Message_text, Message_time FROM Messages WHERE Local_id = ? AND Chat_id = ?;", -1, &stmt, &tail);
    sqlite3_bind_int(stmt, 1, message_id);
    sqlite3_bind_int(stmt, 2, chat_id);
    sqlite3_step(stmt);

    message->sender_id = sqlite3_column_int(stmt, 0);
    message->message_text = mx_strdup((const char*)sqlite3_column_text(stmt, 1));
    message->message_time = mx_strdup((const char*)sqlite3_column_text(stmt, 2));
    message->sender_name = mx_get_username_by_id(db, message->sender_id);
    sqlite3_finalize(stmt);

    return message;
}

void mx_clear_message(Message *message) {
    free(message->message_text);
    free(message->message_time);
    free(message->sender_name);
    free(message);
    message = NULL;
}

int mx_get_message_ids_by_chat_id(sqlite3* db, int chat_id, int** message_ids) {
    sqlite3_stmt* stmt;
    const char* tail;
    int result;
    result = sqlite3_prepare_v2(db, "SELECT Local_id FROM Messages WHERE Chat_id = ?;", -1, &stmt, &tail);
    if (result != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, chat_id);

    int id_count = 0;
    int *ids = NULL;

    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int *tmp_ids = realloc(ids, (id_count + 1) * sizeof(int));
        if (tmp_ids == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            free(ids);
            sqlite3_finalize(stmt);
            return -1;
        }
        ids = tmp_ids;
        ids[id_count] = id;
        id_count++;
    }
    
    if (result != SQLITE_DONE) {
        fprintf(stderr, "Ошибка выполнения запроса: %s\n", sqlite3_errmsg(db));
        free(ids);
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    *message_ids = ids;
    return id_count;
}

