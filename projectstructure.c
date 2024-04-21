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

// Função para exibir uma operação
void mostrarOperacao(Operacao operacao, int inicio) {
    printf("Maquina %d, Tempo de Inicio %d, Duracao %d\n", operacao.maquina, inicio, operacao.duracao);
}

// Função para exibir o problema do Job-Shop (apenas para fins de depuração)
void mostrarJobShop(JobShop *job_shop) {
    for (int i = 0; i < NUM_TRABALHOS; i++) {
        printf("Trabalho %d:\n", i);
        for (int j = 0; j < NUM_OPERACOES_MAX; j++) {
            printf("  Operacao %d: ", j);
            mostrarOperacao(job_shop->trabalhos[i].operacoes[j], 0);
        }
    }
}

// Função para calcular o tempo total de conclusão de um escalonamento
int calcularTempoTotal(JobShop *job_shop) {
    int tempo_total = 0;
    int tempo_inicio[NUM_MAQUINAS] = {0}; // Tempo de início das operações em cada máquina

    for (int i = 0; i < NUM_TRABALHOS; i++) {
        for (int j = 0; j < NUM_OPERACOES_MAX; j++) {
            int maquina = job_shop->trabalhos[i].operacoes[j].maquina;
            int duracao = job_shop->trabalhos[i].operacoes[j].duracao;
            int inicio = tempo_inicio[maquina];
            tempo_inicio[maquina] += duracao; // Atualizar o tempo de início da próxima operação na máquina
            tempo_total = (tempo_inicio[maquina] > tempo_total) ? tempo_inicio[maquina] : tempo_total;
        }
    }

    return tempo_total;
}

// Função para calcular o próximo escalonamento
int proximoEscalonamento(JobShop *job_shop) {
    int i, j;

    // Encontrar a próxima tarefa a ser trocada
    for (i = NUM_TRABALHOS - 1; i >= 0; i--) {
        for (j = NUM_OPERACOES_MAX - 1; j >= 0; j--) {
            if (job_shop->trabalhos[i].operacoes[j].maquina < NUM_MAQUINAS - 1) {
                job_shop->trabalhos[i].operacoes[j].maquina++;
                return 1; // Encontrou um próximo escalonamento
            } else {
                job_shop->trabalhos[i].operacoes[j].maquina = 0;
            }
        }
    }

    return 0; // Não há mais escalonamentos possíveis
}

// Função para exibir a matriz de soluções
void mostrarMatriz(JobShop *job_shop) {
    printf("\nMatriz de Solucoes:\n");
    for (int i = 0; i < NUM_TRABALHOS; i++) {
        printf("Trabalho %d:\n", i);
        for (int j = 0; j < NUM_OPERACOES_MAX; j++) {
            printf("  Operacao %d: Maquina %d, Duracao %d\n", j, job_shop->trabalhos[i].operacoes[j].maquina, job_shop->trabalhos[i].operacoes[j].duracao);
        }
    }
    printf("\n");
}

int main() {
    // Inicializar o gerador de números aleatórios
    srand(time(NULL));

    // Criar e inicializar o problema do Job-Shop
    JobShop job_shop;
    inicializarJobShopExemplo(&job_shop);

    // Exibir o problema do Job-Shop (apenas para fins de depuração)
    mostrarJobShop(&job_shop);

    // Calcular e exibir todas as possíveis soluções e o tempo total de conclusão de cada uma
    printf("Todas as possiveis solucoes e seus tempos totais de conclusao:\n");
    clock_t start = clock();
    int num_solucoes = 0;
    do {
        printf("Solucao %d:\n", num_solucoes + 1);
        mostrarMatriz(&job_shop);
        int tempo_total = calcularTempoTotal(&job_shop);
        printf("Tempo total de conclusao: %d\n", tempo_total);
        printf("-------------------------\n");
        num_solucoes++;
    } while (proximoEscalonamento(&job_shop));
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tempo decorrido: %f segundos\n", time_spent);

    return 0;
}
