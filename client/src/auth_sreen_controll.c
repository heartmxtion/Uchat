#include "../inc/client.h"

int mx_auth(DataForChat *data, char *request) {
    char *buff[BUFFER_SIZE];
    mx_memset(&buff, '\0', BUFFER_SIZE);

    send(data->conn.socket, request, mx_strlen(request), 0);

    int status = read(data->conn.socket, buff, BUFFER_SIZE);

    if (status < 0) return -1;
    return mx_atoi((const char *)buff);
}

void on_continue_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkWidget *window = data->this_window;

    GtkNotebook *notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "main_notebook"));

    GtkEntryBuffer *login_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "login_buffer"));
    const char *login = gtk_entry_buffer_get_text(login_buff);
    GtkEntryBuffer *password_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "password_buffer"));
    const char *password = gtk_entry_buffer_get_text(password_buff);
    GtkWidget *login_error = GTK_WIDGET(gtk_builder_get_object(builder, "login_error_label"));
    GtkWidget *reg_error = GTK_WIDGET(gtk_builder_get_object(builder, "login_error_label1"));
    
    int answer = 0;
    char *request = NULL;
    
    int current_page = gtk_notebook_get_current_page(notebook);
    if (current_page == 0) {
        request = mx_log_and_pass_to_string("l", login, password);
        char *encrypted_mess = mx_encrypting_data(request, data->server_key);
        answer = mx_auth(data, encrypted_mess);
        free(request);
        free(encrypted_mess);
        if (answer == SUCCESS) {
            gtk_widget_hide(window);
            create_chat(data_chat);
        }
        else if (answer == ERROR) gtk_label_set_text(GTK_LABEL(login_error), "Wrong login or password");
    } 
    else if (current_page == 1) {
        GtkEntryBuffer *password_buff_r = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "r_password_buffer"));
        const char *password_r = gtk_entry_buffer_get_text(password_buff_r);
        if (mx_strcmp(password, password_r) == 0) {
            if (strlen(login) >= 4) {
                if (strlen(password) >= 8) {
                    //добавить сюда проверку пароля на простоту
                    request = mx_log_and_pass_to_string("r", login, password);
                    char *encrypted_mess = mx_encrypting_data(request, data->server_key);
                    answer = mx_auth(data, encrypted_mess);
                    free(encrypted_mess);
                    free(request);
                    if (answer == SUCCESS) {
                        gtk_widget_hide(window);
                        create_chat(data_chat);
                    }
                    else if (answer == ALREADY) gtk_label_set_text(GTK_LABEL(reg_error), "This login is already taken");
                } else gtk_label_set_text(GTK_LABEL(reg_error), "Password is too short(<8)");
             } else gtk_label_set_text(GTK_LABEL(reg_error), "Login is too short(<4)");
        } 
        else gtk_label_set_text(GTK_LABEL(reg_error), "Password mismatch");
    }
}

