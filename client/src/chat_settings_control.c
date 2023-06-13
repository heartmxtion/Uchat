#include "../inc/client.h"


void on_user_row_selected(GtkWidget *w, GdkEventButton *event, gpointer data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkListBox *listbox = GTK_LIST_BOX(w);
    GtkWidget *context_menu = GTK_WIDGET(gtk_builder_get_object(builder, "user_context_menu"));
    
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        GtkListBoxRow *selected_row = gtk_list_box_get_row_at_y(listbox, event->y);
        if (selected_row != NULL) {
            int row_index = gtk_list_box_row_get_index(selected_row);
            data->row_id = row_index;
            printf("Right clicked on user %d\n", row_index);
            gtk_widget_show_all(context_menu);
            gtk_menu_popup_at_pointer(GTK_MENU(context_menu), (GdkEvent *)event);
        }
    }

}

void del_user_butt_activate(GtkMenuItem *menuitem, gpointer data_chat) {
    (void)menuitem;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    if (data->del_menu == 1) {
        GtkListBox *user_container = GTK_LIST_BOX(gtk_builder_get_object(builder, "user_container"));
        GtkListBoxRow *row = gtk_list_box_get_row_at_index(user_container, data->row_id);
        gtk_container_remove(GTK_CONTAINER(user_container), GTK_WIDGET(row));
    }
    if (data->del_menu == 2) {
        GtkListBox *listbox_sett_members = GTK_LIST_BOX(gtk_builder_get_object(builder, "listbox_sett_members"));
        GtkListBoxRow *row = gtk_list_box_get_row_at_index(listbox_sett_members, data->row_id);
        GtkWidget *user_id_l = mx_find_widget_by_name(GTK_WIDGET(row), "user_id_l");
        const char *user_id_text = gtk_label_get_text(GTK_LABEL(user_id_l));
        int user_id_num = mx_atoi(user_id_text);
        if (!remove_item_from_list(data->new_users, user_id_num)) {
            data->ban_users = mx_add_in_list(data->ban_users, user_id_num);
        }
        gtk_container_remove(GTK_CONTAINER(listbox_sett_members), GTK_WIDGET(row));
    }
}

void on_back_settings_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;

    // GtkWidget *chat_settings = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_chat_settings"));
    GtkWidget *entry_name = GTK_WIDGET(gtk_builder_get_object(builder, "entry_name"));
    GtkWidget *member_name = GTK_WIDGET(gtk_builder_get_object(builder, "member_name"));
    GtkWidget *chat_settings = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_chat_settings"));
    GtkWidget *apply_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "apply_error_label"));
    GtkWidget *listbox_sett_members = GTK_WIDGET(gtk_builder_get_object(data->builder, "listbox_sett_members"));
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users_list"));
    free(data->new_users);
    free(data->ban_users);
    data->new_users = (int *)malloc(sizeof(int) * 1);
    data->ban_users = (int *)malloc(sizeof(int) * 1);
    data->new_users[0] = -1;
    data->ban_users[0] = -1;
    gtk_widget_hide(chat_settings);
    gtk_label_set_text(GTK_LABEL(apply_error_label), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(member_name), "");
    g_signal_handler_disconnect(finded_users_list, data->handler_id);
    clear_widget(listbox_sett_members);
}

void on_apply_settings_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkWidget *entry_name = GTK_WIDGET(gtk_builder_get_object(builder, "entry_name"));
    GtkWidget *apply_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "apply_error_label"));
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users_list"));
    GtkWidget *chat_settings = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_chat_settings"));
    GtkWidget *listbox_sett_members = GTK_WIDGET(gtk_builder_get_object(data->builder, "listbox_sett_members"));
    GtkWidget *member_name = GTK_WIDGET(gtk_builder_get_object(builder, "member_name"));
    GtkWidget *current_chat_name = GTK_WIDGET(gtk_builder_get_object(builder, "current_chat_name"));
    
    const char *current_chat_name_s = gtk_label_get_text(GTK_LABEL(current_chat_name));
    const char *new_chat_name_s = gtk_entry_get_text(GTK_ENTRY(entry_name));

    if (mx_strcmp(current_chat_name_s, new_chat_name_s) != 0) {
        char *req = mx_strjoin("scn|-|", new_chat_name_s);
        char *enc_mess = mx_encrypting_data(req, data->server_key);

        send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);

        free(enc_mess);
        free(req);
        g_usleep(10000);
    }

    if (data->ban_users[0] != -1) {
        char *req = mx_strdup("cd|-");
        for (int i = 0; data->ban_users[i] > 0; i++) {
            char *user_id_s = mx_itoa(data->ban_users[i]);
            req = mx_ljoin(req, "|");
            req = mx_ljoin(req, user_id_s);
            free(user_id_s);
        }
        char *enc_mess = mx_encrypting_data(req, data->server_key);

        send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);

        free(enc_mess);
        free(req);
        g_usleep(10000);
    }

    if (data->new_users[0] != -1) {
        char *req = mx_strdup("ca|-");
        for (int i = 0; data->new_users[i] > 0; i++) {
            char *user_id_s = mx_itoa(data->new_users[i]);
            req = mx_ljoin(req, "|");
            req = mx_ljoin(req, user_id_s);
            free(user_id_s);
        }
        char *enc_mess = mx_encrypting_data(req, data->server_key);

        send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);

        free(enc_mess);
        free(req);
    }

    gtk_widget_hide(chat_settings);
    gtk_label_set_text(GTK_LABEL(apply_error_label), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(member_name), "");
    g_signal_handler_disconnect(finded_users_list, data->handler_id);
    clear_widget(listbox_sett_members);
}

