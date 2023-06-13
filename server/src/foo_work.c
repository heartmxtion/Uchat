#include "../inc/server.h"

void mx_send_chats_inf(sqlite3 *db, char *command, Thread_data *data) {
    for (int i = 0; i < MAX_THREAD; i++) {
        if ((*(*data).thread_data)[i].thread_work) {
            if (mx_is_user_in_chat(db , (*data).current_chat, (*(*data).thread_data)[i].id)) {
                Chat_info *ch_inf = mx_get_chat_info(db, (*data).current_chat);
                char *ch_inf_c = mx_chat_info_to_str(command, ch_inf);
                char *encrypted_mess = mx_encrypting_data(ch_inf_c, (*(*data).thread_data)[i].client_public_key);

                send((*(*data).thread_data)[i].socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
                free(ch_inf_c);
                free(encrypted_mess);
                mx_clear_chat_info(ch_inf);
            }
        }
    }
}

void mx_send_chats_inf_s(sqlite3 *db, char *command, Thread_data *data, char *mess_data) {
    char **temp = mx_strsplit(mess_data, '|');
    for (int i = 0; i < MAX_THREAD; i++) {
        if ((*(*data).thread_data)[i].thread_work) {
            for (int j = 1; temp[j]; j++) {
                if ((*(*data).thread_data)[i].id == mx_atoi(temp[j])) {
                    Chat_info *ch_inf = mx_get_chat_info(db, (*data).current_chat);
                    char *ch_inf_c = mx_chat_info_to_str(command, ch_inf);
                    char *encrypted_mess = mx_encrypting_data(ch_inf_c, (*(*data).thread_data)[i].client_public_key);

                    send((*(*data).thread_data)[i].socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
                    free(ch_inf_c);
                    free(encrypted_mess);
                    mx_clear_chat_info(ch_inf);
                }
            }
        }
    }
}


void mx_get_users_in_chat(sqlite3 *db, int chat_id, Thread_data *thr_data) {
    sqlite3_stmt *stmt;
    const char *tail;

    int rc = sqlite3_prepare_v2(db, "SELECT User_id FROM Chats_users WHERE Chat_id = ?;", -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, chat_id);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int user_id = sqlite3_column_int(stmt, 0);
        char *user_id_s = mx_itoa(user_id);
        char *username = mx_get_username_by_id(db, user_id);
        char *req = mx_strjoin("uic|", user_id_s);
        req = mx_ljoin(req, "|");
        req = mx_ljoin(req, username);
        char *encrypt_mess = mx_encrypting_data(req, thr_data->client_public_key);

        send(thr_data->socket, encrypt_mess, mx_strlen(encrypt_mess), 0);

        free(encrypt_mess);
        free(req);
        free(username);
        free(user_id_s);
        if (!mx_wait_answer(db, thr_data)) return;
    }

    sqlite3_finalize(stmt);
}

void mx_foo(sqlite3 *db, char *data, Thread_data *thr_data) {
    char **commands = mx_strsplit(data, '|');
    int res = 0;
    if (mx_strcmp(commands[0], "cn") == 0) {
        if (mx_is_user_in_chat(db, mx_atoi(commands[1]), (*thr_data).id)) {
            char *encripted = mx_encrypting_data("om", (*thr_data).client_public_key);
            send((*thr_data).socket, encripted, mx_strlen(encripted), 0);
            (*thr_data).current_chat = mx_atoi(commands[1]);
            mx_get_messages(db, thr_data);
            free(encripted);
        }
    }

    else if (mx_strcmp(commands[0], "ci") == 0) {
        mx_get_chats(db, thr_data);
    }
    else if (mx_strcmp(commands[0], "cc") == 0) {
        char *cur_chat = mx_strjoin("cc|", mx_itoa((*thr_data).current_chat));
        char *encripted = mx_encrypting_data(cur_chat, (*thr_data).client_public_key);
        send((*thr_data).socket, encripted, mx_strlen(encripted), 0);
        free(cur_chat);
        free(encripted);
    }
    else if (mx_strcmp(commands[0], "mc") == 0) {
        res = mx_create_message(db, thr_data, commands[1]);
        if (res > 0) {
            mx_send_message(db, res, "ms", thr_data);
        }
    }
    else if (mx_strcmp(commands[0], "me") == 0) {
        if (mx_edit_message_safe(db, (*thr_data).current_chat, mx_atoi(commands[1]), (*thr_data).id, commands[2])) {
            mx_send_message(db, mx_atoi(commands[1]), "me", thr_data);
        }
    }
    else if (mx_strcmp(commands[0], "md") == 0) {
        if (mx_delete_message_safe(db, (*thr_data).current_chat, mx_atoi(commands[1]), (*thr_data).id)) {
            mx_send_message(db, mx_atoi(commands[1]), "me", thr_data);
        }
    }
    else if (mx_strcmp(commands[0], "pe") == 0) {
        mx_send_all_last_messages(db, thr_data);
    }
    else if (mx_strcmp(commands[0], "us") == 0) {
        mx_send_user_info_by_username(db, commands[2], thr_data, commands[1]);
    }
    else if (mx_strcmp(commands[0], "cr") == 0) {
        (*thr_data).current_chat = mx_create_chat(db, commands[1], (*thr_data).id);
        if (commands[2]) {
            for (int i = 2; commands[i]; i++) {
                mx_add_user_in_chat(db, (*thr_data).current_chat, mx_atoi(commands[i]));
            }
        }
        mx_send_chats_inf(db, "ci", thr_data);
    }
    else if (mx_strcmp(commands[0], "ca") == 0) {
        mx_add_users_in_chat(db, data, (*thr_data).current_chat);
        mx_send_chats_inf_s(db, "ce", thr_data, data);
    }
    else if (mx_strcmp(commands[0], "cd") == 0) {
        mx_send_chats_inf_s(db, "cd", thr_data, data);
        mx_del_users_from_chat(db, data, (*thr_data).current_chat);
        if (!mx_wait_answer(db, thr_data)) return;
        mx_send_chats_inf(db, "ce", thr_data);
    }
    else if (mx_strcmp(commands[0], "scn") == 0) {
        if (mx_change_chat_name(db, (mx_atoi(commands[1]) ? mx_strcmp(commands[1], "-") != 0: (*thr_data).current_chat), commands[2])) {
            mx_send_chats_inf(db, "ce", thr_data);
        }
    }
    else if (mx_strcmp(commands[0], "cui") == 0) {
        mx_get_users_in_chat(db, mx_atoi(commands[1]), thr_data);
    }
}

void* foo(void* p) {
    Thread_data data = *(Thread_data *)p;
    printf("Новое подключение!\n");
    ((Thread_data *)p)->thread_work = true;
    mx_handle_request((data.socket), (Thread_data *)p);
    printf("Закрытие подключения!\n");
    ((Thread_data *)p)->thread_work = false;
    close(data.socket);
    return NULL;
}

bool mx_wait_answer(sqlite3 *db, Thread_data *data) {
    char buffer[BUFFER_SIZE] = {0};
    printf("id socket: %i  ", (*data).socket);
    int size_mess = read((*data).socket, buffer, BUFFER_SIZE);
    printf("buffer: %s  %i\n", buffer, size_mess);
    if (size_mess == 0) return 0;
    if (size_mess == 1 || mx_strcmp(buffer, "1") == 0) return 1;
    else {
        printf("here!");
        char *decrypted_mess = mx_decrypting_data(buffer, (*data).server_key_pair->private_key);
        printf("in wait decrypt: %s\n", decrypted_mess);
        mx_foo(db, buffer, data);
        free(decrypted_mess);
        return 1;
    }
}

