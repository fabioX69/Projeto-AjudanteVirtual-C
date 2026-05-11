#include "../include/interface.h"

typedef struct {
    GtkWidget *area_conversa;
    GtkWidget *entrada_mensagem;
} InterfaceDados;

static void adicionar_texto(GtkWidget *area_conversa, const char *texto) {
    GtkTextBuffer *buffer;
    GtkTextIter fim;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(area_conversa));

    gtk_text_buffer_get_end_iter(buffer, &fim);
    gtk_text_buffer_insert(buffer, &fim, texto, -1);

    gtk_text_buffer_get_end_iter(buffer, &fim);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(area_conversa), &fim, 0.0, FALSE, 0.0, 0.0);
}

static void enviar_mensagem(GtkWidget *widget, gpointer dados) {
    InterfaceDados *interface = (InterfaceDados *) dados;

    const char *mensagem = gtk_entry_get_text(GTK_ENTRY(interface->entrada_mensagem));

    if (mensagem == NULL || mensagem[0] == '\0') {
        return;
    }

    adicionar_texto(interface->area_conversa, "Você: ");
    adicionar_texto(interface->area_conversa, mensagem);
    adicionar_texto(interface->area_conversa, "\n");

    adicionar_texto(interface->area_conversa, "Assistente: ");
    adicionar_texto(interface->area_conversa, "Mensagem recebida.\n\n");

    gtk_entry_set_text(GTK_ENTRY(interface->entrada_mensagem), "");
}

void iniciar_interface(int argc, char *argv[]) {
    GtkWidget *janela;
    GtkWidget *caixa_principal;
    GtkWidget *cabecalho;
    GtkWidget *area_conversa;
    GtkWidget *caixa_inferior;
    GtkWidget *entrada_mensagem;
    GtkWidget *botao_enviar;

    InterfaceDados *interface;

    gtk_init(&argc, &argv);

    interface = g_malloc(sizeof(InterfaceDados));

    janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Projeto Assistente Virtual");
    gtk_window_set_default_size(GTK_WINDOW(janela), 750, 500);
    gtk_container_set_border_width(GTK_CONTAINER(janela), 10);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    caixa_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(janela), caixa_principal);

    cabecalho = gtk_label_new(
        "==================================================\n"
        "        PROJETO ASSISTENTE VIRTUAL\n"
        "=================================================="
    );
    gtk_widget_set_name(cabecalho, "cabecalho");
    gtk_box_pack_start(GTK_BOX(caixa_principal), cabecalho, FALSE, FALSE, 0);

    area_conversa = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(area_conversa), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(area_conversa), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(area_conversa), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_name(area_conversa, "area-conversa");

    gtk_box_pack_start(GTK_BOX(caixa_principal), area_conversa, TRUE, TRUE, 0);

    caixa_inferior = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(caixa_principal), caixa_inferior, FALSE, FALSE, 0);

    entrada_mensagem = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrada_mensagem), "Digite sua mensagem...");
    gtk_widget_set_name(entrada_mensagem, "entrada-mensagem");

    gtk_box_pack_start(GTK_BOX(caixa_inferior), entrada_mensagem, TRUE, TRUE, 0);

    botao_enviar = gtk_button_new_with_label("Enviar");
    gtk_widget_set_name(botao_enviar, "botao-enviar");

    gtk_box_pack_start(GTK_BOX(caixa_inferior), botao_enviar, FALSE, FALSE, 0);

    interface->area_conversa = area_conversa;
    interface->entrada_mensagem = entrada_mensagem;

    g_signal_connect(botao_enviar, "clicked", G_CALLBACK(enviar_mensagem), interface);
    g_signal_connect(entrada_mensagem, "activate", G_CALLBACK(enviar_mensagem), interface);

    GtkCssProvider *css = gtk_css_provider_new();

    gtk_css_provider_load_from_data(css,
        "window {"
        "   background-color: #000000;"
        "}"
        "#cabecalho {"
        "   background-color: #000000;"
        "   color: #00ff66;"
        "   font-family: Consolas, monospace;"
        "   font-size: 15px;"
        "   padding: 8px;"
        "}"
        "#area-conversa {"
        "   background-color: #000000;"
        "   color: #00ff66;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "   border: 1px solid #00ff66;"
        "}"
        "#entrada-mensagem {"
        "   background-color: #111111;"
        "   color: #ffffff;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   padding: 8px;"
        "   border: 1px solid #00ff66;"
        "}"
        "#botao-enviar {"
        "   background-color: #111111;"
        "   color: #00ff66;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   padding: 8px;"
        "   border: 1px solid #00ff66;"
        "}",
        -1,
        NULL
    );

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    adicionar_texto(area_conversa,
        "Assistente: Olá! Eu sou o assistente virtual do projeto.\n"
        "Assistente: Digite uma mensagem abaixo para começar.\n\n"
    );

    gtk_widget_show_all(janela);

    gtk_main();

    g_free(interface);
}
