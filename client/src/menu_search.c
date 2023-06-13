#include "../inc/client.h"

void mx_nick_manager(GtkWidget *mess, gchar **prev_nick, gboolean option) {
    GtkWidget *nickname_label = mx_find_label_in_box(mess, "nick");
    const gchar *nick = gtk_label_get_text(GTK_LABEL(nickname_label));
    
    if (*prev_nick != NULL && g_strcmp0(nick, *prev_nick) == 0) {
        gtk_widget_set_visible(nickname_label, option);
    } else {
        g_free(*prev_nick);
        *prev_nick = g_strdup(nick);
    }
}

void on_search_changed(GtkSearchEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *prev_nick = NULL;
    GtkListBox *list_box = GTK_LIST_BOX(user_data);
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    gchar *lowercase_text = g_utf8_strdown(text, -1);

    if (g_strcmp0(text, "") == 0) {
        for (GList *i = children; i != NULL; i = i->next) {
            GtkWidget *mess = GTK_WIDGET(i->data);
            mx_nick_manager(mess, &prev_nick, FALSE);
            gtk_widget_show(mess);
        }

        g_list_free(children);
        g_free(prev_nick);
        g_free(lowercase_text);
        return;
    }

    for (GList *i = children; i != NULL; i = i->next) {
        GtkWidget *mess = GTK_WIDGET(i->data);
        GtkWidget *content_label = mx_find_label_in_box(mess, "comms");
        GtkWidget *nickname_label = mx_find_label_in_box(mess, "nick");

        if (content_label != NULL && nickname_label != NULL) {
            const char *content_text = gtk_label_get_text(GTK_LABEL(content_label));
            gchar *lowercase_content_text = g_utf8_strdown(content_text, -1);

            const char *nick_text = gtk_label_get_text(GTK_LABEL(nickname_label));
            gchar **text_split = g_strsplit(text, ":", 2);
            if (text_split[1] != NULL) {
                gchar *lowercase_content = g_utf8_strdown(text_split[1], -1);
                if (g_ascii_strcasecmp(nick_text, text_split[0]) == 0 && g_strstr_len(lowercase_content_text, -1, lowercase_content) 
                            && !g_strstr_len(content_text, -1, "*message deleted*")) {
                    gtk_widget_set_visible(nickname_label, TRUE);
                    mx_nick_manager(mess, &prev_nick, FALSE);
                    gtk_widget_show(mess);
                } else if (g_strstr_len(lowercase_content_text, -1, lowercase_text) && !g_strstr_len(content_text, -1, "*message deleted*")) {
                    gtk_widget_set_visible(nickname_label, TRUE);
                    mx_nick_manager(mess, &prev_nick, FALSE);
                    gtk_widget_show(mess);
                } else {
                    gtk_widget_hide(mess);
                }
                free(lowercase_content);
            }
            else {
                if (g_ascii_strcasecmp(nick_text, text_split[0]) == 0 && !g_strstr_len(content_text, -1, "*message deleted*")) {
                    gtk_widget_set_visible(nickname_label, TRUE);
                    mx_nick_manager(mess, &prev_nick, FALSE);
                    gtk_widget_show(mess);
                } else if (g_strstr_len(lowercase_content_text, -1, lowercase_text) && !g_strstr_len(content_text, -1, "*message deleted*")) {
                    gtk_widget_set_visible(nickname_label, TRUE);
                    mx_nick_manager(mess, &prev_nick, FALSE);
                    gtk_widget_show(mess);
                } else {
                    gtk_widget_hide(mess);
                }
            }
            g_free(lowercase_content_text);
            g_strfreev(text_split);
        }
    }

    g_list_free(children);
    g_free(lowercase_text);
}

// char **mx_get_search_data(char *data) {
//     if (mx_strcmp(data, "") == 0) return NULL;
//     char *s = mx_strstr(data, ":");
//     char **search_data = malloc(sizeof(char *) * 2);
//     search_data[0] = NULL;
//     if (data[0] == '@' && s != 0) {
//         char *start = data;
//         char *user = mx_strndup(++start, s - data - 1);
//         if (mx_strlen(user) > 0) search_data[0] = user;
//         else search_data[0] = NULL;
//     }
//     if (s == 0 || search_data[0] == NULL) {
//         search_data[1] = data;
//         return search_data;
//     }
//     else {
//         s++;
//         if (s[0] == ' ') s++;
//         char *req = mx_strdup(s);
//         if (mx_strlen(req) == 0) search_data[1] = NULL;
//         else search_data[1] = req;
//     }
//     return search_data;
// }

// void on_search_changed(GtkSearchEntry *entry, gpointer user_data) { //.h
//     const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
//     GtkListBox *list_box = GTK_LIST_BOX(user_data);
//     GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
//     char **search_data = mx_get_search_data((char *)text);

//     if (g_strcmp0(text, "") == 0) {
//         for (GList *i = children; i != NULL; i = i->next) {
//             GtkWidget *mess = GTK_WIDGET(i->data);
//             gtk_widget_show(mess);
//         }
//         g_list_free(children);
//         if (search_data != NULL) {
//             if (search_data[0] != NULL) free(search_data[0]);
//             if (search_data[1] != NULL) free(search_data[1]);
//             free(search_data);
//         }
//         return;
//     }

