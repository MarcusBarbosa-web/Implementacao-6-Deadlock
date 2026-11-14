#include <stdio.h>
#include <stdlib.h>

typedef struct code{
    int *available;
    int **maximum;
    int **allocation;
    int **need;
    int n_clientes;
    int n_recursos;
}info;

void liberar_matriz(int **m, int linhas){
    int i;

    if(m == NULL){
        return;
    }

    for(i = 0; i < linhas; i++){
        free(m[i]);
    }
    free(m);
}


int ler_txt_customer(info *dados){
    FILE *arquivo;
    char linha[300];

    char *atual, *fim;
    long valor;
    int i, j;

    arquivo = fopen("customer.txt", "r");

    if(arquivo == NULL){
        fprintf(stderr, "Nao foi possivel abrir customer.txt\n");
        return 1;
    }

    dados->n_clientes = 0;

    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        if(linha[0] != '\n' && linha[0] != '\r' && linha[0] != '\0'){
            dados->n_clientes++;
        }
    }

    if(dados->n_clientes == 0){
        fprintf(stderr, "O arquivo customer.txt esta vazio\n");

        fclose(arquivo);
        return 1;
    }

    dados->maximum = calloc(dados->n_clientes, sizeof(int*));

    if(dados->maximum == NULL){
        fprintf(stderr, "Falha de memoria\n");
        
        fclose(arquivo);
        return 1;
    }

    for(i = 0; i < dados->n_clientes; i++){
        dados->maximum[i] = malloc(dados->n_recursos * sizeof(int));

        if(dados->maximum[i] == NULL){
            fprintf(stderr, "Falha de memoria\n");
            goto limpar;
        }
    }

    rewind(arquivo);

    i = 0;

    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        if(linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0'){
            continue;
        }

        atual = linha;
        for(j = 0; j < dados->n_recursos; j++){
            valor = strtol(atual, &fim, 10);

            if(fim == atual || valor < 0){
                goto formato_invalido;
            }
            dados->maximum[i][j] = (int)valor;
            atual = fim;

            if(j < dados->n_recursos - 1){
                if(*atual != ','){
                    goto formato_invalido;
                }
                atual++;
            }
        }

        if(*atual != '\n' && *atual != '\r' && *atual != '\0'){
            goto formato_invalido;
        }
        i++;
    }

    fclose(arquivo);
    return 0;

formato_invalido:
    fprintf(stderr, "Formato invalido no arquivo customer.txt (cliente %d)\n", i);

limpar:
    fclose(arquivo);
    liberar_matriz(dados->maximum, dados->n_clientes);
    dados->maximum = NULL;
    return 1;
}





int main(int argc, char *argv[]){
    info dados = {0};
    int i, j;

    if(argc < 2){
        fprintf(stderr, "Uso: %s <recurso1> <recurso2> ...\n", argv[0]);
        return 1;
    }

    dados.n_recursos = argc - 1;

    dados.available = malloc(dados.n_recursos * sizeof(int));

    if(dados.available == NULL){
        fprintf(stderr, "Falha de memoria\n");
        return 1;
    }

    for(j = 0; j < dados.n_recursos; j++){
        dados.available[j] = atoi(argv[j + 1]);
    }

    if(ler_txt_customer(&dados) != 0){
        free(dados.available);
        return 1;
    }

    for(i = 0; i < dados.n_clientes; i++){
        for(j = 0; j < dados.n_recursos; j++){
            printf("%d ", dados.maximum[i][j]);
        }
        printf("\n");
    }

    liberar_matriz(dados.maximum, dados.n_clientes);
    free(dados.available);

    return 0;
}