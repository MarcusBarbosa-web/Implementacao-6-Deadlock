# Algoritmo do Banqueiro (Banker's Algorithm)

Implementação 6.
Programa em C que simula o algoritmo do banqueiro para evitar deadlock, de acordo com o livro *Operating System Concepts*, com o número de clientes e de recursos definido pela entrada.

**Autor:** Marcus Vinícius Pereira Barbosa (mvpb@cesar.school)

## Estrutura do repositório

| Arquivo | Descrição |
|---|---|
| `banker.c` | Código-fonte do programa |
| `Makefile` | Compilação (`make`) e limpeza (`make clean`) |
| `evidencias.log` | Evidências de execução e testes |
| `README.md` | Este arquivo |
| Relatório (PDF) | Relatório da implementação |

## Compilação

```bash
make
```

Gera o executável `banker`. Para apagar o que foi compilado:

```bash
make clean
```

## Execução

```bash
./banker 10 5 7
```

O exemplo acima cria 3 tipos de recursos, com 10, 5 e 7 instâncias. O programa precisa de `customer.txt` e `commands.txt` na pasta em que for executado e gera `result.txt` nessa mesma pasta.

## Arquivos de entrada

### customer.txt

Cada linha é um cliente, e cada número é o máximo (MAXIMUM) que ele pode pedir de cada recurso, separados por vírgula. O número de linhas define o número de clientes. Exemplo com 5 clientes e 3 recursos:

```
7,5,3
3,2,2
9,0,2
2,2,2
4,3,3
```

### commands.txt

Uma sequência de comandos, um por linha:

| Comando | Significado |
|---|---|
| `RQ <cliente> <r1> <r2> ...` | Cliente pede recursos (request) |
| `RL <cliente> <r1> <r2> ...` | Cliente libera recursos (release) |
| `*` | Escreve o estado atual (MAXIMUM, ALLOCATION, NEED e AVAILABLE) |

## Funcionamento

Para cada `RQ`, o programa verifica, nesta ordem:

1. Se o pedido excede o NEED do cliente, é negado (`exceed its maximum allocation`).
2. Se o pedido excede o AVAILABLE, é negado (`are not enough`).
3. Se o pedido deixaria o sistema em estado inseguro, o estado é restaurado e o pedido é negado (`result in an unsafe state`). A segurança é verificada pelo algoritmo de segurança do livro.

Se passar nas três verificações, os recursos são alocados. Para cada `RL`, o programa verifica se o cliente tem os recursos que quer devolver e, em caso positivo, atualiza AVAILABLE, ALLOCATION e NEED.

## Saída (result.txt)

As mensagens seguem os mesmos formatos dos exemplos:

```
Allocate to customer 0 the resources 0 2 1
Release from customer 0 the resources 0 1 1
The resources 3 3 2 are not enough to customer 4 request 4 0 0
The customer 0 request 0 2 0 was denied because result in an unsafe state
The customer 2 request 0 1 0 was denied because exceed its maximum allocation
```

O comando `*` escreve a tabela `MAXIMUM | ALLOCATION | NEED` seguida da linha `AVAILABLE`.
  