//     for (GList *i = children; i != NULL; i = i->next) {
//         GtkWidget *mess = GTK_WIDGET(i->data);
//         GtkWidget *content_label = mx_find_label_in_box(mess, "comms");
//         GtkWidget *nickname_label = mx_find_label_in_box(mess, "nick");
//         if (content_label != NULL) {
//             const char *nick_text = gtk_label_get_text(GTK_LABEL(nickname_label));
//             const char *content_text = gtk_label_get_text(GTK_LABEL(content_label));
//             gchar *lowercase_content_text = g_utf8_strdown(content_text, -1);

//             // if (g_strcmp0(text, "") == 0) gtk_widget_show(mess);
//             if (mx_strcmp(content_text, "*message deleted*") != 0) {
//                 if (search_data[0] && !search_data[1]) {
//                     if (mx_strcmp(nick_text, search_data[0]) == 0)
//                         gtk_widget_show(mess);
//                     else gtk_widget_hide(mess);
//                     // free(search_data[0]);
//                 }
//                 if (!search_data[0] && search_data[1]) {
//                     gchar *lowercase_text = g_utf8_strdown(search_data[1], -1);
//                     if (g_strstr_len(lowercase_content_text, -1, lowercase_text))
//                         gtk_widget_show(mess);
//                     else gtk_widget_hide(mess);
//                     free(lowercase_text);
//                     // free(search_data[1]);
//                 }
//                 if (search_data[0] && search_data[1]) {
//                     gchar *lowercase_text = g_utf8_strdown(search_data[1], -1);
//                     if (mx_strcmp(nick_text, search_data[0]) == 0 && g_strstr_len(lowercase_content_text, -1, lowercase_text))
//                         gtk_widget_show(mess);
//                     else gtk_widget_hide(mess);
//                     free(lowercase_text);
//                     // free(search_data[0]);
//                     // free(search_data[1]);
//                 }
//                 // free(search_data);
//             } else gtk_widget_hide(mess);
//             g_free(lowercase_content_text);
//         }
//     }
//     g_list_free(children);
//     if (search_data != NULL) {
//         if (search_data[0] != NULL) free(search_data[0]);
//         if (search_data[1] != NULL) free(search_data[1]);
//         free(search_data);
//     }
// }

void on_search_chats_changed(GtkSearchEntry *entry, gpointer chat_list) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkListBox *list_box = GTK_LIST_BOX(chat_list);
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    gchar *lowercase_text = g_utf8_strdown(text, -1);

    for (GList *i = children; i != NULL; i = i->next) {
        GtkWidget *mess = GTK_WIDGET(i->data);
        GtkWidget *content_label = mx_find_label_in_box(mess, "chat_name");
        if (content_label != NULL) {
            const char *content_text = gtk_label_get_text(GTK_LABEL(content_label));
            gchar *lowercase_content_text = g_utf8_strdown(content_text, -1);
            if (g_strstr_len(lowercase_content_text, -1, lowercase_text)) {
                gtk_widget_show(mess);
            } else {
                gtk_widget_hide(mess);
            }
            g_free(lowercase_content_text);
        }
    }
    g_list_free(children);
    g_free(lowercase_text);
}

void on_menuitem_tag_activate(GtkMenuItem *menuitem, gpointer data_chat) {
    (void)menuitem;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkListBox *listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "chat_field"));
    GtkTextView *message_field = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "message_field"));
    int row_index = data->row_id;
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(listbox, row_index);
    GtkWidget *login = mx_find_label_in_box(GTK_WIDGET(row), "nick");
    if (login != NULL) {
        GtkTextIter end_iter;
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(message_field);
        const char *nick = gtk_label_get_text(GTK_LABEL(login));
        char *str = mx_strjoin("@", nick);
        gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
        if (gtk_text_iter_get_offset(&end_iter) > 0) {
            gtk_text_buffer_insert(text_buffer, &end_iter, " ", 1);
        }
        gtk_text_buffer_insert(text_buffer, &end_iter, str, -1);
        g_free(str);
    }
}


void on_menuitem_edit_activate(GtkMenuItem *menuitem, gpointer data_chat) {
    (void)menuitem;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkListBox *listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "chat_field"));
    GtkTextView *message_field = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "message_field"));
    int row_index = data->row_id;
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(listbox, row_index);
    GtkWidget *label = mx_find_label_in_box(GTK_WIDGET(row), "comms");
    if (label != NULL) {
        const gchar *text = gtk_label_get_text(GTK_LABEL(label));
        char *num = mx_itoa(row_index + 1);
        char *str = mx_strjoin("/edit(", num);
        str = mx_ljoin(str, ") ");
        str = mx_ljoin(str, text);
        int edit_tag_length = mx_strlen_until_char(str, ' ') + 1;

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_field));
        gtk_text_buffer_set_text(buffer, str, -1);
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_iter_at_offset(buffer, &end, edit_tag_length);
        GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "editable", FALSE, NULL);
        gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
        free(num);
        g_free(str);
    }
}

void on_menuitem_del_activate(GtkMenuItem *menuitem, gpointer data_chat) { //.h
    (void)menuitem;
    DataForChat *data = (DataForChat *)data_chat;
    int row_index = -1;
    row_index = data->row_id;
    row_index++;
    Connection conn = data->conn;
    char *num = mx_itoa(row_index);
    char *message = mx_strjoin("md|", num);
    char *encrypted_mess = mx_encrypting_data(message, data->server_key);
    send(conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    free(encrypted_mess);
    free(message);
    free(num);
}

