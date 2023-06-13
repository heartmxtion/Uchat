#include "../inc/client.h"

void mx_edit_message(DataForChat *data, char *message_data) {
    char **temp = mx_strsplit(message_data, '|');
    GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(data->builder, "chat_field"));
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_field), (mx_atoi(temp[2]) - 1));
    GtkWidget *content_label = mx_find_label_in_box(GTK_WIDGET(row), "comms");
    gtk_label_set_text(GTK_LABEL(content_label), temp[5]);
    for (int i = 0; temp[i] != NULL; i++) free(temp[i]);
    free(temp);
}

void show_notification(char *tittle, char *message) {
    // notify_init("Uchat");
    // const char *sound_path = "client/resources/notify.wav";

    // GVariant *sound_variant = g_variant_new_string(sound_path);
    // NotifyNotification *notification = notify_notification_new(tittle, message, NULL);
    // notify_notification_set_hint(notification, "sound-file", sound_variant);
    // notify_notification_set_timeout(notification, 5000);
    // notify_notification_set_category(notification, "im.received");
    // g_object_ref(notification);
    // notify_notification_show(notification, NULL);

    // g_object_unref(G_OBJECT(notification));
    // g_variant_unref(sound_variant);

    // notify_uninit();
}

gboolean create_message_w(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    DataForChat *data_chat = user_data->data;
    GtkBuilder *message_builder = gtk_builder_new_from_file("client/resources/message.xml");
    GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(data_chat->builder, "chat_field"));
    GtkWidget *text_label = GTK_WIDGET(gtk_builder_get_object(message_builder, "comment_label"));
    GtkWidget *nickname_label = GTK_WIDGET(gtk_builder_get_object(message_builder, "nickname_label"));
    GtkWidget *message_box = GTK_WIDGET(gtk_builder_get_object(message_builder, "message_box"));
    GtkWidget *time_label = GTK_WIDGET(gtk_builder_get_object(message_builder, "time_label"));
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');
    char **time_list = mx_strsplit(temp[6], ' ');
    char *time = mx_strndup(time_list[1], 5);
    char *cur_time = mx_time_to_local(time);
    GList *children = gtk_container_get_children(GTK_CONTAINER(chat_field));

    PangoAttrList *default_attrs = pango_attr_list_new();
    PangoAttribute *color_attr = pango_attr_foreground_new(47545, 53199, 64250);
    pango_attr_list_insert(default_attrs, color_attr);
    gtk_label_set_attributes(GTK_LABEL(text_label), default_attrs);
    pango_attr_list_unref(default_attrs);

    // Проверяем, есть ли тег в начале сообщения
    PangoAttrList *attr_list = NULL;
    char *text = temp[5];
    if (mx_strlen(text) >= 3 && text[0] == '<') {
        attr_list = pango_attr_list_new();
        while (text[0] == '<' && text[2] == '>') {
            if (text[1] == 'b') {
                pango_attr_list_insert(attr_list, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
                text += 3;
            } else if (text[1] == 'i') {
                pango_attr_list_insert(attr_list, pango_attr_style_new(PANGO_STYLE_ITALIC));
                text += 3;
            } else if (text[1] == 'u') {
                pango_attr_list_insert(attr_list, pango_attr_underline_new(PANGO_UNDERLINE_SINGLE));
                text += 3;
            } else if (text[1] == 's') {
                pango_attr_list_insert(attr_list, pango_attr_strikethrough_new(TRUE));
                text += 3;
            } else if (text[1] == 'm') {
                pango_attr_list_insert(attr_list, pango_attr_family_new("Monospace")); 
                text += 3;
            } else if (text[1] == 'y') {
                pango_attr_list_insert(attr_list, pango_attr_foreground_new(65535, 65535, 0));
                text += 3;
            } else {
                text += 3;
            }
        }
    }

    if (attr_list == NULL) {
        gtk_label_set_text(GTK_LABEL(text_label), text);
    } else {
        // Применяем атрибуты к тегу
        PangoLayout *layout = gtk_widget_create_pango_layout(text_label, NULL);
        pango_layout_set_attributes(layout, attr_list);
        gtk_label_set_attributes(GTK_LABEL(text_label), attr_list);
        gtk_label_set_text(GTK_LABEL(text_label), text);
        g_object_unref(layout);
        pango_attr_list_unref(attr_list);
    }

    if (children != NULL) {
        GtkListBoxRow *last_row = GTK_LIST_BOX_ROW(g_list_last(children)->data);
        GtkWidget *prev_nick = mx_find_widget_by_name(GTK_WIDGET(last_row), "nick");
        const char *prev_nick_text = gtk_label_get_text(GTK_LABEL(prev_nick));
        if (mx_strcmp(prev_nick_text, temp[4]) == 0) gtk_widget_set_visible(nickname_label, FALSE);
    }
    g_list_free(children);
    
    gtk_label_set_text(GTK_LABEL(text_label), text);
    gtk_label_set_text(GTK_LABEL(nickname_label), temp[4]);
    gtk_label_set_text(GTK_LABEL(time_label), cur_time);
    gtk_list_box_insert(GTK_LIST_BOX(chat_field), message_box, -1);
    for (int i = 0; temp[i] != NULL; i++) free(temp[i]);
    for (int i = 0; time_list[i] != NULL; i++) free(time_list[i]);
    free(time_list);
    free(cur_time);
    free(time);
    free(temp);
    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_create_message(DataForChat *data, char *message_data) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(message_data);

    g_idle_add(create_message_w, user_data);
}

