#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_FILME 100
#define MAX_GENERO_FILME 100
#define MAX_USUARIO 100
#define MAX_FAVORITOS 100

typedef struct {
    int id;
    char filme[MAX_NOME_FILME];
    char genero[MAX_GENERO_FILME];
} Filme;

typedef struct {
    int id;
    char nome[MAX_USUARIO];
    int filmes_favoritos[MAX_FAVORITOS];
    int num_favoritos;
} Usuario;

void adicionar_filme(FILE *filme_file);
void adicionar_usuario(FILE *user_file);
void listar_filmes(FILE *filme_file);
void listar_usuarios(FILE *user_file);
void adicionar_favorito(FILE *filme_file, FILE *user_file);

int main() {
    FILE *filme_file, *user_file;

    filme_file = fopen("filmes.bin", "rb+");
    if (!filme_file) filme_file = fopen("filmes.bin", "wb+");

    user_file = fopen("usuarios.bin", "rb+");
    if (!user_file) user_file = fopen("usuarios.bin", "wb+");

    if (!filme_file || !user_file) {
        printf("Erro ao abrir os arquivos binários.\n");
        return 1;
    }

    int opcao;
    do {
        printf("\n=== MENU ===\n");
        printf("1. Adicionar Filme\n");
        printf("2. Adicionar Usuário\n");
        printf("3. Listar Filmes\n");
        printf("4. Listar Usuários\n");
        printf("5. Adicionar Favorito\n");
        printf("6. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionar_filme(filme_file);
                break;
            case 2:
                adicionar_usuario(user_file);
                break;
            case 3:
                listar_filmes(filme_file);
                break;
            case 4:
                listar_usuarios(user_file);
                break;
            case 5:
                adicionar_favorito(filme_file, user_file);
                break;
            case 6:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 6);

    fclose(filme_file);
    fclose(user_file);

    return 0;
}

void adicionar_filme(FILE *filme_file) {
    Filme filme = {0};
    printf("\n=== Adicionar Filme ===\n");
    printf("ID: ");
    scanf("%d", &filme.id);
    printf("Título: ");
    getchar();
    fgets(filme.filme, MAX_NOME_FILME, stdin);
    strtok(filme.filme, "\n");
    printf("Gênero: ");
    fgets(filme.genero, MAX_GENERO_FILME, stdin);
    strtok(filme.genero, "\n");

    fseek(filme_file, 0, SEEK_END);
    fwrite(&filme, sizeof(Filme), 1, filme_file);
    fflush(filme_file);
}

void listar_filmes(FILE *filme_file) {
    Filme filme;
    fseek(filme_file, 0, SEEK_SET);
    printf("\n=== Lista de Filmes ===\n");
    while (fread(&filme, sizeof(Filme), 1, filme_file)) {
        printf("ID: %d, Título: %s, Gênero: %s\n", filme.id, filme.filme, filme.genero);
    }
}

void adicionar_usuario(FILE *user_file) {
    Usuario usuario = {0};
    printf("\n=== Adicionar Usuário ===\n");
    printf("ID: ");
    scanf("%d", &usuario.id);
    printf("Nome: ");
    getchar();
    fgets(usuario.nome, MAX_USUARIO, stdin);
    strtok(usuario.nome, "\n");
    usuario.num_favoritos = 0;

    fseek(user_file, 0, SEEK_END);
    fwrite(&usuario, sizeof(Usuario), 1, user_file);
    fflush(user_file);
}

void listar_usuarios(FILE *user_file) {
    Usuario usuario;
    fseek(user_file, 0, SEEK_SET);
    printf("\n=== Lista de Usuários ===\n");
    while (fread(&usuario, sizeof(Usuario), 1, user_file)) {
        printf("ID: %d, Nome: %s, Número de Favoritos: %d\n", usuario.id, usuario.nome, usuario.num_favoritos);
    }
}

void adicionar_favorito(FILE *filme_file, FILE *user_file) {
    Usuario usuario;
    Filme filme;
    int usuario_id, filme_id;
    int encontrado = 0;

    printf("\n=== Adicionar Filme Favorito ===\n");
    printf("ID do usuário: ");
    scanf("%d", &usuario_id);

    fseek(user_file, 0, SEEK_SET);
    while (fread(&usuario, sizeof(Usuario), 1, user_file)) {
        if (usuario.id == usuario_id) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Usuário não encontrado.\n");
        return;
    }

    printf("Usuário encontrado: %s\n", usuario.nome);
    listar_filmes(filme_file);

    printf("ID do filme para adicionar como favorito: ");
    scanf("%d", &filme_id);

    fseek(filme_file, 0, SEEK_SET);
    encontrado = 0;
    while (fread(&filme, sizeof(Filme), 1, filme_file)) {
        if (filme.id == filme_id) {
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("Filme não encontrado.\n");
        return;
    }

    if (usuario.num_favoritos >= MAX_FAVORITOS) {
        printf("O usuário já atingiu o limite de favoritos.\n");
        return;
    }

    usuario.filmes_favoritos[usuario.num_favoritos++] = filme_id;

    fseek(user_file, -sizeof(Usuario), SEEK_CUR);
    fwrite(&usuario, sizeof(Usuario), 1, user_file);
    fflush(user_file);

    printf("Filme '%s' adicionado aos favoritos de '%s'.\n", filme.filme, usuario.nome);
}

