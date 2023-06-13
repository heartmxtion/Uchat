#include "../inc/server.h"

int mx_get_current_chat(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT Current_chat FROM Users WHERE User_id = ?;";
    int current_chat = -1;

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        current_chat = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return current_chat;
}

int mx_get_id(sqlite3 *db, char *username, char *password) {
    sqlite3_stmt* stmt;
    char *sql = "SELECT User_id FROM Users WHERE Username = ? AND Password = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int userId = 0;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return userId;
}

char *mx_get_username_by_id(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    char *sql = "SELECT Username FROM Users WHERE User_id = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_step(stmt);
    char *name = mx_strdup((const char*)sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);
    return name;
}

void mx_add_user_to_users_tables(sqlite3 *db, char *username, char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Users (Username, Password, Current_chat) VALUES (?, ?, 1)";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void mx_delete_user_by_id(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    char *sql = "DELETE FROM Users WHERE User_id = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

bool mx_user_exists(sqlite3* db, const char* username) {
    sqlite3_stmt* statement;
    const char* sql_query = "SELECT * FROM Users WHERE Username = ?";
    
    sqlite3_prepare_v2(db, sql_query, -1, &statement, NULL);
    sqlite3_bind_text(statement, 1, username, strlen(username), SQLITE_STATIC);
    
    int result = sqlite3_step(statement);
    bool exists = (result == SQLITE_ROW);
    
    sqlite3_finalize(statement);
    return exists;
}

void renumber_users_table(sqlite3 *db) { // возможно не нужно
    char *errmsg;
    sqlite3_stmt *stmt;
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
    sqlite3_prepare_v2(db, "SELECT User_id, Username FROM Users", -1, &stmt, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);

    int new_id = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *username = (const char*) sqlite3_column_text(stmt, 1);
        sqlite3_stmt *update_stmt;
        sqlite3_prepare_v2(db, "UPDATE Users SET User_id = ? WHERE Username = ?", -1, &update_stmt, NULL);
        sqlite3_bind_int(update_stmt, 1, new_id);
        sqlite3_bind_text(update_stmt, 2, username, -1, SQLITE_STATIC);
        sqlite3_step(update_stmt);
        sqlite3_finalize(update_stmt);
        new_id++;
    }
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT", NULL, NULL, &errmsg);
}

int mx_sign_up(sqlite3 *db, char *username, char *password) {
    if (!mx_user_exists(db, username)) {
        mx_add_user_to_users_tables(db, username, password);
        return SUCCESS;
    }
    else return ALREADY;
}

int mx_sign_in(sqlite3 *db, char *username, char *password) {
    int s = mx_get_id(db, username, password);
    if (s) return SUCCESS;
    else return ERROR;
}