void mx_read_messages(DataForChat *data) {
    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        send(data->conn.socket, "1", 1, 0);
        mx_memset(buffer, '\0', BUFFER_SIZE);
        int valread = recv(data->conn.socket, buffer, BUFFER_SIZE, 0);
        if (valread == 0) {
            printf("Server closed connection\n");
            close(data->conn.socket);
            break;
        }
        char *decrypted_mess = mx_decrypting_data(buffer, data->client_pair.private_key);
        if (mx_strcmp(decrypted_mess, "end") == 0) {
            free(decrypted_mess);
            break;
        }
        mx_create_message(data, decrypted_mess);
        free(decrypted_mess);
    }
}

void mx_get_old_messages(DataForChat *data, int chat) {
    char buffer[BUFFER_SIZE] = {0};
    char *num = mx_itoa(chat);
    Connection conn = data->conn;
    char *rec = mx_strjoin("cn|", num);
    char *encrypted_mess = mx_encrypting_data(rec, data->server_key);
    send(conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    char *decrypted_mess = NULL;
    free(encrypted_mess);
    free(rec);
    free(num);
    if (recv(data->conn.socket, buffer, BUFFER_SIZE, 0) == 0) return;
    while (1) {
        send(conn.socket, "1", 1, 0);
        mx_memset(buffer, '\0', BUFFER_SIZE);
        int valread = recv(data->conn.socket, buffer, BUFFER_SIZE, 0);
        if (valread == 0) {
            printf("Server closed connection\n");
            close(conn.socket);
            break;
        }
        decrypted_mess = mx_decrypting_data(buffer, data->client_pair.private_key);
        if (mx_strcmp(decrypted_mess, "end") == 0) break;
        mx_create_message(data, decrypted_mess);
        free(decrypted_mess);
    }
    if (decrypted_mess != NULL) free(decrypted_mess);
    send(conn.socket, "1", 1, 0);
}


gboolean add_last_message(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    char** temp = mx_strsplit(user_data->decrypted_mess, '|');
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "chat_list"));
    GtkWidget *chat_row = mx_find_widget_by_name(chat_list, temp[1]);
    GtkWidget *last_mess = mx_find_label_in_box(chat_row, "last_mess");
    GtkWidget *l_mess_time = mx_find_label_in_box(chat_row, "l_mess_time");
    GtkWidget *nick_l = mx_find_label_in_box(chat_row, "nick_l");

    GtkWidget *chat_name = mx_find_label_in_box(chat_row, "chat_name");
    printf("chat name: %s\n", gtk_label_get_text(GTK_LABEL(chat_name)));

    char *text = temp[5];
    if (mx_strlen(text) >= 3 && text[0] == '<') {
        while (text[0] == '<' && text[2] == '>') {
            text += 3;
        }
    }

    char **time_temp = mx_strsplit(temp[6], ' ');
    char *time = mx_strndup(time_temp[1], 5);
    char *curr_time = mx_time_to_local(time);
    char *nick = mx_strjoin(temp[4], ": ");

    gtk_label_set_text(GTK_LABEL(nick_l), nick);
    gtk_label_set_text(GTK_LABEL(last_mess), text);
    gtk_label_set_text(GTK_LABEL(l_mess_time), curr_time);
    
    for (int i = 0; temp[i]; i++) free(temp[i]);
    for (int i = 0; time_temp[i]; i++) free(time_temp[i]);
    free(time_temp);
    free(time);
    free(curr_time);
    free(temp);
    free(nick);

    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_add_last_message(DataForChat *data, char *decrypted_mess) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;

    if (mx_strlen(decrypted_mess) >= 3 && decrypted_mess[0] == '<') {
        while (decrypted_mess[0] == '<' && decrypted_mess[2] == '>') {
            decrypted_mess += 3;
        }
    }

    user_data->decrypted_mess = g_strdup(decrypted_mess);
    g_idle_add(add_last_message, user_data);
}

