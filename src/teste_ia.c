#include <stdio.h>
#include <stdlib.h>
#include "ia.h"

int main() {
    char *resposta = enviar_mensagem_ia(
        "Olá! Responda em uma frase curta dizendo que a integração com IA funcionou."
    );

    if (resposta != NULL) {
        printf("IA: %s\n", resposta);
        free(resposta);
    } else {
        printf("Não foi possível obter resposta da IA.\n");
    }

    return 0;
}
