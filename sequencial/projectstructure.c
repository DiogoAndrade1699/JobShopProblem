#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_JOBS 100
#define MAX_MACHINES 100
#define MAX_OPERATIONS (MAX_JOBS * MAX_MACHINES)

int nmachines = 0;

// Função para calcular os starts e mostrar os resultados
void calcularStarts(int matriz[MAX_JOBS][MAX_MACHINES * 2], int num_jobs, int operations_per_job, const char *output_file_name) {
    int starts[MAX_JOBS][MAX_MACHINES] = {0};
    int max_conclusao = 0;
    int maquina_livre[MAX_MACHINES] = {0}; // Armazena o tempo de término da última operação em cada máquina

    // guardar os resultados em um ficheiro
    FILE *file = fopen(output_file_name, "w");
    if (file == NULL) {
        printf("Erro ao abrir o ficheiro.\n");
        return;
    }

    clock_t start_time = clock();

    fprintf(file, "Job\tMachine\tStart\n");

    

    for (int i = 0; i < num_jobs; i++) {
        int start_anterior = 0;
        for (int j = 0; j < operations_per_job * 2; j += 2) {
            int machine = matriz[i][j];
            clock_t midtime = clock();
            //printf midtime
            //printf("Tempo de mid: %ld\n", midtime);
            int duration = matriz[i][j + 1];

            // Verifica se a máquina está livre
            if (maquina_livre[machine] > start_anterior) {
                start_anterior = maquina_livre[machine];
            }

            int start = start_anterior;
            starts[i][machine] = start + duration;

            fprintf(file, "%d\t%d\t%d\n", i, machine, start);

            maquina_livre[machine] = starts[i][machine]; // Atualiza o tempo de término da máquina

            start_anterior = starts[i][machine]; // Atualiza o start_anterior para a próxima operação (qualquer máquina)
        }

        // Atualiza o tempo máximo de conclusão
        if (starts[i][operations_per_job - 1] > max_conclusao) {
            max_conclusao = starts[i][operations_per_job - 1];
        }

         // Calcular o tempo máximo de conclusão somado com o tempo de duração da última máquina
        max_conclusao += matriz[num_jobs - 1][operations_per_job * 2 - 1];
    }

    fprintf(file, "Tempo maximo de conclusao: %d\n", max_conclusao);


    fclose(file);

    // Registrar o tempo de fim
    clock_t end_time = clock();

    // Calcular o tempo decorrido em segundos
    double elapsed_time = difftime(end_time, start_time) / CLOCKS_PER_SEC;

    printf("Tempo decorrido: %.6f segundos\n", elapsed_time);


}

int main() {
    int matriz[MAX_JOBS][MAX_MACHINES * 2];
    char input_file_name[100];
    char output_file_name[100];
    int num_jobs, num_machines;
    int operations_per_job = 0;

    // Solicitar o nome do ficheiro de entrada ao utilizador
    printf("Digite o nome do ficheiro de entrada: ");
    scanf("%s", input_file_name);

    // Leitura da matriz a partir do ficheiro de entrada
    FILE *file = fopen(input_file_name, "r");
    if (file == NULL) {
        printf("Erro ao abrir o ficheiro.\n");
        return 1;
    }

    // Lendo o número de máquinas e trabalhos
    fscanf(file, "%d %d", &num_machines, &num_jobs);

    operations_per_job= num_machines;
    nmachines = num_machines;

    printf("Matriz:\n");
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < nmachines * 2; j++) {
            fscanf(file, "%d", &matriz[i][j]);
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }

    fclose(file);

    // Solicitar o nome do ficheiro de saída ao utilizador
    printf("Digite o nome do ficheiro de saida: ");
    scanf("%s", output_file_name);

    calcularStarts(matriz, num_jobs, nmachines, output_file_name);
    return 0;
}
