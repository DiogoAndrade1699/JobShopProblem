#include <stdio.h>
#include <time.h>

#define MAX_JOBS 3
#define MAX_MACHINES 3
#define OPERATIONS_PER_JOB 3
#define MAX_OPERATIONS (MAX_JOBS * OPERATIONS_PER_JOB)

// Função para calcular os starts e mostrar os resultados
void calcularStarts(int matriz[MAX_JOBS][OPERATIONS_PER_JOB * 2]) {
    int starts[MAX_JOBS][MAX_MACHINES] = {0};
    int max_conclusao = 0;
    int maquina_livre[MAX_MACHINES] = {0}; // Armazena o tempo de término da última operação em cada máquina

    printf("Job\tMachine\tStart\n");

    // Registrar o tempo de início
    clock_t start_time = clock();

    for (int i = 0; i < MAX_JOBS; i++) {
        int start_anterior = 0;
        for (int j = 0; j < OPERATIONS_PER_JOB * 2; j += 2) {
            int machine = matriz[i][j];
            int duration = matriz[i][j + 1];

            // Verifica se a máquina está livre
            if (maquina_livre[machine] > start_anterior) {
                start_anterior = maquina_livre[machine];
            }

            int start = start_anterior;
            starts[i][machine] = start + duration;

            printf("%d\t%d\t%d\n", i, machine, start);

            maquina_livre[machine] = starts[i][machine]; // Atualiza o tempo de término da máquina

            start_anterior = starts[i][machine]; // Atualiza o start_anterior para a próxima operação (qualquer máquina)
        }

        // Atualiza o tempo máximo de conclusão
        if (starts[i][MAX_MACHINES - 1] > max_conclusao) {
            max_conclusao = starts[i][MAX_MACHINES - 1];
        }
    }

    // Registrar o tempo de fim
    clock_t end_time = clock();

    // Calcular o tempo decorrido em segundos
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Calcular o tempo máximo de conclusão  somando com o tempo de duração da última máquina
    max_conclusao += matriz[MAX_JOBS - 1][OPERATIONS_PER_JOB * 2 - 1];

    printf("Tempo maximo de conclusao: %d\n", max_conclusao);
    printf("Tempo decorrido: %.6f segundos\n", elapsed_time);

    // Salvar os resultados em um arquivo
    FILE *file = fopen("resultados.txt", "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "Job\tMachine\tStart\n");
    for (int i = 0; i < MAX_JOBS; i++) {
        for (int j = 0; j < MAX_MACHINES; j++) {
            fprintf(file, "%d\t%d\t%d\n", i, j, starts[i][j]);
        }
    }

    fprintf(file, "Tempo maximo de conclusao: %d\n", max_conclusao);
    fprintf(file, "Tempo decorrido: %.6f segundos\n", elapsed_time);

    fclose(file);

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

    calcularStarts(matriz);
    return 0;
}
