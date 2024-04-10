#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MAQUINAS 3
#define NUM_TRABALHOS 3
#define NUM_OPERACOES_MAX 3

// Estrutura para representar uma operação
typedef struct {
    int maquina;  // Número da máquina onde a operação será realizada
    int duracao;  // Duração da operação
} Operacao;

// Estrutura para representar um trabalho
typedef struct {
    Operacao operacoes[NUM_OPERACOES_MAX];  // Array de operações para o trabalho
} Trabalho;

// Estrutura para representar o problema do Job-Shop
typedef struct {
    Trabalho trabalhos[NUM_TRABALHOS];  // Array de trabalhos
} JobShop;

// Função para inicializar o problema do Job-Shop com dados de exemplo
void inicializarJobShopExemplo(JobShop *job_shop) {
    // Exemplo de dados de entrada
    int dados[NUM_TRABALHOS][NUM_OPERACOES_MAX][2] = {
        {{0, 3}, {1, 2}, {2, 2}},
        {{0, 2}, {2, 1}, {1, 4}},
        {{1, 4}, {2, 3}, {0, 1}}
    };

    // Inicializar o problema do Job-Shop com os dados de exemplo
    for (int i = 0; i < NUM_TRABALHOS; i++) {
        for (int j = 0; j < NUM_OPERACOES_MAX; j++) {
            job_shop->trabalhos[i].operacoes[j].maquina = dados[i][j][0];
            job_shop->trabalhos[i].operacoes[j].duracao = dados[i][j][1];
        }
    }
}

// Função para exibir o problema do Job-Shop (apenas para fins de depuração)
void mostrarJobShop(JobShop *job_shop) {
    for (int i = 0; i < NUM_TRABALHOS; i++) {
        printf("Trabalho %d:\n", i);
        for (int j = 0; j < NUM_OPERACOES_MAX; j++) {
            printf("  Operacao %d: Maquina %d, Duracao %d\n", j, job_shop->trabalhos[i].operacoes[j].maquina, job_shop->trabalhos[i].operacoes[j].duracao);
        }
    }
}

int main() {
    // Inicializar o gerador de números aleatórios
    srand(time(NULL));

    // Criar e inicializar o problema do Job-Shop
    JobShop job_shop;
    inicializarJobShopExemplo(&job_shop);

    // Exibir o problema do Job-Shop (apenas para fins de depuração)
    mostrarJobShop(&job_shop);

    return 0;
}
