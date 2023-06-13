#include "../inc/client.h"

gboolean create_chat_w(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    DataForChat *data_chat = user_data->data;
    GtkBuilder *chat_builder = gtk_builder_new_from_file("client/resources/chat.xml");
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(data_chat->builder, "chat_list"));
    GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(chat_builder, "chat"));
    GtkWidget *chat_name_label = GTK_WIDGET(gtk_builder_get_object(chat_builder, "chat_name_label"));
    GtkLabel *current_chat_name = GTK_LABEL(gtk_builder_get_object(data_chat->builder, "current_chat_name"));
    GtkWidget *scroll_list2 = GTK_WIDGET(gtk_builder_get_object(data_chat->builder, "scroll_list2"));
    if (gtk_widget_get_visible(scroll_list2)) {
        gtk_widget_hide(scroll_list2);
    }
    
    // GtkWidget *last_message = GTK_WIDGET(gtk_builder_get_object(chat_builder, "last_mesasge_label"));
    GtkWidget *chat_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "chat_id_l"));
    GtkWidget *user_number = GTK_WIDGET(gtk_builder_get_object(chat_builder, "user_number_l"));
    GtkWidget *admin_id = GTK_WIDGET(gtk_builder_get_object(chat_builder, "admin_id_l"));
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');
    gtk_label_set_text(GTK_LABEL(chat_name_label), temp[3]);
    gtk_label_set_text(GTK_LABEL(chat_id), temp[1]);
    gtk_widget_set_name(chat, temp[1]);
    gtk_label_set_text(GTK_LABEL(user_number), temp[2]);
    gtk_label_set_text(GTK_LABEL(admin_id), temp[4]);
    gtk_list_box_insert(GTK_LIST_BOX(chat_list), chat, -1);
    for (int i = 0; temp[i] != NULL; i++) free(temp[i]);
    free(temp);
    g_object_unref(chat_builder);
    
    int should_be_selected = data_chat->current_chat - 1;
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_list), should_be_selected);
    
    GtkWidget *label = mx_find_widget_by_name(GTK_WIDGET(row), "chat_name");
    const char *text = gtk_label_get_text(GTK_LABEL(label));
    //printf("Название чата: %s\n", text);
    gtk_label_set_text(current_chat_name, text);
    //Дублирование кода со стилем строки для того, чтобы применить стиль при загрузке
    gtk_widget_set_state_flags(GTK_WIDGET(row), GTK_STATE_FLAG_SELECTED, TRUE);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".chat_list row:selected {\n"
        "    background-color: darkslateblue;\n" //Цвет фона строки во время выделения
        "    color: white;\n" //Цвет названия чата
        "}\n", -1, NULL);
    GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(chat_list));
    GtkStyleContext *context = gtk_style_context_new();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkStyleContext *style_context = gtk_widget_get_style_context(GTK_WIDGET(row));
    gtk_style_context_add_class(style_context, "selected");
    gtk_list_box_select_row(GTK_LIST_BOX(chat_list), row);
    if (G_IS_OBJECT(chat_builder)) g_object_unref(chat_builder);
    if (G_IS_OBJECT(provider)) g_object_unref(provider);
    if (G_IS_OBJECT(context)) g_object_unref(context);
    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_create_chat_w(DataForChat *data, char *chat_data) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(chat_data);

    g_idle_add(create_chat_w, user_data);
}

gboolean edit_chat_w(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');

    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "chat_list"));
    GtkWidget *chat_row = mx_find_widget_by_name(chat_list, temp[1]);
    if (chat_row != NULL) {
        GtkWidget *chat_name = mx_find_label_in_box(chat_row, "chat_name");
        GtkWidget *user_number = mx_find_label_in_box(chat_row, "user_number");

        gtk_label_set_text(GTK_LABEL(user_number), temp[2]);
        gtk_label_set_text(GTK_LABEL(chat_name), temp[3]);
    }
    else mx_create_chat_w(user_data->data, user_data->decrypted_mess);

    for (int i = 0; temp[i]; i++) free(temp[i]);
    free(temp);

    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_edit_chat_w(DataForChat *data, char *chat_data) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(chat_data);

    g_idle_add(edit_chat_w, user_data);
}

gboolean del_chat_w(gpointer data) {
    Data_for_idle *user_data = (Data_for_idle*) data;
    char **temp = mx_strsplit(user_data->decrypted_mess, '|');

    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "chat_list"));
    GList *children = gtk_container_get_children(GTK_CONTAINER(chat_list));
    if (children != NULL) {
        for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
            GtkWidget *child = GTK_WIDGET(iter->data);
            GtkWidget *chat_row = mx_find_widget_by_name(child, temp[1]);
            // GtkWidget *chat_id = mx_find_label_in_box(chat_row, "chat_id");
            // const char *chat_id_s = gtk_label_get_text(GTK_LABEL(chat_id));
            // if (mx_atoi(chat_id_s) == user_data->data->current_chat) {

            //     GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "chat_field"));
            //     // GtkLabel *current_chat_name = GTK_LABEL(gtk_builder_get_object(user_data->data->builder, "current_chat_name"));
            //     GtkWidget *search_entry = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "search_message"));
            //     GtkWidget *search_chat_entry = GTK_WIDGET(gtk_builder_get_object(user_data->data->builder, "search_chat"));

            //     // GList *children_1 = gtk_container_get_children(GTK_CONTAINER(chat_list));

            //     // for (GList *iter_1 = children_1; iter_1 != NULL; iter_1 = g_list_next(iter_1)) {
            //     //     GtkWidget *child_1 = GTK_WIDGET(iter_1->data);
            //     //     GtkWidget *chat_row_1 = mx_find_widget_by_name(child, "1");
            //     //     if (chat_row_1 != NULL) {
            //     //         const char *text_1 = gtk_label_get_text(GTK_LABEL(chat_row_1));
            //     //         gtk_label_set_text(current_chat_name, text_1);
            //     //         gtk_list_box_select_row(GTK_LIST_BOX(chat_list), GTK_LIST_BOX_ROW(child_1));
            //     //         break;
            //     //     }
            //     // }
            //     // g_list_free(children_1);


            //     // clear_widget(chat_field);
            //     // user_data->data->current_chat = 1;
            //     // char *encrypted_mess = mx_encrypting_data("cn|1", user_data->data->server_key);
            //     // send(user_data->data->conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
            //     // gtk_entry_set_text(GTK_ENTRY(search_chat_entry), "");
            //     // gtk_entry_set_text(GTK_ENTRY(search_entry), "");
            //     // free(encrypted_mess);
            // }

            if (chat_row != NULL) {
                gtk_container_remove(GTK_CONTAINER(chat_list), GTK_WIDGET(child));
                break;
            }
        }
    }
    g_list_free(children);

    g_free(user_data->decrypted_mess);
    g_free(user_data);

    return G_SOURCE_REMOVE;
}

