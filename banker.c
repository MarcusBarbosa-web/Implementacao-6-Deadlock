#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

            if(fim == atual || valor < 0 || valor > INT_MAX){
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


int ler_txt_commands(info *dados){
    FILE *arquivo;
    char linha[300];

    char *atual, *fim;
    long valor;
    int j;
    int numero_linha = 0;

    arquivo = fopen("commands.txt", "r");

    if(arquivo == NULL){
        fprintf(stderr, "Nao foi possivel abrir commands.txt\n");
        return 1;
    }

    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        numero_linha++;

        if(linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0'){
            continue;
        }

        atual = linha;

        if(atual[0] == '*'){
            atual++;

        }else{
            if(atual[0] != 'R' || (atual[1] != 'Q' && atual[1] != 'L') || atual[2] != ' '){
                goto invalido;
            }
            atual += 2;

            valor = strtol(atual, &fim, 10);

            if(fim == atual || valor < 0 || valor >= dados->n_clientes){
                goto invalido;
            }

            atual = fim;

            for(j = 0; j < dados->n_recursos; j++){
                valor = strtol(atual, &fim, 10);

                if(fim == atual || valor < 0 || valor > INT_MAX){
                    goto invalido;
                }
                atual = fim;
            }
        }

        while(*atual == ' ' || *atual == '\t'){
            atual++;
        }
        if(*atual != '\n' && *atual != '\r' && *atual != '\0'){
            goto invalido;
        }
    }

    fclose(arquivo);
    return 0;

invalido:
    fprintf(stderr, "Formato invalido em commands.txt (linha %d)\n", numero_linha);

    fclose(arquivo);
    return 1;
}


int inicializar_matrizes(info *dados){
    int i, j;

    dados->allocation = calloc(dados->n_clientes, sizeof(int*));
    dados->need = calloc(dados->n_clientes, sizeof(int*));

    if(dados->allocation == NULL || dados->need == NULL){
        fprintf(stderr, "Falha de memoria\n");
        goto limpar;
    }

    for(i = 0; i < dados->n_clientes; i++){
        dados->allocation[i] = calloc(dados->n_recursos, sizeof(int));
        dados->need[i] = malloc(dados->n_recursos * sizeof(int));

        if(dados->allocation[i] == NULL || dados->need[i] == NULL){
            fprintf(stderr, "Falha de memoria\n");
            goto limpar;
        }

        for(j = 0; j < dados->n_recursos; j++){
            dados->need[i][j] = dados->maximum[i][j] - dados->allocation[i][j];
        }
    }

    return 0;

limpar:
    liberar_matriz(dados->allocation, dados->n_clientes);
    liberar_matriz(dados->need, dados->n_clientes);
    dados->allocation = NULL;
    dados->need = NULL;
    return 1;
}


void imprimir_estado(info *dados, FILE *saida){
    int **matrizes[3];
    int largura[3] = {10, 12, 0};
    int i, j, k, escrito;

    matrizes[0] = dados->maximum;
    matrizes[1] = dados->allocation;
    matrizes[2] = dados->need;

    fprintf(saida, "MAXIMUM   |   ALLOCATION  |   NEED\n");

    for(i = 0; i < dados->n_clientes; i++){
        for(k = 0; k < 3; k++){
            escrito = 0;

            for(j = 0; j < dados->n_recursos; j++){
                escrito += fprintf(saida, "%d ", matrizes[k][i][j]);
            }

            if(k < 2){
                while(escrito < largura[k]){
                    fputc(' ', saida);
                    escrito++;
                }
                fprintf(saida, "|   ");
            }
        }
        fprintf(saida, "\n");
    }

    fprintf(saida, "AVAILABLE ");

    for(j = 0; j < dados->n_recursos; j++){
        fprintf(saida, "%d ", dados->available[j]);
    }
    fprintf(saida, "\n");
}


