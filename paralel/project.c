#include <stdio.h>
#include <omp.h> // Incluir a biblioteca OpenMP

#define MAX_JOBS 3
#define MAX_MACHINES 3
#define MAX_OPERATIONS 6

// Função para calcular os starts e mostrar os resultados
void calcularStarts(int matriz[MAX_JOBS][MAX_OPERATIONS]) {
    int starts[MAX_JOBS][MAX_MACHINES] = {0};
    int max_conclusao = 0;
    int maquina_livre[MAX_MACHINES] = {0}; // Armazena o tempo de término da última operação em cada máquina

    printf("Job\tMachine\tStart\n");

    #pragma omp parallel for // Paralelizar o loop principal dos jobs
    for (int i = 0; i < MAX_JOBS; i++) {
        int start_anterior = 0;
        for (int j = 0; j < MAX_OPERATIONS; j += 2) {
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

    printf("Tempo maximo de conclusao: %d + 1 = %d\n", max_conclusao, max_conclusao + 1);
}

int main() {
    int matriz[MAX_JOBS][MAX_OPERATIONS];

    // Leitura da matriz a partir de um arquivo
    FILE *file = fopen("matriz.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    printf("Matriz:\n");
    for (int i = 0; i < MAX_JOBS; i++) {
        for (int j = 0; j < MAX_OPERATIONS; j++) {
            fscanf(file, "%d", &matriz[i][j]);
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }

    fclose(file);

    calcularStarts(matriz);
    return 0;
}