#include "../inc/server.h"

sqlite3 *mx_open_db() {
    sqlite3 *db;
    sqlite3_open("resources/uchat.db", &db);
    return db;
}

void mx_close_db(sqlite3 *db) {
    sqlite3_close(db);
}

int mx_auth(int socket, KeyPair server_keys) {
    sqlite3 *db;
    while(1) {
        char buffer[1024] = {0};
        int res;
        int status = read(socket, buffer, 1024);
        if (status == 0) return -1;
        char *decrypted_data = mx_decrypting_data(buffer, server_keys.private_key);
        char **data = mx_strsplit(decrypted_data, '|');
        free(decrypted_data);

        db = mx_open_db();
        if (mx_strcmp(data[0], "l") == 0) {
            res = mx_sign_in(db, data[1], data[2]);
            send(socket, mx_itoa(res), 1, 0);
            if (res == SUCCESS) return mx_get_id(db, data[1], data[2]);
        }

        if (mx_strcmp(data[0], "r") == 0) {
            res = mx_sign_up(db, data[1], data[2]);
            mx_add_user_in_chat(db, 1, mx_get_id(db, data[1], data[2]));
            send(socket, mx_itoa(res), 1, 0);
            if (res == SUCCESS) return mx_get_id(db, data[1], data[2]);
        }
        mx_close_db(db);
    }
}

int mx_get_count_of_chats(sqlite3 *db, int user_id) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(DISTINCT Chat_id) FROM Chats_users WHERE User_id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    rc = sqlite3_bind_int(stmt, 1, user_id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        printf("chat count: %i\n", count);
        sqlite3_finalize(stmt);
        return count;
    } else return -1;
}

void mx_send_user_info_by_username(sqlite3 *db, const char *username_fragment, Thread_data *data, char *dest) {
    sqlite3_stmt *stmt;
    const char *query = "SELECT * FROM Users WHERE Username LIKE ?;";
    int rc;
    // char buffer[BUFFER_SIZE] = {0};

    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    char pattern[256];
    snprintf(pattern, sizeof(pattern), "%%%s%%", username_fragment);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
    int resultsFound = 0;

    // printf("Информация о пользователях, чье поле Username содержит фрагмент текста '%s':\n", username_fragment);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        resultsFound = 1;
        int user_id = sqlite3_column_int(stmt, 0);
        const unsigned char *username = sqlite3_column_text(stmt, 1);
        // const unsigned char *global_nickname = sqlite3_column_text(stmt, 4);
        char *us_id = mx_itoa(user_id);
        char *mess = mx_strjoin("ui|", dest);
        mess = mx_strjoin(mess, "|");
        mess = mx_strjoin(mess, us_id);
        mess = mx_strjoin(mess, "|");
        mess = mx_strjoin(mess, (char *)username);
        char *encrypted_mess = mx_encrypting_data(mess, (*data).client_public_key);
        // mess = mx_strjoin(mess, "|");
        // mess = mx_strjoin(mess, global_nickname);
        send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
        free(encrypted_mess);
        free(mess);
        if (!mx_wait_answer(db, data)) return;
        // if (read((*data).socket, buffer, 1024) == 0) return;
    }
    sqlite3_finalize(stmt);
    if (!resultsFound) {
        char *mess = mx_strjoin("ui|", dest);
        mess = mx_strjoin(mess, "|-");
        char *encrypted_mess = mx_encrypting_data(mess, (*data).client_public_key);
        send((*data).socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
        free(encrypted_mess);
        free(mess);
        if (!mx_wait_answer(db, data)) return;
        // if (read((*data).socket, buffer, 1024) == 0) return;
    };
    // if (!mx_wait_answer(db, data)) return;
}

void mx_handle_request(int socket, Thread_data *data) {
    char *key = mx_key_request_check(socket);
    if (key == NULL) return;
    (*data).client_public_key = handle_key_exchange(socket, *((*data).server_key_pair), key);
    Thread_data dta = *data;
    
    int id = mx_auth(socket, *(dta.server_key_pair));
    if (id < 0) return;

    sqlite3 *db = mx_open_db();

    char *name = mx_get_username_by_id(db, id);
    // int current_chat = mx_get_current_chat(db, id);
    // (*data).current_chat = current_chat;
    (*data).current_chat = mx_get_current_chat(db, id);
    (*data).id = id;
    printf("%s connected\n", name);
    mx_close_db(db);

    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        printf("wait for new message\n");
        int n = read(socket, buffer, BUFFER_SIZE);
        printf("buffer--: %s\n", buffer);
        if (n == 1) continue;
        if (n == 0) {
            printf("%s disconnected\n", name);
            return;
        }
        char *decrypted_mess = mx_decrypting_data(buffer, dta.server_key_pair->private_key);
        printf("%s: %s\n", name, decrypted_mess);

        sqlite3 *db = mx_open_db();
        // mx_foo(db, id, decrypted_mess, &((*data).current_chat), dta, lisst);
        mx_foo(db, decrypted_mess, data);
        free(decrypted_mess);
        mx_close_db(db);
    }
}

static void daemonize() {
    pid_t pid, sid;
    
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    if ((sid = setsid()) < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }
    signal(SIGTERM, SIG_DFL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./uchat_server port\n");
        exit(0);
    }
    daemonize();

    KeyPair server_key_pair = mx_generate_key_pair();

    syslog(LOG_INFO, "Hello, world!");

    char ip[10] = "127.0.0.1";
    int server_fd = mx_create_socket();

    mx_socket_bind(server_fd, ip, mx_atoi(argv[1]));
    mx_socket_listen(server_fd, server_key_pair);

    RSA_free(server_key_pair.public_key);
    RSA_free(server_key_pair.private_key);

    exit(EXIT_SUCCESS);
}

