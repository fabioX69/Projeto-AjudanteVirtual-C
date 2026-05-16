#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "ia.h"

typedef struct {
    char *dados;
    size_t tamanho;
} RespostaAPI;

static size_t receber_resposta(
    void *conteudo,
    size_t tamanho,
    size_t quantidade,
    void *usuario
) {
    size_t tamanho_real = tamanho * quantidade;
    RespostaAPI *resposta = (RespostaAPI *) usuario;

    char *novo_buffer = realloc(
        resposta->dados,
        resposta->tamanho + tamanho_real + 1
    );

    if (novo_buffer == NULL) {
        return 0;
    }

    resposta->dados = novo_buffer;

    memcpy(
        resposta->dados + resposta->tamanho,
        conteudo,
        tamanho_real
    );

    resposta->tamanho += tamanho_real;
    resposta->dados[resposta->tamanho] = '\0';

    return tamanho_real;
}

static char *copiar_texto(const char *texto) {
    if (texto == NULL) {
        return NULL;
    }

    size_t tamanho = strlen(texto) + 1;
    char *copia = malloc(tamanho);

    if (copia == NULL) {
        return NULL;
    }

    memcpy(copia, texto, tamanho);
    return copia;
}

char* enviar_mensagem_ia(const char* mensagem) {
    if (mensagem == NULL || strlen(mensagem) == 0) {
        printf("Erro: mensagem vazia.\n");
        return NULL;
    }

    const char *api_key = getenv("GEMINI_API_KEY");

    if (api_key == NULL || strlen(api_key) == 0) {
        printf("Erro: variável GEMINI_API_KEY não configurada.\n");
        return NULL;
    }

    CURL *curl = curl_easy_init();

    if (curl == NULL) {
        printf("Erro ao iniciar o CURL.\n");
        return NULL;
    }

    RespostaAPI resposta;
    resposta.dados = malloc(1);
    resposta.tamanho = 0;

    if (resposta.dados == NULL) {
        printf("Erro ao reservar memória para a resposta.\n");
        curl_easy_cleanup(curl);
        return NULL;
    }

    resposta.dados[0] = '\0';

    cJSON *json_requisicao = cJSON_CreateObject();
    cJSON *contents = cJSON_CreateArray();
    cJSON *conteudo = cJSON_CreateObject();
    cJSON *parts = cJSON_CreateArray();
    cJSON *parte_texto = cJSON_CreateObject();

    if (
        json_requisicao == NULL ||
        contents == NULL ||
        conteudo == NULL ||
        parts == NULL ||
        parte_texto == NULL
    ) {
        printf("Erro ao criar JSON da requisição.\n");

        cJSON_Delete(json_requisicao);
        free(resposta.dados);
        curl_easy_cleanup(curl);

        return NULL;
    }

    cJSON_AddStringToObject(parte_texto, "text", mensagem);
    cJSON_AddItemToArray(parts, parte_texto);
    cJSON_AddItemToObject(conteudo, "parts", parts);
    cJSON_AddItemToArray(contents, conteudo);
    cJSON_AddItemToObject(json_requisicao, "contents", contents);

    char *corpo_json = cJSON_PrintUnformatted(json_requisicao);

    if (corpo_json == NULL) {
        printf("Erro ao transformar JSON em texto.\n");

        cJSON_Delete(json_requisicao);
        free(resposta.dados);
        curl_easy_cleanup(curl);

        return NULL;
    }

    struct curl_slist *cabecalhos = NULL;

    cabecalhos = curl_slist_append(
        cabecalhos,
        "Content-Type: application/json"
    );

    char cabecalho_api_key[512];

    snprintf(
        cabecalho_api_key,
        sizeof(cabecalho_api_key),
        "x-goog-api-key: %s",
        api_key
    );

    cabecalhos = curl_slist_append(
        cabecalhos,
        cabecalho_api_key
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-3-flash-preview:generateContent"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        cabecalhos
    );

    curl_easy_setopt(
        curl,
        CURLOPT_POST,
        1L
    );

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        corpo_json
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        receber_resposta
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &resposta
    );

    CURLcode resultado = curl_easy_perform(curl);

    if (resultado != CURLE_OK) {
        printf(
            "Erro na requisição: %s\n",
            curl_easy_strerror(resultado)
        );

        curl_slist_free_all(cabecalhos);
        curl_easy_cleanup(curl);
        cJSON_Delete(json_requisicao);
        free(corpo_json);
        free(resposta.dados);

        return NULL;
    }

    long codigo_http = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &codigo_http
    );

    if (codigo_http < 200 || codigo_http >= 300) {
        printf("Erro HTTP %ld retornado pela API.\n", codigo_http);
        printf("Resposta da API:\n%s\n", resposta.dados);

        curl_slist_free_all(cabecalhos);
        curl_easy_cleanup(curl);
        cJSON_Delete(json_requisicao);
        free(corpo_json);
        free(resposta.dados);

        return NULL;
    }

    cJSON *json_resposta = cJSON_Parse(resposta.dados);

    if (json_resposta == NULL) {
        printf("Erro ao interpretar a resposta JSON.\n");
        printf("Resposta recebida:\n%s\n", resposta.dados);

        curl_slist_free_all(cabecalhos);
        curl_easy_cleanup(curl);
        cJSON_Delete(json_requisicao);
        free(corpo_json);
        free(resposta.dados);

        return NULL;
    }

    char *texto_final = NULL;

    cJSON *candidates = cJSON_GetObjectItem(json_resposta, "candidates");

    if (cJSON_IsArray(candidates)) {
        cJSON *primeiro_candidate = cJSON_GetArrayItem(candidates, 0);

        if (primeiro_candidate != NULL) {
            cJSON *content = cJSON_GetObjectItem(primeiro_candidate, "content");

            if (content != NULL) {
                cJSON *parts_resposta = cJSON_GetObjectItem(content, "parts");

                if (cJSON_IsArray(parts_resposta)) {
                    cJSON *primeira_parte = cJSON_GetArrayItem(parts_resposta, 0);

                    if (primeira_parte != NULL) {
                        cJSON *texto = cJSON_GetObjectItem(primeira_parte, "text");

                        if (cJSON_IsString(texto) && texto->valuestring != NULL) {
                            texto_final = copiar_texto(texto->valuestring);
                        }
                    }
                }
            }
        }
    }

    if (texto_final == NULL) {
        printf("A API respondeu, mas não encontrei o texto final.\n");
        printf("Resposta completa:\n%s\n", resposta.dados);
    }

    curl_slist_free_all(cabecalhos);
    curl_easy_cleanup(curl);
    cJSON_Delete(json_requisicao);
    cJSON_Delete(json_resposta);
    free(corpo_json);
    free(resposta.dados);

    return texto_final;
}