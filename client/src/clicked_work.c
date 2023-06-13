#include <stdio.h>
#include <pthread.h>
#include "../inc/client.h"

void on_chat_send_clicked(GtkButton *b, gpointer data_chat) {
    (void)b;
    DataForChat *data = (DataForChat *)data_chat;
    GtkBuilder *builder = data->builder;
    GtkTextIter start, end;
    GtkWidget *text_view = GTK_WIDGET(gtk_builder_get_object(builder, "message_field"));
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);

    char *text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
    gtk_text_buffer_set_text(text_buffer, "", -1);
    char *p1;
    char *p2;
    char *message_recv = NULL;
    char *encrypted_mess = NULL;

    if (g_strcmp0(text, "") != 0 ) {
        if ((p1 = mx_strstr(text, "/edit(")) != 0 && (p2 = mx_strchr(text, ')')) != 0) {
            p1 += 6;
            char *p = mx_strndup(p1, (int)(p2 - p1));
            message_recv = mx_ljoin(p, "|");
            message_recv = mx_ljoin(message_recv, (p2 + 2));
            message_recv = mx_rjoin("me|", message_recv);
        }
        else {
            message_recv = mx_strjoin("mc|", text);
        }
        encrypted_mess = mx_encrypting_data(message_recv, data->server_key);
        send(data->conn.socket, encrypted_mess, mx_strlen(encrypted_mess), 0);
        free(message_recv);
        free(encrypted_mess);
        g_free(text);

    }
}

void on_emoji_clicked(GtkButton *button, DataForChat* data_chat) {
    GtkBuilder *builder = data_chat->builder;
    GtkWidget *emoji_selector = GTK_WIDGET(gtk_builder_get_object(builder, "emoji_selector"));
    GtkTextView *message_field = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "message_field"));
    gtk_widget_hide(emoji_selector);

    const char *emoji_id = gtk_button_get_label(button);

    GtkTextIter end_iter;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(message_field);
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
    if (gtk_text_iter_get_offset(&end_iter) > 0) {
        gtk_text_buffer_insert(text_buffer, &end_iter, " ", 1);
    }
    gtk_text_buffer_insert(text_buffer, &end_iter, emoji_id, -1);
}


void on_emoji_send_clicked(GtkButton *button, DataForChat *data_chat) {
    GtkBuilder *builder = data_chat->builder;
    GtkWidget *emoji_selector = GTK_WIDGET(gtk_builder_get_object(builder, "emoji_selector"));

    if (gtk_widget_get_visible(emoji_selector)) {
        gtk_widget_hide(emoji_selector);
    } else {
        gtk_popover_set_relative_to(GTK_POPOVER(emoji_selector), GTK_WIDGET(button));
        gtk_widget_show_all(emoji_selector);
    }
}

void on_arrow_clicked(GtkButton *arrow, DataForChat *data_chat) {
    (void)arrow;
    GtkBuilder *builder = data_chat->builder;
    // прокрутка
    GtkWidget *scrolled_window = GTK_WIDGET(gtk_builder_get_object(builder, "scrlldwnd_found_msgs"));
    GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    if (vadj != NULL) {
        // Получаем максимальное значение для вертикальной прокрутки
        double max_value = gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj);
        // Устанавливаем позицию прокрутки в самый низ
        gtk_adjustment_set_value(vadj, max_value);
    }

}
