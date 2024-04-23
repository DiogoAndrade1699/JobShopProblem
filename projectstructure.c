#include <stdio.h>

#define NUM_JOBS 3
#define NUM_MACHINES 3

int main() {
    // Definir a matriz de tempos de processamento para cada trabalho
    int processing_times[NUM_JOBS][NUM_MACHINES][2] = {
        {{0, 3}, {1, 2}, {2, 2}},
        {{0, 2}, {2, 1}, {1, 4}},
        {{1, 4}, {2, 3}, {0, 1}}
    };

    // Imprimir os tempos de processamento de cada máquina para cada trabalho
    printf("Tempos de processamento de cada máquina para cada trabalho:\n");
    for (int i = 0; i < NUM_JOBS; i++) {
        printf("J%d: ", i);
        for (int j = 0; j < NUM_MACHINES; j++) {
            printf("Machine%d duracao %d", processing_times[i][j][0], processing_times[i][j][1]);
            if (j < NUM_MACHINES - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }

    // Calcular o somatório dos tempos de duração de cada máquina para todos os trabalhos
    int machine_sums[NUM_MACHINES] = {0};

    for (int i = 0; i < NUM_JOBS; i++) {
        for (int j = 0; j < NUM_MACHINES; j++) {
            machine_sums[processing_times[i][j][0]] += processing_times[i][j][1];
        }
    }

    // Imprimir os somatórios do tempo de duração de cada máquina para todos os trabalhos
    printf("\nSomatorios do tempo de duracao de cada maquina para todos os trabalhos:\n");
    for (int i = 0; i < NUM_MACHINES; i++) {
        printf("Machine%d total duration: %d\n", i, machine_sums[i]);
    }

     // Encontrar o valor máximo do somatório
    int max_sum = machine_sums[0];
    int max_machine = 0;
    for (int i = 1; i < NUM_MACHINES; i++) {
        if (machine_sums[i] > max_sum) {
            max_sum = machine_sums[i];
            max_machine = i;
        }
    }
    printf("\nValor maximo do somatorio: %d\n", max_sum);

    printf("A maquina com o maior somatorio e a Machine%d com um tempo total de %d.\n\n", max_machine, max_sum);




    
}
