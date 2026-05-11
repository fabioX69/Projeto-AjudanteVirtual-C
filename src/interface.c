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
    gtk_text_view_scroll_to_iter(
        GTK_TEXT_VIEW(area_conversa),
        &fim,
        0.0,
        FALSE,
        0.0,
        0.0
    );
}

static void enviar_mensagem(GtkWidget *widget, gpointer dados) {
    InterfaceDados *interface = (InterfaceDados *) dados;

    const char *mensagem = gtk_entry_get_text(GTK_ENTRY(interface->entrada_mensagem));

    if (mensagem == NULL || mensagem[0] == '\0') {
        return;
    }

    adicionar_texto(interface->area_conversa, "C:\\Usuario\\Fabio> ");
    adicionar_texto(interface->area_conversa, mensagem);
    adicionar_texto(interface->area_conversa, "\n");

    adicionar_texto(interface->area_conversa, "Assistente> ");
    adicionar_texto(interface->area_conversa, "Entrada recebida. Modulo de IA ainda nao conectado.\n\n");

    gtk_entry_set_text(GTK_ENTRY(interface->entrada_mensagem), "");
}

void iniciar_interface(int argc, char *argv[]) {
    GtkWidget *janela;
    GtkWidget *caixa_principal;
    GtkWidget *cabecalho;
    GtkWidget *area_conversa;
    GtkWidget *rolagem;
    GtkWidget *caixa_inferior;
    GtkWidget *entrada_mensagem;
    GtkWidget *botao_enviar;

    InterfaceDados *interface;

    gtk_init(&argc, &argv);

    interface = g_malloc(sizeof(InterfaceDados));

    janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Projeto Assistente Virtual");
    gtk_window_set_default_size(GTK_WINDOW(janela), 850, 550);
    gtk_container_set_border_width(GTK_CONTAINER(janela), 8);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    caixa_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_name(caixa_principal, "caixa-principal");
    gtk_container_add(GTK_CONTAINER(janela), caixa_principal);

    cabecalho = gtk_label_new("PROJETO ASSISTENTE VIRTUAL");
    gtk_widget_set_name(cabecalho, "cabecalho");
    gtk_box_pack_start(GTK_BOX(caixa_principal), cabecalho, FALSE, FALSE, 0);

    rolagem = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(rolagem),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC
    );
    gtk_widget_set_name(rolagem, "rolagem");

    gtk_box_pack_start(GTK_BOX(caixa_principal), rolagem, TRUE, TRUE, 0);

    area_conversa = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(area_conversa), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(area_conversa), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(area_conversa), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_name(area_conversa, "area-conversa");

    gtk_container_add(GTK_CONTAINER(rolagem), area_conversa);

    caixa_inferior = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_name(caixa_inferior, "caixa-inferior");
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

        "#caixa-principal {"
        "   background-color: #000000;"
        "}"

        "#cabecalho {"
        "   background-color: #000000;"
        "   color: #00ff00;"
        "   font-family: Consolas, monospace;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "}"

        "#rolagem {"
        "   background-color: #000000;"
        "   border: 1px solid #003300;"
        "}"

        "#rolagem viewport {"
        "   background-color: #000000;"
        "}"

        "#area-conversa {"
        "   background-color: #000000;"
        "   color: #00ff00;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   padding: 12px;"
        "}"

        "#area-conversa text {"
        "   background-color: #000000;"
        "   color: #00ff00;"
        "}"

        "#area-conversa text selection {"
        "   background-color: #003300;"
        "   color: #00ff00;"
        "}"

        "#entrada-mensagem {"
        "   background-color: #000000;"
        "   color: #00ff00;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "   border: 1px solid #003300;"
        "}"

        "#entrada-mensagem:focus {"
        "   border: 1px solid #00ff00;"
        "}"

        "#entrada-mensagem selection {"
        "   background-color: #003300;"
        "   color: #00ff00;"
        "}"

        "#botao-enviar {"
        "   background-color: #000000;"
        "   color: #00ff00;"
        "   font-family: Consolas, monospace;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 10px 18px;"
        "   border: 1px solid #003300;"
        "}"

        "#botao-enviar:hover {"
        "   background-color: #001a00;"
        "   color: #00ff00;"
        "}"

        "#botao-enviar:active {"
        "   background-color: #003300;"
        "   color: #00ff00;"
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
        "Microsoft Windows [versao ProjetoAssistenteVirtual]\n"
        "(c) Projeto academico em linguagem C. Todos os direitos reservados.\n\n"
        "Assistente> Sistema iniciado.\n"
        "Assistente> Ola, Fabio. Como posso ajudar?\n\n"
    );

    gtk_widget_show_all(janela);

    gtk_main();

    g_free(interface);
}