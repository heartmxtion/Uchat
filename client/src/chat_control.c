#include "../inc/client.h"

int mx_strlen_until_char(const char *str, char delim) {
    int i = 0;
    while (str[i] && str[i] != delim) i++;
    return i;
}

void on_message_row_selected(GtkWidget *w, GdkEventButton *event, gpointer data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkListBox *listbox = GTK_LIST_BOX(w);
    GtkWidget *context_menu = GTK_WIDGET(gtk_builder_get_object(builder, "context_menu"));
    
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        GtkListBoxRow *selected_row = gtk_list_box_get_row_at_y(listbox, event->y);
        GtkWidget *content_label = mx_find_label_in_box(GTK_WIDGET(selected_row), "comms");

        if (content_label != NULL) {
            const char *content_text = gtk_label_get_text(GTK_LABEL(content_label));
            
            if (selected_row != NULL && !g_strstr_len(content_text, -1, "*message deleted*")) {
                int row_index = gtk_list_box_row_get_index(selected_row);
                data->row_id = row_index;
                printf("Right clicked on row %d\n", row_index);
                gtk_widget_show_all(context_menu);
                gtk_menu_popup_at_pointer(GTK_MENU(context_menu), (GdkEvent *)event);
            }
        }
    }
}

void on_scroll_changed(GtkWidget *w, gpointer data_chat) {
    (void)w;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkWidget *arrow = GTK_WIDGET(gtk_builder_get_object(builder, "arrow_btn"));
    GtkWidget *scroll = GTK_WIDGET(gtk_builder_get_object(builder, "scrlldwnd_found_msgs"));
    
    
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    double value = gtk_adjustment_get_value(adjustment);
    double upper = gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment);

    if (value >= upper) {
        gtk_widget_hide(arrow);
    } else {
        gtk_widget_show_all(arrow);
    }

}

char *mx_time_to_local(char *time_m) {
    time_t t = time(NULL);
    struct tm *local_time = localtime(&t);
    int offset_hours = local_time->tm_hour - gmtime(&t)->tm_hour;
    char *new_time = mx_strdup(time_m);
    char *s = mx_strndup(time_m, 2);
    int time_i = mx_atoi(s);
    free(s);
    
    time_i = time_i + offset_hours;
    if (time_i >= 24) time_i -= 24;
    else if (time_i < 0) time_i += 24;

    s = mx_itoa(time_i);
    if (time_i < 10) {
        new_time[0] = '0';
        new_time[1] = s[0];
    } else {
        new_time[0] = s[0];
        new_time[1] = s[1];
    }
    free(s);
    return new_time;
}

// void mx_local_time_transfer(char **time_list, char *time_str) {
//     time_t local_time = time(NULL);
//     struct tm *local_tm = localtime(&local_time);

//     struct tm message_tm = {0};
//     sscanf(time_list[1], "%d:%d:%d", &message_tm.tm_hour, &message_tm.tm_min, &message_tm.tm_sec);
//     message_tm.tm_year = local_tm->tm_year;
//     message_tm.tm_mon = local_tm->tm_mon;
//     message_tm.tm_mday = local_tm->tm_mday;
//     time_t message_time = mktime(&message_tm);
//     time_t gmt_offset = local_tm->tm_gmtoff;
 
//     time_t final_time = message_time + gmt_offset;

//     if (local_tm->tm_isdst > 0 && gmtime(&final_time)->tm_isdst == 0) {
//         final_time -= 3600;
//     } else if (local_tm->tm_isdst == 0 && gmtime(&final_time)->tm_isdst > 0) {
//         final_time += 3600;
//     }
    
//     strftime(time_str, sizeof(time_str), "%H:%M", localtime(&final_time));
// }

char* rsaKeyToString(RSA* rsaKey) {
    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio) {
        return NULL;
    }

    if (!PEM_write_bio_RSAPublicKey(bio, rsaKey)) {
        BIO_free(bio);
        return NULL;
    }

    char* keyString = NULL;
    long keyLength = BIO_get_mem_data(bio, &keyString);
    char* result = malloc(keyLength + 1);
    if (!result) {
        BIO_free(bio);
        return NULL;
    }

    memcpy(result, keyString, keyLength);
    result[keyLength] = '\0';
    BIO_free(bio);

    return result;
}