int is_safe(info *dados){
    int *work, *finish;
    int i, j, achou, pode;
    int concluidos = 0;

    work = malloc(dados->n_recursos * sizeof(int));
    finish = calloc(dados->n_clientes, sizeof(int));

    if(work == NULL || finish == NULL){
        fprintf(stderr, "Falha de memoria\n");
        free(work);
        free(finish);
        return -1;
    }

    for(j = 0; j < dados->n_recursos; j++){
        work[j] = dados->available[j];
    }

    do{
        achou = 0;

        for(i = 0; i < dados->n_clientes; i++){
            if(finish[i]){
                continue;
            }

            pode = 1;

            for(j = 0; j < dados->n_recursos; j++){
                if(dados->need[i][j] > work[j]){
                    pode = 0;
                    break;
                }
            }

            if(pode){
                for(j = 0; j < dados->n_recursos; j++){
                    work[j] += dados->allocation[i][j];
                }
                finish[i] = 1;
                concluidos++;
                achou = 1;
            }
        }

    }while(achou);

    free(work);
    free(finish);

    return concluidos == dados->n_clientes;
}


void imprimir_vetor(FILE *saida, int *v, int n){
    int j;

    for(j = 0; j < n; j++){
        if(j > 0){
            fputc(' ', saida);
        }
        fprintf(saida, "%d", v[j]);
    }
}


int request_resources(info *dados, int cliente, int *pedido, FILE *saida){
    int j, seguro;

    for(j = 0; j < dados->n_recursos; j++){
        if(pedido[j] > dados->need[cliente][j]){
            fprintf(saida, "The customer %d request ", cliente);
            imprimir_vetor(saida, pedido, dados->n_recursos);
            fprintf(saida, " was denied because exceed its maximum allocation\n");
            return 1;
        }
    }

    for(j = 0; j < dados->n_recursos; j++){
        if(pedido[j] > dados->available[j]){
            fprintf(saida, "The resources ");
            imprimir_vetor(saida, dados->available, dados->n_recursos);
            fprintf(saida, " are not enough to customer %d request ", cliente);
            imprimir_vetor(saida, pedido, dados->n_recursos);
            fprintf(saida, " \n");
            return 1;
        }
    }

    for(j = 0; j < dados->n_recursos; j++){
        dados->available[j] -= pedido[j];
        dados->allocation[cliente][j] += pedido[j];
        dados->need[cliente][j] -= pedido[j];
    }

    seguro = is_safe(dados);

    if(seguro != 1){
        for(j = 0; j < dados->n_recursos; j++){
            dados->available[j] += pedido[j];
            dados->allocation[cliente][j] -= pedido[j];
            dados->need[cliente][j] += pedido[j];
        }

        if(seguro == -1){
            return -1;
        }

        fprintf(saida, "The customer %d request ", cliente);
        imprimir_vetor(saida, pedido, dados->n_recursos);
        fprintf(saida, " was denied because result in an unsafe state\n");
        return 1;
    }

    fprintf(saida, "Allocate to customer %d the resources ", cliente);
    imprimir_vetor(saida, pedido, dados->n_recursos);
    fprintf(saida, "\n");
    return 0;
}


int release_resources(info *dados, int cliente, int *devolucao, FILE *saida){
    int j;

    for(j = 0; j < dados->n_recursos; j++){
        if(devolucao[j] > dados->allocation[cliente][j]){
            fprintf(saida, "The customer %d release ", cliente);
            imprimir_vetor(saida, devolucao, dados->n_recursos);
            fprintf(saida, " was denied because exceed its allocation\n");
            return 1;
        }
    }

    for(j = 0; j < dados->n_recursos; j++){
        dados->available[j] += devolucao[j];
        dados->allocation[cliente][j] -= devolucao[j];
        dados->need[cliente][j] += devolucao[j];
    }

    fprintf(saida, "Release from customer %d the resources ", cliente);
    imprimir_vetor(saida, devolucao, dados->n_recursos);
    fprintf(saida, "\n");
    return 0;
}


