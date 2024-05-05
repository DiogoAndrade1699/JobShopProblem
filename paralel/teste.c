#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MAX_JOBS 3
#define MAX_MACHINES 3
#define OPERATIONS_PER_JOB 3
#define MAX_OPERATIONS (MAX_JOBS * OPERATIONS_PER_JOB)
#define NUM_THREADS 3

typedef struct {
    int start_job;
    int end_job;
    int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2];
} ThreadArgs;

void *calcularStartsThread(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;

    int starts[MAX_JOBS][MAX_MACHINES] = {0};
    int max_conclusao = 0;
    int maquina_livre[MAX_MACHINES] = {0};

    for (int i = thread_args->start_job; i < thread_args->end_job; i++) {
        int start_job = 0; // Tempo de início do job

        for (int j = 0; j < OPERATIONS_PER_JOB * 2; j += 2) {
            int machine = thread_args->matriz[i][j];
            int duration = thread_args->matriz[i][j + 1];

            // Tempo de espera entre operações na mesma máquina
            int espera = maquina_livre[machine] > start_job ? maquina_livre[machine] - start_job : 0;
            start_job += espera; // Incrementa o tempo de início pelo tempo de espera
            int start = start_job;

            // Atualiza o tempo de início real da operação na máquina
            starts[i][machine] = start;

            // Atualiza o tempo de término da operação na máquina
            maquina_livre[machine] = start + duration;

            start_job += duration; // Atualiza o tempo de início para a próxima operação
        }

        // Atualiza o tempo máximo de conclusão
        int conclusao_job = maquina_livre[MAX_MACHINES - 1]; // Tempo de conclusão do job
        if (conclusao_job > max_conclusao) {
            max_conclusao = conclusao_job;
        }
    }

    // Retornar os resultados em um ponteiro para int (int *)
    int *results = malloc(sizeof(int) * 2);
    results[0] = max_conclusao;
    results[1] = thread_args->end_job - thread_args->start_job; // Quantidade de jobs processados
    return results;
}


int main() {
    int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2];
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];
    int max_conclusao = 0;

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

    // Iniciar as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start_job = i * (MAX_JOBS / NUM_THREADS);
        args[i].end_job = (i + 1) * (MAX_JOBS / NUM_THREADS);
        for (int j = 0; j < MAX_JOBS; j++) {
            for (int k = 0; k < OPERATIONS_PER_JOB * 2; k++) {
                args[i].matriz[j][k] = matriz[j][k];
            }
        }
        pthread_create(&threads[i], NULL, calcularStartsThread, &args[i]);
    }

    // Aguardar a conclusão das threads
    for (int i = 0; i < NUM_THREADS; i++) {
        void *thread_result;
        pthread_join(threads[i], &thread_result);
        int *results = (int *)thread_result;
        if (results[0] > max_conclusao) {
            max_conclusao = results[0];
        }
        free(results); // Liberar memória alocada pela thread
    }

    printf("Tempo maximo de conclusao: %d\n", max_conclusao);

    return 0;
}
