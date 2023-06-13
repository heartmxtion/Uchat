#include <stdio.h>
#include <pthread.h>
#include "../inc/client.h"

bool check_already_open(char s[]) {
    // Получаем список всех верхнеуровневых окон
    GList* toplevels = gtk_window_list_toplevels();

    // Проходим по списку окон
    for (GList* l = toplevels; l != NULL; l = l->next) {
        GtkWidget* window = GTK_WIDGET(l->data);
    
        // Проверяем название окна
        const gchar* title = gtk_window_get_title(GTK_WINDOW(window));
        if (g_strcmp0(title, s) == 0) {
            // Окно уже открыто, делаем его активным
            gtk_window_present(GTK_WINDOW(window));
            // Выходим из цикла, так как дальнейшие окна уже не нужны
            return true;
        }
    }

    // Освобождаем список окон
    g_list_free(toplevels);
    return false;
}

void css_loader(DataForChat *data_chat){
    if (data_chat == NULL || data_chat->builder == NULL) {
        fprintf(stderr, "Error: Invalid data_chat or builder pointer\n");
        return;
    }
    
    GtkSettings *settings = gtk_settings_get_default();
    if (settings == NULL) {
        fprintf(stderr, "Error: Could not get default GtkSettings object\n");
        return;
    }
    g_object_set(settings, "gtk-theme-name", "Adwaita-dark", NULL);

    // Получаем GtkBuilder и создаем локальные переменные для элементов интерфейса
    GtkBuilder *ui_builder = data_chat->builder;
    if (ui_builder == NULL) {
        fprintf(stderr, "Error: Invalid GtkBuilder pointer\n");
        return;
    }


    GtkWidget *main_notebook = GTK_WIDGET(gtk_builder_get_object(ui_builder, "main_notebook"));
    GtkWidget *back_of_notebook = GTK_WIDGET(gtk_builder_get_object(ui_builder, "back_of_notebook"));
    GtkWidget *back_of_notebook_2 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "back_of_notebook_2"));
    GtkWidget *auth_iternal = GTK_WIDGET(gtk_builder_get_object(ui_builder, "auth_iternal"));
    GtkWidget *box_chat_header = GTK_WIDGET(gtk_builder_get_object(ui_builder, "box_chat_header"));
    GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_field"));
    GtkWidget *chat_window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_screen"));
    GtkWidget *side_menu_header = GTK_WIDGET(gtk_builder_get_object(ui_builder, "side_menu_header"));
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_list"));
    GtkWidget *add_chat = GTK_WIDGET(gtk_builder_get_object(ui_builder, "create_chat"));
    GtkWidget *chat_settings_box = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_settings_box"));
    
    if (!main_notebook || !back_of_notebook || !back_of_notebook_2 || !auth_iternal ||
        !box_chat_header || !chat_field || !chat_window || !side_menu_header || !chat_list) {
        g_warning("Unable to retrieve UI elements in css_loader function");
        return;
    }
    
    // Установка стилей
    const gchar *css_data =
        ".main_notebook, .auth_iternal {\n"
        "    background-color: #171139;\n" // Цвет фона для кнопок переключения меню и нижней части меню регистрации/авторизации
        "    color: white;\n" // Цвет текста
        "}\n"
        ".back_of_notebook, .back_of_notebook_2 {\n"
        "    background-color: #19144a;\n" // Цвет фона для форм регистрации/авторизации
        "    color: white;\n"
        "}\n"
        ".chat_field {\n"
        "    background-color: #19144a;\n" // Цвет фона для списка сообщений
        "    color: white;\n"
        "}\n"
        ".box_chat_header {\n"
        "    background-color: #171139;\n" // Цвет фона для заголовка чата
        "    color: white;\n"
        "}\n"
        ".chat_screen {\n"
        "    background-color: #171139;\n" //Цвет фона
        "    color: white;\n" //Цвет
        "}\n"
        ".chat_list {\n"
        "    background-color: #19144a;\n" //Цвет фона строки во время выделения
        "    color: white;\n" //Цвет логина и сообщений
        "}\n"
        ".side_menu_header {\n"
        "    background-color: #171139;\n" //Цвет фона
        "    color: white;\n" //Цвет названия чата
        "}\n"
        ".create_chat {\n"
        "    background-color: #171139;\n" //Цвет фона
        "    color: white;\n" //Цвет
        "}\n"
        ".chat_settings {\n"
        "    background-color: #171139;\n" //Цвет фона
        "    color: white;\n" //Цвет
        "}\n";


    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    
    GError *error = NULL;
    if (!gtk_css_provider_load_from_data(provider, css_data, -1, &error)) {
        g_print("Ошибка: Не удалось загрузить CSS: %s\n", error->message);
        g_error_free(error);
        g_object_unref(provider);
        return;
    }

    GtkStyleContext *context = gtk_widget_get_style_context(main_notebook);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(back_of_notebook);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(back_of_notebook_2);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(auth_iternal);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(chat_field);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(box_chat_header);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(chat_window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(chat_list);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(side_menu_header);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(add_chat);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    context = gtk_widget_get_style_context(chat_settings_box);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

RSA *mx_key_exchange(RSA *client_public_key, int socket) {
    // Создаем BIO для хранения ключа клиента
    BIO *client_key_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(client_key_bio, client_public_key);
    // Получаем размер ключа клиента
    int client_key_len = BIO_pending(client_key_bio);
    // Выделяем достаточно памяти для хранения строки с ключом
    char *client_key_str = malloc(client_key_len + 1);
    // Читаем ключ из BIO и записываем его в строку
    BIO_read(client_key_bio, client_key_str, client_key_len);
    // Добавляем символ конца строки в конец
    client_key_str[client_key_len] = '\0';
    // Создаем строку с префиксом "ke|"
    char* client_keystr_with_prefix = malloc(client_key_len + 4);
    sprintf(client_keystr_with_prefix, "ke|%s", client_key_str);

    // Отправляем ключ на сервер
    int send_result = send(socket, client_keystr_with_prefix, client_key_len + 3, 0);
    if (send_result == -1) {
        fprintf(stderr, "Error sending data to server\n");
        exit(1);
    }

    // Освобождаем память, занятую строкой с ключом и строкой с префиксом
    free(client_key_str);
    free(client_keystr_with_prefix);

    // Создаем буфер для ответа от сервера
    char server_buffer[client_key_len + 1];
    // Получаем ответ от сервера
    int bytes_received = recv(socket, server_buffer, client_key_len, 0);
    if (bytes_received <= 0) {
        fprintf(stderr, "Error receiving data from server\n");
        exit(1);
    }
    server_buffer[bytes_received] = '\0';

    // Создаем BIO для хранения ключа сервера
    BIO* server_key_bio = BIO_new(BIO_s_mem());
    // Записываем полученный ответ в BIO
    BIO_write(server_key_bio, server_buffer, bytes_received);
    // Читаем ключ из BIO
    RSA* server_key = PEM_read_bio_RSAPublicKey(server_key_bio, NULL, NULL, NULL);
    if (server_key == NULL) {
        fprintf(stderr, "Error: unable to load RSA key\n");
        exit(1);
    }

    // Освобождаем память, занятую BIO
    BIO_free(server_key_bio);
    BIO_free(client_key_bio);

    return server_key;
}
// Функция для генерации RSA-пары ключей и сохранения их в структуру KeyPair
KeyPair mx_generate_key_pair() {
    KeyPair key_pair;
    key_pair.private_key = RSA_new();
    BIGNUM *bne = BN_new();
    int bits = 2048;
    unsigned long e = RSA_F4;
    BN_set_word(bne, e);

    RSA_generate_key_ex(key_pair.private_key, bits, bne, NULL);
    key_pair.public_key = RSAPublicKey_dup(key_pair.private_key);

    BN_free(bne);

    return key_pair;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./uchat ip port");
        exit(0);
    }

    Connection conn = mx_init_socket(mx_atoi(argv[1]), argv[2]);
    if (connect(conn.socket, (struct sockaddr *)&conn.serv_addr, sizeof(conn.serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    KeyPair client_key_pair = mx_generate_key_pair();
    RSA* server_key = mx_key_exchange(client_key_pair.public_key, conn.socket);

    gtk_init(&argc, &argv);

    GtkBuilder * ui_builder;
    GError * err = NULL;

    ui_builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(ui_builder, "client/resources/gui_reborn.glade", &err)) {
        g_critical ("Ошибка загрузки файла с UI: %s", err->message);
        g_error_free (err);
    }

    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "auth_screen"));
    
    GtkWidget *continue_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "continue_btn"));
    GtkWidget *chat_settings_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_settings_btn"));
    GtkWidget *back_settings_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "back_settings_btn"));
    GtkWidget *apply_settings_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "apply_settings_btn"));
    GtkWidget *chat_send_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "send_"));
    GtkWidget *add_chat_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "add_chat"));
    GtkWidget *cancel_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "cancel_btn"));
    GtkWidget *create_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "create_btn"));
    GtkWidget *send_emoji = GTK_WIDGET(gtk_builder_get_object(ui_builder, "send_emoji"));
    
    GtkWidget *scroll = GTK_WIDGET(gtk_builder_get_object(ui_builder, "scrlldwnd_found_msgs"));
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));
    //GtkWidget *overlay = GTK_WIDGET(gtk_builder_get_object(ui_builder, "overlay"));
    GtkWidget *arrow_btn = GTK_WIDGET(gtk_builder_get_object(ui_builder, "arrow_btn"));
    GtkWidget *chat_window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_screen"));
    GtkWidget *chat_field = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_field"));
    GtkWidget *menuitem_tag = GTK_WIDGET(gtk_builder_get_object(ui_builder, "tag_but"));
    GtkWidget *menuitem_edit = GTK_WIDGET(gtk_builder_get_object(ui_builder, "edit_but"));
    GtkWidget *menuitem_del = GTK_WIDGET(gtk_builder_get_object(ui_builder, "del_but"));
    GtkWidget *search_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "search_message"));
    GtkWidget *chat_list = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_list"));
    GtkWidget *chart_create_search_user = GTK_WIDGET(gtk_builder_get_object(ui_builder, "user_searchss"));
    GtkWidget *search_chat_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "search_chat"));
    GtkWidget *member_name = GTK_WIDGET(gtk_builder_get_object(ui_builder, "member_name"));
    GtkWidget *listbox_sett_members = GTK_WIDGET(gtk_builder_get_object(ui_builder, "listbox_sett_members"));
    GtkWidget *user_container = GTK_WIDGET(gtk_builder_get_object(ui_builder, "user_container"));
    GtkWidget *del_user_but = GTK_WIDGET(gtk_builder_get_object(ui_builder, "del_user_but"));
    gtk_widget_set_halign(arrow_btn, GTK_ALIGN_END);
    gtk_widget_set_valign(arrow_btn, GTK_ALIGN_END);

    //Вызов виджетов для эмоджи
    GtkWidget *emoji_1 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_1"));
    GtkWidget *emoji_2 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_2"));
    GtkWidget *emoji_3 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_3"));
    GtkWidget *emoji_4 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_4"));
    GtkWidget *emoji_5 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_5"));
    GtkWidget *emoji_6 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_6"));
    GtkWidget *emoji_7 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_7"));
    GtkWidget *emoji_8 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_8"));
    GtkWidget *emoji_9 = GTK_WIDGET(gtk_builder_get_object(ui_builder, "emoji_9"));
    
    DataForChat* data_chat = g_new(DataForChat, 1);
    data_chat->builder = ui_builder;
    data_chat->this_window = window;
    data_chat->conn = conn;
    data_chat->server_key = server_key;
    data_chat->client_pair = client_key_pair;
    
    css_loader(data_chat);
      
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_closed), window);
    g_signal_connect(continue_btn, "clicked", G_CALLBACK(on_continue_clicked), data_chat);
    g_signal_connect(chat_settings_btn, "clicked", G_CALLBACK(on_settings_clicked), data_chat);
    g_signal_connect(back_settings_btn, "clicked", G_CALLBACK(on_back_settings_clicked), data_chat);
    g_signal_connect(apply_settings_btn, "clicked", G_CALLBACK(on_apply_settings_clicked), data_chat);
    g_signal_connect(chat_send_btn, "clicked", G_CALLBACK(on_chat_send_clicked), data_chat);
    g_signal_connect(add_chat_btn, "clicked", G_CALLBACK(on_add_chat_clicked), data_chat);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel_clicked), data_chat);
    g_signal_connect(create_btn, "clicked", G_CALLBACK(on_create_chat_clicked), data_chat);
    g_signal_connect(send_emoji, "clicked", G_CALLBACK(on_emoji_send_clicked), data_chat);
    
    
    //Обработка сигналов для эмоджи
    g_signal_connect(emoji_1, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_2, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_3, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_4, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_5, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_6, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_7, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_8, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    g_signal_connect(emoji_9, "clicked", G_CALLBACK(on_emoji_clicked), data_chat);
    
    
    g_signal_connect(chat_window, "destroy", G_CALLBACK(on_window_closed), chat_window);
    g_signal_connect(chat_field, "button-press-event", G_CALLBACK(on_message_row_selected), data_chat);
    g_signal_connect(menuitem_tag, "activate", G_CALLBACK(on_menuitem_tag_activate), data_chat);
    g_signal_connect(menuitem_edit, "activate", G_CALLBACK(on_menuitem_edit_activate), data_chat);
    g_signal_connect(menuitem_del, "activate", G_CALLBACK(on_menuitem_del_activate), data_chat);
    g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_changed), chat_field);
    g_signal_connect(chat_list, "row-selected", G_CALLBACK(on_chat_row_selected), data_chat);
    g_signal_connect(chart_create_search_user, "search-changed", G_CALLBACK(on_chat_create_user_search_changed), data_chat);
    g_signal_connect(search_chat_entry, "search-changed", G_CALLBACK(on_search_chats_changed), chat_list);
    g_signal_connect(member_name, "search-changed", G_CALLBACK(on_search_member_name_changed), data_chat);
    g_signal_connect(adjustment, "value-changed", G_CALLBACK(on_scroll_changed), data_chat);
    g_signal_connect(arrow_btn, "clicked", G_CALLBACK(on_arrow_clicked), data_chat);
    g_signal_connect(listbox_sett_members, "button-press-event", G_CALLBACK(on_user_row_selected), data_chat);
    g_signal_connect(user_container, "button-press-event", G_CALLBACK(on_user_row_selected), data_chat);
    g_signal_connect(del_user_but, "activate", G_CALLBACK(del_user_butt_activate), data_chat);

    gtk_builder_connect_signals(ui_builder, NULL);


    gtk_widget_show_all(window);
    gtk_main();

    RSA_free(server_key);
    RSA_free(client_key_pair.private_key);
    RSA_free(client_key_pair.public_key);
    server_key = NULL;
    client_key_pair.private_key = NULL;
    client_key_pair.public_key = NULL;
    
    close(conn.socket);
    g_object_unref(data_chat->builder);
    g_free(data_chat);
    return EXIT_SUCCESS;
}

