#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "ia.h"

typedef struct {
    GtkWidget *janela;
    GtkWidget *area_texto;
    GtkTextBuffer *buffer_texto;
    GtkWidget *entrada_mensagem;
    GtkWidget *botao_enviar;
} InterfaceDados;

/**
 * @brief Adiciona texto no final da área de exibição e força a rolagem automática do scrollbar.
 */
static void adicionar_texto_terminal(InterfaceDados *interface, const char *texto) {
    GtkTextIter fim;

    gtk_text_buffer_get_end_iter(interface->buffer_texto, &fim);
    gtk_text_buffer_insert(interface->buffer_texto, &fim, texto, -1);

    // Cria uma marca temporária para rastrear a última linha inserida
    GtkTextMark *marca_fim = gtk_text_buffer_create_mark(
        interface->buffer_texto, NULL, &fim, FALSE
    );

    // Desloca a visão da tela para manter a última marca visível
    gtk_text_view_scroll_mark_onscreen(
        GTK_TEXT_VIEW(interface->area_texto), marca_fim
    );

    gtk_text_buffer_delete_mark(interface->buffer_texto, marca_fim);
}

/**
 * @brief Handler de evento (Callback) acionado ao disparar o envio da mensagem.
 * @note ATENÇÃO: Esta função roda na Thread principal da UI, causando travamento temporário.
 */
static void enviar_mensagem(GtkWidget *widget, gpointer dados) {
    InterfaceDados *interface = (InterfaceDados *) dados;

    const char *texto_digitado =
        gtk_entry_get_text(GTK_ENTRY(interface->entrada_mensagem));

    if (texto_digitado == NULL || strlen(texto_digitado) == 0) {
        return;
    }

    char *mensagem_usuario = g_strdup(texto_digitado);

    adicionar_texto_terminal(interface, "Você: ");
    adicionar_texto_terminal(interface, mensagem_usuario);
    adicionar_texto_terminal(interface, "\n");

    // Reseta o campo de entrada para o usuário poder digitar novamente
    gtk_entry_set_text(GTK_ENTRY(interface->entrada_mensagem), "");

    adicionar_texto_terminal(interface, "IA: pensando...\n");

    // Processa eventos remanescentes na fila do GTK para desenhar o "pensando..." antes de travar
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }

    // Chamada síncrona bloqueante. Trava os inputs do usuário até retornar.
    char *resposta_ia = enviar_mensagem_ia(mensagem_usuario);

    if (resposta_ia != NULL) {
        adicionar_texto_terminal(interface, "IA: ");
        adicionar_texto_terminal(interface, resposta_ia);
        adicionar_texto_terminal(interface, "\n\n");
        free(resposta_ia);
    } else {
        adicionar_texto_terminal(
            interface,
            "IA: Não foi possível obter uma resposta.\n\n"
        );
    }

    g_free(mensagem_usuario);
}

/**
 * @brief Injeta regras CSS no escopo do display GDK para customizar cores e fontes.
 */
static void aplicar_estilo_cmd(void) {
    GtkCssProvider *css = gtk_css_provider_new();

    // Paleta de estilo inspirada em emulação de terminais CRT clássicos (verde sobre preto)
    const char *estilo =
        "window { background-color: #000000; }"
        "textview { background-color: #000000; color: #00ff00; font-family: Consolas, monospace; font-size: 14px; }"
        "textview text { background-color: #000000; color: #00ff00; }"
        "entry { background-color: #000000; color: #00ff00; border: 1px solid #00aa00; caret-color: #00ff00; font-family: Consolas, monospace; font-size: 14px; padding: 8px; }"
        "button { background-color: #001a00; color: #00ff00; border: 1px solid #00aa00; font-family: Consolas, monospace; font-size: 14px; padding: 8px 14px; }"
        "button:hover { background-color: #003300; }"
        "scrolledwindow { border: 1px solid #00aa00; }";

    gtk_css_provider_load_from_data(css, estilo, -1, NULL);
    
    // Associa o provedor CSS globalmente em todas as telas conectadas
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(css);
}

/**
 * @brief Inicializa os subsistemas do GTK, monta a árvore de componentes e inicia o loop principal.
 */
void iniciar_interface(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    aplicar_estilo_cmd();

    InterfaceDados *interface = malloc(sizeof(InterfaceDados));

    interface->janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(interface->janela), "Assistente Virtual");
    gtk_window_set_default_size(GTK_WINDOW(interface->janela), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(interface->janela), 15);

    // Intercepta o sinal de fechamento da janela para encerrar o processo limpadamente
    g_signal_connect(interface->janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *caixa_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(interface->janela), caixa_principal);

    GtkWidget *rolagem = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(rolagem), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(caixa_principal), rolagem, TRUE, TRUE, 0);

    interface->area_texto = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(interface->area_texto), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(interface->area_texto), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(interface->area_texto), GTK_WRAP_WORD_CHAR);

    interface->buffer_texto = gtk_text_view_get_buffer(GTK_TEXT_VIEW(interface->area_texto));
    gtk_container_add(GTK_CONTAINER(rolagem), interface->area_texto);

    GtkWidget *caixa_entrada = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(caixa_principal), caixa_entrada, FALSE, FALSE, 0);

    interface->entrada_mensagem = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(interface->entrada_mensagem), "Digite sua mensagem...");
    gtk_box_pack_start(GTK_BOX(caixa_entrada), interface->entrada_mensagem, TRUE, TRUE, 0);

    interface->botao_enviar = gtk_button_new_with_label("Enviar");
    gtk_box_pack_start(GTK_BOX(caixa_entrada), interface->botao_enviar, FALSE, FALSE, 0);

    // Mapeamento de gatilhos: Clique no botão ou pressionamento da tecla Enter (activate)
    g_signal_connect(interface->botao_enviar, "clicked", G_CALLBACK(enviar_mensagem), interface);
    g_signal_connect(interface->entrada_mensagem, "activate", G_CALLBACK(enviar_mensagem), interface);

    adicionar_texto_terminal(
        interface,
        "Assistente Virtual iniciado.\n"
        "Digite uma mensagem e pressione Enter.\n\n"
    );

    gtk_widget_show_all(interface->janela);
    
    // Inicia o loop de controle de eventos do GTK. Bloqueia a execução da main aqui.
    gtk_main();

    // Executado apenas após a saída do loop (gtk_main_quit)
    free(interface);
}