void on_finded_row_selected1(GtkWidget *w, GdkEventButton *event, gpointer data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkListBox *listbox = GTK_LIST_BOX(w);
    GtkWidget *popover = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users"));
    GtkBuilder *chat_builder = gtk_builder_new_from_file("client/resources/founded_info.xml");
    GtkWidget *finded_info = GTK_WIDGET(gtk_builder_get_object(chat_builder, "finded_info"));
    GtkWidget *nickname = GTK_WIDGET(gtk_builder_get_object(chat_builder, "nickname"));
    GtkWidget *user_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "user_id"));
    GtkWidget *listbox_sett_members = GTK_WIDGET(gtk_builder_get_object(data->builder, "listbox_sett_members"));
    
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_PRIMARY) {
        GtkListBoxRow *selected_row = gtk_list_box_get_row_at_y(listbox, event->y);

        if (selected_row != NULL) {
            GtkWidget *nickname_field = mx_find_widget_by_name(GTK_WIDGET(selected_row), "nickname_l");
            const char *users_field_text = gtk_label_get_text(GTK_LABEL(nickname_field));
            if (mx_strcmp(users_field_text, "User not founded!") != 0 && !is_user_in_list(listbox_sett_members, (char *)users_field_text)) {
                GtkWidget *user_id_l = mx_find_widget_by_name(GTK_WIDGET(selected_row), "user_id_l");
                const char *user_id_text = gtk_label_get_text(GTK_LABEL(user_id_l));
                int user_id_num = mx_atoi(user_id_text);

                if (!remove_item_from_list(data->ban_users, user_id_num))
                    data->new_users = mx_add_in_list(data->new_users, user_id_num);
                
                gtk_label_set_text(GTK_LABEL(nickname), users_field_text);
                gtk_label_set_text(GTK_LABEL(user_id), user_id_text);
                gtk_list_box_insert(GTK_LIST_BOX(listbox_sett_members), finded_info, -1);
            }
            if (G_IS_OBJECT(chat_builder)) g_object_unref(chat_builder);
            gtk_widget_hide(popover);
        }
    }
}

void on_settings_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkWidget *finded_users_list = GTK_WIDGET(gtk_builder_get_object(data->builder, "finded_users_list"));
    GtkWidget *chat_settings = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_chat_settings"));
    GtkWidget *current_chat_name = GTK_WIDGET(gtk_builder_get_object(builder, "current_chat_name"));
    GtkWidget *entry_name = GTK_WIDGET(gtk_builder_get_object(builder, "entry_name"));
    
    const char *current_chat_name_s = gtk_label_get_text(GTK_LABEL(current_chat_name));
    gtk_entry_set_text(GTK_ENTRY(entry_name), current_chat_name_s);
    data->del_menu = 2;
    char *curr_chat = mx_itoa(data->current_chat);
    char *req = mx_strjoin("cui|", curr_chat);
    char *enc_mess = mx_encrypting_data(req, data->server_key);

    send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);

    free(data->new_users);
    free(data->ban_users);
    data->new_users = (int *)malloc(sizeof(int) * 1);
    data->ban_users = (int *)malloc(sizeof(int) * 1);
    data->new_users[0] = -1;
    data->ban_users[0] = -1;

    free(curr_chat);
    free(req);
    free(enc_mess);
    data->handler_id = g_signal_connect(finded_users_list, "button-press-event", G_CALLBACK(on_finded_row_selected1), data_chat);
    gtk_widget_show_all(chat_settings);
}

void on_search_member_name_changed(GtkSearchEntry *entry, gpointer user_data) {
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

gboolean add_user_in_list_callback(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');
    GtkWidget *listbox_sett_members = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "listbox_sett_members"));
    GtkBuilder *chat_builder = gtk_builder_new_from_file("client/resources/founded_info.xml");
    GtkWidget *finded_info = GTK_WIDGET(gtk_builder_get_object(chat_builder, "finded_info"));
    GtkWidget *nickname = GTK_WIDGET(gtk_builder_get_object(chat_builder, "nickname"));
    GtkWidget *user_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "user_id"));

    gtk_label_set_text(GTK_LABEL(nickname), temp[2]);
    gtk_label_set_text(GTK_LABEL(user_id), temp[1]);
    gtk_list_box_insert(GTK_LIST_BOX(listbox_sett_members), finded_info, -1);

    for (int i = 0; temp[i] != NULL; i++) free(temp[i]);
    free(temp);
    g_object_unref(chat_builder);
    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_add_user_in_list(DataForChat *data, char *decrypted_mess) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(decrypted_mess);

    g_idle_add(add_user_in_list_callback, user_data);
}

