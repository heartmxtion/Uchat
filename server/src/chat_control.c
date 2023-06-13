#include "../inc/server.h"

bool mx_change_chat_name(sqlite3 *db, int chat_id, const char *chat_name) {
    sqlite3_stmt *stmt;
    const char *tail;

    int rc = sqlite3_prepare_v2(db, "UPDATE Chats SET Chat_name = ? WHERE Chat_id = ?;", -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, chat_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, chat_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool mx_is_admin(sqlite3 *db, int chat_id, int user_id) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT 1 FROM Chats WHERE Chat_id = ? AND Admin_id = ?;";

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) { 
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool mx_is_user_in_chat(sqlite3 *db, int chat_id, int user_id) {
    sqlite3_stmt* stmt;
    const char* query = "SELECT COUNT(*) FROM Chats_users WHERE Chat_id = ? AND User_id = ?;";
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count > 0;
}

void mx_increase_user_number(sqlite3 *db, int chat_id) {
    sqlite3_stmt *stmt;
    const char *update_chat_query = "UPDATE Chats SET Users_number = Users_number + 1 WHERE Chat_id = ?;";

    sqlite3_prepare_v2(db, update_chat_query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void mx_decrease_user_number(sqlite3 *db, int chat_id) {
    sqlite3_stmt *stmt;
    const char *update_chat_query = "UPDATE Chats SET Users_number = Users_number - 1 WHERE Chat_id = ?;";

    sqlite3_prepare_v2(db, update_chat_query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}


void mx_del_users_from_chat(sqlite3 *db, char *data, int current_chat) {
    char **commands = mx_strsplit(data, '|');
    for (int i = 2; commands[i]; i++) {
        mx_del_user_from_chat(db, (mx_atoi(commands[1]) ? mx_strcmp(commands[1], "-") != 0: current_chat), mx_atoi(commands[i]));
    }
    for (int i = 0; commands[i]; i++) free(commands[i]);
    free(commands);
}


void mx_add_users_in_chat(sqlite3 *db, char *data, int current_chat) {
    char **commands = mx_strsplit(data, '|');
    for (int i = 2; commands[i]; i++) {
        mx_add_user_in_chat(db, (mx_atoi(commands[1]) ? mx_strcmp(commands[1], "-") != 0: current_chat), mx_atoi(commands[i]));
    }
    for (int i = 0; commands[i]; i++) free(commands[i]);
    free(commands);
}

int mx_del_user_from_chat(sqlite3 *db, int chat_id, int user_id) {
    sqlite3_stmt *stmt;
    const char *tail;

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    // int rc = sqlite3_prepare_v2(db, "INSERT INTO Chats_users (Chat_id, User_id) VALUES (?, ?);", -1, &stmt, &tail);
    int rc = sqlite3_prepare_v2(db, "DELETE FROM Chats_users WHERE Chat_id = ? AND User_id = ?;", -1, &stmt, &tail);


    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    mx_increase_user_number(db, chat_id);
    return true;
}

int mx_add_user_in_chat(sqlite3 *db, int chat_id, int user_id) {
    sqlite3_stmt *stmt;
    const char *tail;

    int count = 0;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM Chats_users WHERE Chat_id = ? AND User_id = ?;", -1, &stmt, &tail);
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (count > 0) {
        printf("Пользователь с User_id = %d уже состоит в данном чате с Chat_id = %d\n", user_id, chat_id);
        return false;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    rc = sqlite3_prepare_v2(db, "INSERT INTO Chats_users (Chat_id, User_id) VALUES (?, ?);", -1, &stmt, &tail);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    mx_increase_user_number(db, chat_id);
    return true;
}

int mx_create_chat(sqlite3 *db, const char *chat_name, int admin_id) {
    int rc;
    sqlite3_stmt *stmt;
    const char *tail;
    int chat_id;

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    rc = sqlite3_prepare_v2(db, "INSERT INTO Chats (Chat_name, Admin_id, Users_number) VALUES (?, ?, 0);", -1, &stmt, &tail);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    sqlite3_bind_text(stmt, 1, chat_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, admin_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        return -1;
    }
    chat_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    mx_add_user_in_chat(db, chat_id, admin_id);
    return chat_id;
}
