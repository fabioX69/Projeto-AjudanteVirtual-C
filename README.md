# Assistente Virtual em C

Projeto de um assistente virtual desktop desenvolvido em **C**, com:

- Interface gráfica em **GTK**
- Integração com a **API Gemini**
- Conexão com banco de dados **MySQL**

## Tecnologias

- C
- GTK 3
- libcurl
- cJSON
- Gemini API
- MySQL
- MSYS2 MINGW64

## Estrutura principal

```text
include/
├── interface.h
├── ia.h
├── db.h
└── cJSON.h

src/
├── main.c
├── interface.c
├── ia.c
├── db.c
└── cJSON.c
```

## Dependências

No **MSYS2 MINGW64**:

```bash
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-curl
pacman -S mingw-w64-x86_64-libmariadbclient
```

## Configurar a chave Gemini

```bash
export GEMINI_API_KEY="SUA_CHAVE_AQUI"
```

## Banco de dados

Banco utilizado:

```sql
CREATE DATABASE db_assistente_virtual;
```

Usuário usado no projeto:

```sql
CREATE USER 'assistente_user'@'localhost'
IDENTIFIED BY 'assistente123';

GRANT ALL PRIVILEGES
ON db_assistente_virtual.*
TO 'assistente_user'@'localhost';
```

## Compilar

```bash
gcc src/main.c src/interface.c src/db.c src/ia.c src/cJSON.c \
-Iinclude \
-o assistente.exe \
$(pkg-config --cflags --libs gtk+-3.0 libcurl mysqlclient)
```

## Executar

```bash
./assistente.exe
```

## Status atual

- Interface gráfica funcionando
- IA respondendo dentro da interface
- Banco de dados conectado
- Projeto pronto para salvar histórico de conversas

## Próximos passos

- Criar tabelas no banco
- Salvar mensagens e respostas
- Carregar histórico ao abrir o programa
- Melhorar a interface e o desempenho