void set_widgets_sensitive(GtkBuilder *builder, gboolean sensitive) {
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(builder, "chat_list"));
    GtkWidget *chat_send = GTK_WIDGET(gtk_builder_get_object(builder, "send_"));
    GtkWidget *menuitem_edit = GTK_WIDGET(gtk_builder_get_object(builder, "edit_but"));
    GtkWidget *menuitem_del = GTK_WIDGET(gtk_builder_get_object(builder, "del_but"));
    GtkWidget *menuitem_reply = GTK_WIDGET(gtk_builder_get_object(builder, "tag_but"));
    GtkWidget *chat_settings_btn = GTK_WIDGET(gtk_builder_get_object(builder, "chat_settings_btn"));
    GtkWidget *add_chat = GTK_WIDGET(gtk_builder_get_object(builder, "add_chat"));

    gtk_widget_set_sensitive(chat_list, sensitive);
    gtk_widget_set_sensitive(chat_send, sensitive);
    gtk_widget_set_sensitive(menuitem_edit, sensitive);
    gtk_widget_set_sensitive(menuitem_del, sensitive);
    gtk_widget_set_sensitive(menuitem_reply, sensitive);
    gtk_widget_set_sensitive(chat_settings_btn, sensitive);
    gtk_widget_set_sensitive(add_chat, sensitive);
}

void reconnect(DataForChat *data) {//.h
    int answer = 0;
    char *request = NULL;
    int reconn_try = 1;
    data->is_reconnecting = true;

    GtkBuilder *builder = data->builder;
    //GtkWidget *chat_window = GTK_WIDGET(gtk_builder_get_object(builder, "chat_screen"));
    set_widgets_sensitive(builder, FALSE);

    printf("Connection lost. Trying reconnect.\n");
    close(data->conn.socket);
    data->conn = mx_init_socket(8080, "127.0.0.1");

    GtkWidget *label = GTK_WIDGET(gtk_builder_get_object(builder, "server_status"));
    gtk_label_set_text(GTK_LABEL(label), "Server status: Not responding...");
    while(connect(data->conn.socket, (struct sockaddr *)&(data->conn.serv_addr), sizeof(data->conn.serv_addr)) < 0) {
        printf("%d try failed\n", reconn_try);
        reconn_try++;
        sleep(3);
    }
    gtk_label_set_text(GTK_LABEL(label), "Server status: Active");
    printf("\nReconnected\n");

    GtkEntryBuffer *login_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "login_buffer"));
    const char *login = gtk_entry_buffer_get_text(login_buff);
    GtkEntryBuffer *password_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "password_buffer"));
    const char *password = gtk_entry_buffer_get_text(password_buff);

    data->server_key = mx_key_exchange(data->client_pair.public_key, data->conn.socket);
    request = mx_log_and_pass_to_string("l", login, password);
    char *encrypted_mess = mx_encrypting_data(request, data->server_key);
    answer = mx_auth(data, encrypted_mess);
    free(request);
    free(encrypted_mess);
    set_widgets_sensitive(builder, TRUE);

    if (answer == SUCCESS) {
        GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(builder, "chat_field"));
        GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(builder, "chat_list"));
        clear_widget(chat_field); // очищаем дочерние виджеты
        clear_widget(chat_list);
        memset(&data->thread_chat_listening, 0, sizeof(pthread_t));
        create_chat(data);
    }
}

