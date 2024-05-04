#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

pthread_mutex_t *machine_mutex; // Mutex para cada máquina
pthread_mutex_t max_conclusao_mutex; // Mutex para a variável max_conclusao

int **matriz; // Matriz de jobs e operações
int **starts; // Matriz para armazenar os starts
int *maquina_livre; // Armazena o tempo de término da última operação em cada máquina
int num_jobs = 0;
int max_machines = 0;
int operations_per_job = 0;

// Estrutura para passar argumentos para a função das threads
typedef struct {
    int start_index; // Índice inicial do loop externo
    int end_index;   // Índice final do loop externo
} ThreadArgs;

// Função para calcular os starts em uma faixa específica de índices do loop externo
void *calcularStartsThread(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;

    for (int i = thread_args->start_index; i < thread_args->end_index; i++) {
        int start_anterior = 0;
        for (int j = 0; j < operations_per_job * 2; j += 2) {
            int machine = matriz[i][j];
            int duration = matriz[i][j + 1];

            // Bloqueia o acesso às variáveis compartilhadas
            pthread_mutex_lock(&machine_mutex[machine]);
            pthread_mutex_lock(&max_conclusao_mutex);

            // Verifica se a máquina está livre
            if (maquina_livre[machine] > start_anterior) {
                start_anterior = maquina_livre[machine];
            }

            int start = start_anterior;
            starts[i][machine] = start + duration;

            printf("%d\t%d\t%d\n", i, machine, start);

            maquina_livre[machine] = starts[i][machine];
            start_anterior = starts[i][machine];

            // Libera os mutexes
            pthread_mutex_unlock(&max_conclusao_mutex);
            pthread_mutex_unlock(&machine_mutex[machine]);
        }
    }

    pthread_exit(NULL);
}

// Função para calcular os starts e mostrar os resultados
void calcularStarts(int num_threads, const char *output_file) {
    
    //Abrir o arquivo de saída
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return;
    }

    printf("Job\tMachine\tStart\n");

    // Registrar o tempo de início
    clock_t start_time = clock();

    // Inicializar os mutexes para cada máquina
    machine_mutex = malloc(sizeof(pthread_mutex_t) * max_machines);
    for (int i = 0; i < max_machines; i++) {
        pthread_mutex_init(&machine_mutex[i], NULL);
    }
    pthread_mutex_init(&max_conclusao_mutex, NULL);

    // Alocar memória para starts
    starts = malloc(sizeof(int *) * num_jobs);
    for (int i = 0; i < num_jobs; i++) {
        starts[i] = malloc(sizeof(int) * max_machines);
    }

    // Alocar memória para maquina_livre
    maquina_livre = malloc(sizeof(int) * max_machines);

    // Inicializar maquina_livre
    for (int i = 0; i < max_machines; i++) {
        maquina_livre[i] = 0;
    }

    // Criar threads
    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    // Dividir o trabalho entre as threads
    int chunk_size = num_jobs / num_threads; // Tamanho do pedaço de trabalho para cada thread
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].start_index = i * chunk_size;
        thread_args[i].end_index = (i == num_threads - 1) ? num_jobs : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, calcularStartsThread, (void *)&thread_args[i]);
    }

    // Aguardar o término das threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Determinar o tempo máximo de conclusão
    int max_conclusao = 0;
    for (int i = 0; i < max_machines; i++) {
        if (maquina_livre[i] > max_conclusao) {
            max_conclusao = maquina_livre[i];
        }
    }

    // Liberar a memória alocada para os mutexes
    for (int i = 0; i < max_machines; i++) {
        pthread_mutex_destroy(&machine_mutex[i]);
    }
    free(machine_mutex);

    // Liberar o mutex para max_conclusao
    pthread_mutex_destroy(&max_conclusao_mutex);

    // Escrever os resultados no arquivo de saída
    fprintf(file, "Job\tMachine\tStart\n");
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < max_machines; j++) {
            fprintf(file, "%d\t%d\t%d\n", i, j, starts[i][j]);
        }
    }

    // Liberar a memória alocada para starts
    for (int i = 0; i < num_jobs; i++) {
        free(starts[i]);
    }
    free(starts);

    // Liberar a memória alocada para maquina_livre
    free(maquina_livre);

    // Registrar o tempo de fim
    clock_t end_time = clock();

    // Calcular o tempo decorrido em segundos
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    fprintf(file, "Tempo maximo de conclusao: %d\n", max_conclusao);

    fclose(file);

    printf("Tempo decorrido: %.6f segundos\n", elapsed_time);
}

int main() {
    char input_file[100];
    char output_file[100];

    // Solicitar o nome do arquivo de entrada
    printf("Digite o nome do arquivo de entrada: ");
    scanf("%s", input_file);

    // Solicitar o nome do arquivo de saída
    printf("Digite o nome do arquivo de saida: ");
    scanf("%s", output_file);

    // Leitura da matriz a partir de um arquivo
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    int nmachines = 0;
    int numjobs = 0;

    // Lê o arquivo matriz.txt
    fscanf(file, "%d %d", &nmachines, &numjobs);

    // Definir variáveis globais
    max_machines = nmachines;
    num_jobs = numjobs;
    operations_per_job = numjobs;

    // Alocar memória para matriz
    matriz = malloc(sizeof(int *) * num_jobs);
    for (int i = 0; i < num_jobs; i++) {
        matriz[i] = malloc(sizeof(int) * operations_per_job * 2);
    }

    // Lê os números do arquivo matriz.txt e os armazena na matriz matriz, linha por linha, até que o fim do ficheiro seja alcançado ou até que o limite máximo de linhas (MAX_JOBS) seja atingido.
    // Preencher a matriz
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < operations_per_job * 2; j++) {
            fscanf(file, "%d", &matriz[i][j]);
        }
    }

    fclose(file);

    printf("Numero de linhas no arquivo: %d\n", num_jobs);

    // Número de threads a serem usadas
    int num_threads;
    printf("Digite o numero de threads (entre 1 e %d): ", num_jobs);
    scanf("%d", &num_threads);
    if (num_threads < 1 || num_threads > num_jobs) {
        printf("Numero invalido de threads. Deve ser um numero entre 1 e %d.\n", num_jobs);
        return 1;
    }

    calcularStarts(num_threads, output_file);

    // Liberar a memória alocada para matriz
    for (int i = 0; i < num_jobs; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