void mx_del_chat_w(DataForChat *data, char *chat_data) {
    Data_for_idle *user_data = g_new0(Data_for_idle, 1);
    user_data->data = data;
    user_data->decrypted_mess = g_strdup(chat_data);

    g_idle_add(del_chat_w, user_data);
}

void mx_edit_chat_name(char *chat_name, DataForChat *data) {
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(data->builder, "chat_list"));
    int should_be_selected = data->current_chat - 1;
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_list), should_be_selected);
    GtkWidget *label = mx_find_widget_by_name(GTK_WIDGET(row), "chat_name");
    gtk_label_set_text(GTK_LABEL(label), chat_name);
    const char *text = gtk_label_get_text(GTK_LABEL(label));
    GtkLabel *current_chat_name = GTK_LABEL(gtk_builder_get_object(data->builder, "current_chat_name"));
    gtk_label_set_text(current_chat_name, text);
}

void mx_get_chats(DataForChat *data) {
    char buffer[BUFFER_SIZE] = {0};
    Connection conn = data->conn;
    char *encrypted_mess = mx_encrypting_data("ci", data->server_key);
    send(conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    char *decrypted_mess = NULL;
    free(encrypted_mess);

    while (1) {
        mx_memset(buffer, '\0', BUFFER_SIZE);
        int valread = recv(data->conn.socket, buffer, BUFFER_SIZE, 0);
        if (valread == 0) {
            printf("Server closed connection\n");
            close(conn.socket);
            break;
        }
        decrypted_mess = mx_decrypting_data(buffer, data->client_pair.private_key);
        if (mx_strcmp(decrypted_mess, "end") == 0) break;
        mx_create_chat_w(data, decrypted_mess);
        free(decrypted_mess);
        send(conn.socket, "1", 1, 0);
    }
    free(decrypted_mess);
}

int mx_get_current_chat(DataForChat *data) {
    char buffer[BUFFER_SIZE] = {0};
    char *encrypted_mess = mx_encrypting_data("cc", data->server_key);
    send(data->conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
    free(encrypted_mess);
    read(data->conn.socket, buffer, BUFFER_SIZE);
    char *decrypted_mess = mx_decrypting_data(buffer, data->client_pair.private_key);
    printf("chat: %s\n", decrypted_mess);
    char** temp = mx_strsplit(decrypted_mess, '|');
    int cc = mx_atoi(temp[1]);
    for (int i = 0; temp[i]; i++) free(temp[i]);
    free(decrypted_mess);
    free(temp);
    return cc;
}

void on_chat_row_selected(GtkListBox *listbox, GtkListBoxRow *row, gpointer data_chat) {
    (void)row;
    DataForChat *data = (DataForChat *)data_chat;
    GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(data->builder, "chat_field"));
    GtkLabel *current_chat_name = GTK_LABEL(gtk_builder_get_object(data->builder, "current_chat_name"));
    GtkWidget *search_entry = GTK_WIDGET(gtk_builder_get_object(data->builder, "search_message"));
    GtkWidget *search_chat_entry = GTK_WIDGET(gtk_builder_get_object(data->builder, "search_chat"));
    
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(listbox);

    if (selected_row != NULL) {
        GtkWidget *label = mx_find_widget_by_name(GTK_WIDGET(selected_row), "chat_name");
        const char *text = gtk_label_get_text(GTK_LABEL(label));
        printf("Название чата: %s\n", text);
        gtk_label_set_text(current_chat_name, text);
        GList *children = gtk_container_get_children(GTK_CONTAINER(listbox));
        int row_index = g_list_index(children, selected_row);
        data->row_id = row_index;
        GtkWidget *chat_id_l = mx_find_widget_by_name(GTK_WIDGET(selected_row), "chat_id");
        const char *content_text = gtk_label_get_text(GTK_LABEL(chat_id_l));
        if (data->current_chat != mx_atoi(content_text)) {
            clear_widget(chat_field);
            data->current_chat = mx_atoi(content_text);
            char *rec = mx_strjoin("cn|", content_text);
            char *encrypted_mess = mx_encrypting_data(rec, data->server_key);
            send(data->conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
            gtk_entry_set_text(GTK_ENTRY(search_chat_entry), "");
            gtk_entry_set_text(GTK_ENTRY(search_entry), "");
            free(encrypted_mess);
            free(rec);
        }
    }
}