int executar_comandos(info *dados, FILE *saida){
    FILE *arquivo;
    char linha[300];
    char *atual, *fim;
    char tipo;
    int *valores;
    int cliente, j;

    arquivo = fopen("commands.txt", "r");

    if(arquivo == NULL){
        fprintf(stderr, "Nao foi possivel abrir commands.txt\n");
        return 1;
    }

    valores = malloc(dados->n_recursos * sizeof(int));

    if(valores == NULL){
        fprintf(stderr, "Falha de memoria\n");
        fclose(arquivo);
        return 1;
    }

    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        if(linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0'){
            continue;
        }

        if(linha[0] == '*'){
            imprimir_estado(dados, saida);
            continue;
        }

        tipo = linha[1];
        atual = linha + 2;

        cliente = (int)strtol(atual, &fim, 10);
        atual = fim;

        for(j = 0; j < dados->n_recursos; j++){
            valores[j] = (int)strtol(atual, &fim, 10);
            atual = fim;
        }

        if(tipo == 'Q'){
            if(request_resources(dados, cliente, valores, saida) == -1){
                free(valores);
                fclose(arquivo);
                return 1;
            }
        }else{
            release_resources(dados, cliente, valores, saida);
        }
    }

    free(valores);
    fclose(arquivo);
    return 0;
}


int ler_argumentos(int argc, char *argv[], info *dados){
    char *fim;
    long valor;
    int j;

    if(argc < 2){
        fprintf(stderr, "Uso: %s <recurso1> <recurso2> ...\n", argv[0]);
        return 1;
    }

    dados->n_recursos = argc - 1;
    dados->available = malloc(dados->n_recursos * sizeof(int));

    if(dados->available == NULL){
        fprintf(stderr, "Falha de memoria\n");
        return 1;
    }

    for(j = 0; j < dados->n_recursos; j++){
        valor = strtol(argv[j + 1], &fim, 10);

        if(fim == argv[j + 1] || *fim != '\0' || valor < 0 || valor > INT_MAX){
            fprintf(stderr, "Argumento invalido: %s (esperado inteiro nao negativo)\n", argv[j + 1]);

            free(dados->available);
            dados->available = NULL;
            return 1;
        }
        dados->available[j] = (int)valor;
    }

    return 0;
}





int main(int argc, char *argv[]){
    info dados = {0};
    FILE *saida;

    if(ler_argumentos(argc, argv, &dados) != 0){
        return 1;
    }

    if(ler_txt_customer(&dados) != 0){
        free(dados.available);
        return 1;
    }

    if(ler_txt_commands(&dados) != 0){
        liberar_matriz(dados.maximum, dados.n_clientes);
        free(dados.available);
        return 1;
    }

    if(inicializar_matrizes(&dados) != 0){
        liberar_matriz(dados.maximum, dados.n_clientes);
        free(dados.available);
        return 1;
    }

    saida = fopen("result.txt", "w");

    if(saida == NULL){
        fprintf(stderr, "Nao foi possivel criar result.txt\n");
        liberar_matriz(dados.maximum, dados.n_clientes);
        liberar_matriz(dados.allocation, dados.n_clientes);
        liberar_matriz(dados.need, dados.n_clientes);
        free(dados.available);
        return 1;
    }

    if(executar_comandos(&dados, saida) != 0){
        fclose(saida);
        liberar_matriz(dados.maximum, dados.n_clientes);
        liberar_matriz(dados.allocation, dados.n_clientes);
        liberar_matriz(dados.need, dados.n_clientes);
        free(dados.available);
        return 1;
    }

    fclose(saida);
    liberar_matriz(dados.maximum, dados.n_clientes);
    liberar_matriz(dados.allocation, dados.n_clientes);
    liberar_matriz(dados.need, dados.n_clientes);
    free(dados.available);

    return 0;
}