void *thread_chat_listening(void* data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    char buffer[BUFFER_SIZE] = {0};
    GtkEntryBuffer *login_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "login_buffer"));
    int status;
    data->current_chat = mx_get_current_chat(data);
    data->new_users = (int *)malloc(sizeof(int) * 1);
    data->new_users[0] = -1;
    data->ban_users = (int *)malloc(sizeof(int) * 1);
    data->ban_users[0] = -1;
    mx_get_chats(data);
    mx_get_old_messages(data, data->current_chat);
    char *enc_mess = mx_encrypting_data("pe", data->server_key);
    const char *login = gtk_entry_buffer_get_text(login_buff);
    char *str = mx_strjoin("@", login); 
    send(data->conn.socket, enc_mess, mx_strlen(enc_mess), 0);
    if (enc_mess != NULL) free(enc_mess);
    printf("current chat: %i\n", data->current_chat);
    
    while (1) {
        mx_memset(buffer, '\0', BUFFER_SIZE);
        printf("wait for new message\n");
        status = read(data->conn.socket, buffer, BUFFER_SIZE);
        printf("size: %i\n", status);
        if (status == 0) {
            reconnect(data);
            break;
        }
        printf("start decrypting\n");
        // printf("encrypted mess: %s\n", buffer);
        char *decrypted_mess = mx_decrypting_data(buffer, data->client_pair.private_key);
        printf("decrypted_mess: %s\n", decrypted_mess);
        char **temp = mx_strsplit(decrypted_mess, '|');

        if (mx_strcmp(temp[0], "ms") == 0) {
            if (mx_atoi(temp[1]) == data->current_chat) {
                mx_create_message(data, decrypted_mess);
            }
            else {
                char *found = mx_strstr(decrypted_mess, str);
                if(found != NULL){
                    show_notification("Someone tagget you!", temp[5]);
                } else{
                    show_notification("New message", "You have a new message in uchat!");
                }
                mx_add_last_message(data, decrypted_mess);
            }
        }
        if (mx_strcmp(temp[0], "me") == 0) {
            if (mx_atoi(temp[1]) == data->current_chat) {
                mx_edit_message(data, decrypted_mess);
                mx_add_last_message(data, decrypted_mess);
            }
        }
        if (mx_strcmp(temp[0], "cc") == 0) data->current_chat = mx_atoi(temp[1]);
        if (mx_strcmp(temp[0], "om") == 0) mx_read_messages(data);
        if (mx_strcmp(temp[0], "ui") == 0) mx_show_users(data, decrypted_mess);
        if (mx_strcmp(temp[0], "lm") == 0) mx_add_last_message(data, decrypted_mess);
        if (mx_strcmp(temp[0], "ci") == 0) mx_create_chat_w(data, decrypted_mess);
        if (mx_strcmp(temp[0], "uic") == 0) mx_add_user_in_list(data, decrypted_mess);
        if (mx_strcmp(temp[0], "ce") == 0) mx_edit_chat_w(data, decrypted_mess);
        if (mx_strcmp(temp[0], "cd") == 0) mx_del_chat_w(data, decrypted_mess);

        GtkWidget *arrow = GTK_WIDGET(gtk_builder_get_object(data->builder, "arrow_btn"));
        GtkWidget *scroll = GTK_WIDGET(gtk_builder_get_object(data->builder, "scrlldwnd_found_msgs"));

        GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
        double value = gtk_adjustment_get_value(adjustment);
        double upper = gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment);

        if (value >= upper) gtk_widget_hide(arrow);
        else gtk_widget_show_all(arrow);

        for (int i = 0; temp[i] != NULL; i++) free(temp[i]);
        free(decrypted_mess);
        free(temp);
        printf("sending 1\n");
        send(data->conn.socket, "1", 1, 0);
    }
    pthread_exit(NULL);
}

int create_chat(gpointer data_chat) {
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *ui_builder = data->builder;

    //Connection *conn = &data->conn;
    GtkWidget *chat_window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_screen"));
    

//pthread_t thread = data->thread_chat_listening;
//if(data->is_reconnecting == true){
//        if (pthread_kill(thread, 0) == 0) {
            // thread is still running, join it to clean up resources
//            int result = pthread_join(thread, NULL);
//            if (result != 0) {
//                perror("Could not join thread.");
//                return EXIT_FAILURE;
//            }
            // detach the thread to avoid resource leaks
//            result = pthread_detach(thread);
//            if (result != 0) {
//                perror("Could not detach thread.");
//                return EXIT_FAILURE;
//            }
//        }
//}

    pthread_t thread; //comment it if u know how to realise thing before
    // create a new thread
    int result = pthread_create(&thread, NULL, thread_chat_listening, data_chat);
    if (result != 0) {
        perror("Could not create thread.");
        return EXIT_FAILURE;
    }
    data->is_reconnecting = false;
    
    gtk_widget_show_all(chat_window);
    return EXIT_SUCCESS;
}


