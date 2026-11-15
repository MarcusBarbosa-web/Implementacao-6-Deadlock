# Algoritmo do Banqueiro (Banker's Algorithm)

Implementação 5 da disciplina de Infraestrutura de Software (Cesar School).
Programa em C que simula o algoritmo do banqueiro para evitar deadlock, conforme
o livro *Operating System Concepts* (Silberschatz et al., 10ª edição, p. P-45),
com o número de clientes e de recursos definido pela entrada.

**Autor:** Marcus Barbosa (mvpb@cesar.school)

## Estrutura do repositório

| Arquivo | Descrição |
|---|---|
| `banker.c` | Código-fonte do programa |
| `Makefile` | Compilação (`make`) e limpeza (`make clean`) |
| `evidencias.log` | Evidências de execução e testes |
| `README.md` | Este arquivo |
| Relatório (PDF) | Relatório da implementação |

Os arquivos `customer.txt`, `commands.txt` e `result.txt` de exemplo foram
fornecidos pelo professor e não fazem parte da entrega.

## Requisitos

- Linux, Unix ou macOS
- `gcc` e `make`

## Compilação

```bash
make
```

Gera o executável `banker`. Para apagar o que foi compilado:

```bash
make clean
```

## Execução

Cada argumento é o número total de instâncias de um tipo de recurso:

```bash
./banker 10 5 7
```

O exemplo acima cria 3 tipos de recursos, com 10, 5 e 7 instâncias.
O programa precisa de `customer.txt` e `commands.txt` na pasta em que for executado
e gera `result.txt` nessa mesma pasta.

## Arquivos de entrada

### customer.txt

Cada linha é um cliente, e cada número é o máximo (MAXIMUM) que ele pode pedir
de cada recurso, separados por vírgula. O número de linhas define o número de
clientes. Exemplo com 5 clientes e 3 recursos:

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
3. Se o pedido deixaria o sistema em estado inseguro, o estado é restaurado e o
   pedido é negado (`result in an unsafe state`). A segurança é verificada pelo
   algoritmo de segurança do livro.

Se passar nas três verificações, os recursos são alocados. Para cada `RL`, o
programa verifica se o cliente tem os recursos que quer devolver e, em caso
positivo, atualiza AVAILABLE, ALLOCATION e NEED.

## Saída (result.txt)

As mensagens seguem o formato do exemplo do professor:

```
Allocate to customer 0 the resources 0 2 1
Release from customer 0 the resources 0 1 1
The resources 3 3 2 are not enough to customer 4 request 4 0 0
The customer 0 request 0 2 0 was denied because result in an unsafe state
The customer 2 request 0 1 0 was denied because exceed its maximum allocation
```

O comando `*` escreve a tabela `MAXIMUM | ALLOCATION | NEED` seguida da linha `AVAILABLE`.

## Tratamento de erros

O programa não falha silenciosamente: mostra uma mensagem na saída de erro
(`stderr`) e encerra liberando a memória alocada. Os casos tratados são:

- Nenhum argumento informado, ou argumento que não seja um inteiro não negativo.
- `customer.txt` ou `commands.txt` inexistente, impossível de abrir ou vazio (customer).
- Linha de `customer.txt` com quantidade de colunas diferente do número de recursos,
  ou com caracteres inválidos.
- Linha de `commands.txt` com comando desconhecido, número de valores diferente do
  número de recursos, valores negativos ou cliente inexistente.
- Falha de alocação de memória.

Os dois arquivos de entrada são totalmente validados **antes** de qualquer comando
ser executado. Se houver erro, o `result.txt` não é criado nem alterado.

## Decisões de projeto

- As matrizes são alocadas dinamicamente (`int **`, um `malloc` por linha), já
  que o número de clientes e de recursos só é conhecido na execução.
- Se um cliente tentar liberar (`RL`) mais do que tem alocado, a liberação é negada
  com a mensagem `The customer N release ... was denied because exceed its allocation`.
  O enunciado não define esse caso, então o texto segue o padrão das demais mensagens.
- No enunciado, o exemplo de saída mostra `3 2 2` na mensagem de recursos
  insuficientes, mas o AVAILABLE naquele ponto é `3 3 2` (como mostram a tabela
  anterior e o `result.txt` fornecido). O programa segue o `result.txt`.
  