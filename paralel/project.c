#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define MAX_JOBS 3
#define MAX_MACHINES 3
#define OPERATIONS_PER_JOB 3
#define MAX_OPERATIONS (MAX_JOBS * OPERATIONS_PER_JOB)

pthread_mutex_t mutex; // Mutex para sincronização

// Estrutura para passar argumentos para a função das threads
typedef struct {
    int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2];
    int start_index; // Índice inicial do loop externo
    int end_index;   // Índice final do loop externo
    int starts[MAX_JOBS][MAX_MACHINES];
    int *max_conclusao;
    int maquina_livre[MAX_MACHINES];
} ThreadArgs;

// Função para calcular os starts em uma faixa específica de índices do loop externo
void *calcularStartsThread(void *thread_args) {
    ThreadArgs *args = (ThreadArgs *)thread_args;

    for (int i = args->start_index; i < args->end_index; i++) {
        int start_anterior = 0;
        for (int j = 0; j < OPERATIONS_PER_JOB * 2; j += 2) {
            int machine = args->matriz[i][j];
            int duration = args->matriz[i][j + 1];

            // Bloqueia o acesso às variáveis compartilhadas
            pthread_mutex_lock(&mutex);

            // Verifica se a máquina está livre
            if (args->maquina_livre[machine] > start_anterior) {
                start_anterior = args->maquina_livre[machine];
            }

            int start = start_anterior;
            args->starts[i][machine] = start + duration;

            printf("%d\t%d\t%d\n", i, machine, start);

            args->maquina_livre[machine] = args->starts[i][machine];

            start_anterior = args->starts[i][machine];

            // Atualiza o tempo máximo de conclusão
            if (args->starts[i][MAX_MACHINES - 1] > *(args->max_conclusao)) {
                *(args->max_conclusao) = args->starts[i][MAX_MACHINES - 1];
            }

            // Libera o mutex
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_exit(NULL);
}

// Função para calcular os starts e mostrar os resultados
void calcularStarts(int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2], int num_threads) {
    int starts[MAX_JOBS][MAX_MACHINES] = {0};
    int max_conclusao = 0;
    int maquina_livre[MAX_MACHINES] = {0}; // Armazena o tempo de término da última operação em cada máquina

    printf("Job\tMachine\tStart\n");

    // Registrar o tempo de início
    clock_t start_time = clock();

    // Inicializar o mutex
    pthread_mutex_init(&mutex, NULL);

    // Criar threads
    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    // Dividir o trabalho entre as threads
    int chunk_size = MAX_JOBS / num_threads; // Dividir o loop externo igualmente entre as threads
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].start_index = i * chunk_size;
        thread_args[i].end_index = (i + 1) * chunk_size;
        thread_args[i].max_conclusao = &max_conclusao;
        for (int j = 0; j < MAX_MACHINES; j++) {
            thread_args[i].maquina_livre[j] = maquina_livre[j];
        }
        for (int j = 0; j < MAX_JOBS; j++) {
            for (int k = 0; k < MAX_MACHINES; k++) {
                thread_args[i].starts[j][k] = starts[j][k];
            }
        }
        for (int j = 0; j < MAX_JOBS; j++) {
            for (int k = 0; k < OPERATIONS_PER_JOB * 2; k++) {
                thread_args[i].matriz[j][k] = matriz[j][k];
            }
        }
        pthread_create(&threads[i], NULL, calcularStartsThread, (void *)&thread_args[i]);
    }

    // Aguardar o término das threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberar o mutex
    pthread_mutex_destroy(&mutex);

    // Registrar o tempo de fim
    clock_t end_time = clock();

    // Calcular o tempo decorrido em segundos
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Calcular o tempo máximo de conclusão somando com o tempo de duração da última máquina
    max_conclusao += matriz[MAX_JOBS - 1][OPERATIONS_PER_JOB * 2 - 1];

    printf("Tempo maximo de conclusao: %d\n", max_conclusao);
    printf("Tempo decorrido: %.6f segundos\n", elapsed_time);
}

int main() {
    int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2];

    // Leitura da matriz a partir de um arquivo
    FILE *file = fopen("matriz.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    printf("Matriz:\n");
    for (int i = 0; i < MAX_JOBS; i++) {
        for (int j = 0; j < OPERATIONS_PER_JOB * 2; j++) {
            fscanf(file, "%d", &matriz[i][j]);
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }

    fclose(file);

    // Número de threads a serem usadas
    int num_threads;
    printf("Digite o numero de threads (entre 1 e %d): ", MAX_JOBS);
    scanf("%d", &num_threads);
    if (num_threads < 1 || num_threads > MAX_JOBS) {
        printf("Número inválido de threads. Deve ser um número entre 1 e %d.\n", MAX_JOBS);
        return 1;
    }

    calcularStarts(matriz, num_threads);
    return 0;
}
