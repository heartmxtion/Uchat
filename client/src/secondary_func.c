#include "../inc/client.h"

int *mx_add_in_list(int *list, int user_id) {
    int size = 0;
    for (int i = 0; list[i] > 0; i++) size++;
    int *temp = (int *)malloc(sizeof(int) * size + 1 + 1);
    int i = 0;
    for (i = 0; list[i] > 0; i++) temp[i] = list[i];
    temp[size] = user_id;
    temp[size + 1] = -1;
    free(list);
    return temp;
}

bool remove_item_from_list(int* list, int num) {
    for (int i = 0; list[i] != -1; i++) {
        printf(" ->>>> %i\n", list[i]);
        if (list[i] == num) {
            for (int j = i; list[j] != -1; j++) {
                list[j] = list[j + 1];
            }
            return true;
        }
    }
    return false;
}

bool is_user_in_list(GtkWidget *list, char *text) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(list));

    if (children != NULL) {
        for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
            GtkWidget *child = GTK_WIDGET(iter->data);
            GtkWidget *us_id = mx_find_widget_by_name(child, "nickname_l");
            const char *user_name = gtk_label_get_text(GTK_LABEL(us_id));
            if (mx_strcmp(user_name, text) == 0) {
                g_list_free(children);
                return true;
            }
        }
    }
    g_list_free(children);
    return false;
}

GtkWidget* mx_find_widget_by_name(GtkWidget *box, const gchar *widget_name) { //.h
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));

    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        const gchar *name = gtk_widget_get_name(child);
        if (name != NULL && g_strcmp0(name, widget_name) == 0) {
            g_list_free(children);
            return child;
        }
        if (GTK_IS_CONTAINER(child)) {
            GtkWidget *label = mx_find_widget_by_name(child, widget_name);
            if (label != NULL) {
                g_list_free(children);
                return label;
            }
        }
    }
    g_list_free(children);
    return NULL;
}

GtkWidget* mx_find_label_in_box(GtkWidget *box, const gchar *label_id) { //.h
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));

    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);

        if (GTK_IS_BOX(child)) {
            GtkWidget *label = mx_find_label_in_box(child, label_id);
            if (label != NULL) {
                g_list_free(children);
                return label;
            }
        } else if (GTK_IS_LABEL(child)) {
            const gchar *name = gtk_widget_get_name(child);
            if (name != NULL && g_strcmp0(name, label_id) == 0) {
                g_list_free(children);
            return child;
            }
        }
    }
    g_list_free(children);
    return NULL;
}

void clear_widget(GtkWidget *widget) {
    if (GTK_IS_CONTAINER(widget)) {
        GtkContainer *container = GTK_CONTAINER(widget);
        GList *children = gtk_container_get_children(container);
        if (children != NULL) {
            if (GTK_IS_LIST_BOX(widget)) {
                GtkListBox *list_box = GTK_LIST_BOX(widget);
                GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(list_box);
                if (selected_row != NULL) {
                    gtk_list_box_unselect_all(list_box);
                }
            }
            gtk_container_foreach(container, (GtkCallback)gtk_widget_destroy, NULL);
        }
        g_list_free(children);
    }
}

char *mx_ljoin(char *s1, const char *s2) {
    char *result = mx_strnew(mx_strlen(s1) + mx_strlen(s2) + 1);
    result = mx_strcat(result, s1);
    result = mx_strcat(result, s2);
    free(s1);
    return result;
}

char *mx_rjoin(const char *s1, char *s2) {
    char *result = mx_strnew(mx_strlen(s1) + mx_strlen(s2) + 1);
    result = mx_strcat(result, s1);
    result = mx_strcat(result, s2);
    free(s2);
    return result;
}

void on_window_closed(GtkWidget* widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("Клиент остановлен\n");
    gtk_main_quit();
}

char *mx_decrypting_data(char *buffer, RSA* private_key) {
    // Декодирование base64
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new_mem_buf((void*)buffer, -1);
    bmem = BIO_push(b64, bmem);
    int rsa_len = RSA_size(private_key);
    unsigned char* encrypted = malloc(rsa_len);
    int len = BIO_read(bmem, encrypted, rsa_len);
    BIO_free_all(bmem);
    if (len < rsa_len) {
        fprintf(stderr, "Error: RSA auth decryption failed (BIO_read)\n");
        free(encrypted);
        exit(1);
    }

    // Расшифровка RSA
    unsigned char* decrypted = malloc(rsa_len);
    int decrypted_len = RSA_private_decrypt(rsa_len, encrypted, decrypted, private_key, RSA_PKCS1_PADDING);
    if (decrypted_len == -1) {
        fprintf(stderr, "Error: RSA auth decryption failed\n");
        free(encrypted);
        free(decrypted);
        exit(1);
    }

    // Перевод из бинарного в строковый вид
    char* result = malloc(decrypted_len + 1);
    memcpy(result, decrypted, decrypted_len);
    result[decrypted_len] = '\0';

    free(decrypted);
    free(encrypted);

    return result;
}

char* mx_encrypting_data(const char *data, RSA* server_key) {
    int rsa_len = RSA_size(server_key);
    unsigned char* encrypted_auth = malloc(rsa_len);
    int encrypted_len = RSA_public_encrypt(mx_strlen(data), (unsigned char *)data, encrypted_auth, server_key, RSA_PKCS1_PADDING);
    if (encrypted_len == -1) {
        fprintf(stderr, "Error, failed to encrypt login and password\n");
        free(encrypted_auth);
        exit(1);
    }

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO *bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, encrypted_auth, encrypted_len);
    BIO_flush(b64);
    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(b64, &bufferPtr);
    char *result = malloc(bufferPtr->length + 1);
    memcpy(result, bufferPtr->data, bufferPtr->length);
    result[bufferPtr->length] = '\0';

    BIO_free_all(b64);
    free(encrypted_auth);
    
    return result;
}

void hash_password(const char *password, char *password_hash) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *) password, strlen(password), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&password_hash[i * 2], "%02x", (unsigned int) hash[i]);
    }
}

char *mx_log_and_pass_to_string(char *command, const char *login, const char *password) {
    char *temp = mx_strjoin(command, "|");
    char password_hash[SHA256_DIGEST_LENGTH * 2 + 1];
    hash_password(password, password_hash);

    temp = mx_ljoin(temp, login);
    temp = mx_ljoin(temp, "|");
    temp = mx_ljoin(temp, password_hash);
    
    return temp;
}

