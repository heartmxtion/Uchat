#include "../inc/client.h"

void on_finded_row_selected(GtkWidget *w, GdkEventButton *event, gpointer data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkListBox *listbox = GTK_LIST_BOX(w);
    GtkWidget *popover = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users"));
    GtkBuilder *chat_builder = gtk_builder_new_from_file("client/resources/founded_info.xml");
    GtkWidget *finded_info = GTK_WIDGET(gtk_builder_get_object(chat_builder, "finded_info"));
    GtkWidget *nickname = GTK_WIDGET(gtk_builder_get_object(chat_builder, "nickname"));
    GtkWidget *user_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "user_id"));
    GtkWidget *users_field = GTK_WIDGET(gtk_builder_get_object(data->builder, "user_container"));
    
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_PRIMARY) {
        GtkListBoxRow *selected_row = gtk_list_box_get_row_at_y(listbox, event->y);

        if (selected_row != NULL) {
            GtkWidget *nickname_field = mx_find_widget_by_name(GTK_WIDGET(selected_row), "nickname_l");
            const char *users_field_text = gtk_label_get_text(GTK_LABEL(nickname_field));
            if (mx_strcmp(users_field_text, "User not founded!") != 0) {
                GtkWidget *user_id_l = mx_find_widget_by_name(GTK_WIDGET(selected_row), "user_id_l");
                const char *user_id_text = gtk_label_get_text(GTK_LABEL(user_id_l));
                gtk_label_set_text(GTK_LABEL(nickname), users_field_text);
                gtk_label_set_text(GTK_LABEL(user_id), user_id_text);
                gtk_list_box_insert(GTK_LIST_BOX(users_field), finded_info, -1);
            }
            if (G_IS_OBJECT(chat_builder)) g_object_unref(chat_builder);
            gtk_widget_hide(popover);
        }
    }
}

gboolean show_users_w(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');
    GtkBuilder *chat_builder = gtk_builder_new_from_file("client/resources/founded_info.xml");
    GtkWidget *finded_info = GTK_WIDGET(gtk_builder_get_object(chat_builder, "finded_info"));
    GtkWidget *nickname = GTK_WIDGET(gtk_builder_get_object(chat_builder, "nickname"));
    GtkWidget *user_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "user_id"));

    if (mx_strcmp(temp[2], "-") != 0) {
        gtk_label_set_text(GTK_LABEL(nickname), temp[3]);
        gtk_label_set_text(GTK_LABEL(user_id), temp[2]);
    }
    else gtk_label_set_text(GTK_LABEL(nickname), "User not founded!");

    if (mx_strcmp(temp[1], "1") == 0) {
        GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "finded_users_list"));
        gtk_list_box_insert(GTK_LIST_BOX(finded_users_list), finded_info, -1);
    }

    for (int i = 0; temp[i]; i++) free(temp[i]);
    free(temp);
    if (G_IS_OBJECT(chat_builder)) g_object_unref(chat_builder);
    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_show_users(DataForChat *data, char *decrypted_mess) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(decrypted_mess);

    g_idle_add(show_users_w, user_data);
}


void on_chat_create_user_search_changed(GtkSearchEntry *entry, gpointer user_data) {
    DataForChat *data = (DataForChat *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkWidget *popover = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users"));
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users_list"));
    clear_widget(finded_users_list);
    gtk_popover_set_relative_to(GTK_POPOVER(popover), GTK_WIDGET(entry));

    if (mx_strlen(text) >= 4) {
        gtk_widget_show(popover);
        char *req = mx_strjoin("us|1|", text);
        char *encoded_mess = mx_encrypting_data(req, data->server_key);
        send(data->conn.socket, encoded_mess, mx_strlen(encoded_mess), 0);
        free(encoded_mess);
        free(req);
    }
    else gtk_widget_hide(popover);
}

void on_cancel_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;

    GtkWidget *chat_name = GTK_WIDGET(gtk_builder_get_object(builder, "chat_name"));
    GtkWidget *chart_create_search_user = GTK_WIDGET(gtk_builder_get_object(builder, "user_searchss"));
    GtkWidget *add_chat = GTK_WIDGET(gtk_builder_get_object(builder, "create_chat"));
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(builder, "finded_users_list"));
    g_signal_handler_disconnect(finded_users_list, data->handler_id);
    gtk_entry_set_text(GTK_ENTRY(chart_create_search_user), "");
    gtk_entry_set_text(GTK_ENTRY(chat_name), "");
    gtk_widget_hide(add_chat);
}

void on_create_chat_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    
    GtkWidget *chart_create_search_user = GTK_WIDGET(gtk_builder_get_object(builder, "user_searchss"));
    GtkWidget *add_chat = GTK_WIDGET(gtk_builder_get_object(builder, "create_chat"));
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(builder, "finded_users_list"));
    GtkWidget *create_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "create_error_label"));
    GtkWidget *chat_name = GTK_WIDGET(gtk_builder_get_object(builder, "chat_name"));
    const char *text = gtk_entry_get_text(GTK_ENTRY(chat_name));
    if (mx_strlen(text) >= 4) {
        char *req = mx_strjoin("cr|", text);
        req = mx_ljoin(req, "|");
        GtkWidget *users_field = GTK_WIDGET(gtk_builder_get_object(data->builder, "user_container"));
        GList *children = gtk_container_get_children(GTK_CONTAINER(users_field));

        if (children != NULL) {
            for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
                GtkWidget *child = GTK_WIDGET(iter->data);
                GtkWidget *us_id = mx_find_widget_by_name(child, "user_id_l");
                const char *user_id = gtk_label_get_text(GTK_LABEL(us_id));
                req = mx_ljoin(req, user_id);
                req = mx_ljoin(req, "|");
            }
        }
        g_list_free(children);
        char *temp = mx_strndup(req, mx_strlen(req) - 1);
        char *enc_mess = mx_encrypting_data(temp, data->server_key);
        send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);
        free(enc_mess);
        free(temp);
        free(req);
        gtk_label_set_text(GTK_LABEL(create_error_label), "");
        gtk_entry_set_text(GTK_ENTRY(chat_name), "");
        gtk_entry_set_text(GTK_ENTRY(chart_create_search_user), "");
        g_signal_handler_disconnect(finded_users_list, data->handler_id);
        gtk_widget_hide(add_chat);
    }
    else gtk_label_set_text(GTK_LABEL(create_error_label), "Chat name is too short");
}

void on_add_chat_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    
    GtkWidget *add_chat = GTK_WIDGET(gtk_builder_get_object(builder, "create_chat"));
    data->del_menu = 1; 
    
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(builder, "finded_users_list"));
    GtkWidget *users_field = GTK_WIDGET(gtk_builder_get_object(builder, "user_container"));
    clear_widget(users_field);
    data->handler_id = g_signal_connect(finded_users_list, "button-press-event", G_CALLBACK(on_finded_row_selected), data_chat);
    gtk_widget_show_all(add_chat);
    
}
