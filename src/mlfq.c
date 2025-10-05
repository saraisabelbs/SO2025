#include "mlfq.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>

#define NUM_QUEUES 3     // três níveis de prioridade
#define QUANTUM1 500    // 500 ms em todas as filas
#define QUANTUM2 500
#define QUANTUM3 500

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    static int current_queue = 0; // nível de prioridade atual
    static uint32_t quantum_time = 0; // tempo decorrido no quantum atual

    // Atualiza o tempo total e o tempo dentro do quantum atual.
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        quantum_time += TICKS_MS;

        uint32_t total_time = (*cpu_task)->time_ms;
        uint32_t elapsed = (*cpu_task)->ellapsed_time_ms;

        // Termina o processo se acabou o tempo
        if (elapsed >= total_time) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t))
                perror("write");

            printf("[MLFQ DEBUG] Processo %d terminou na fila %d.\n", (*cpu_task)->pid, current_queue);

            free(*cpu_task);
            *cpu_task = NULL;
            quantum_time = 0;
        }
        // Se o quantum acabou mas o processo ainda não terminou, baixa de fila
        else {
            uint32_t current_quantum = (current_queue == 0) ? QUANTUM1 :
                                       (current_queue == 1) ? QUANTUM2 : QUANTUM3;
            if (quantum_time >= current_quantum) {
                printf("[MLFQ DEBUG] Quantum acabou para processo %d (tempo %d/%d). Desce para fila %d.\n",
                       (*cpu_task)->pid, elapsed, total_time, current_queue + 1);

                enqueue_pcb(rq, *cpu_task);
                *cpu_task = NULL;
                quantum_time = 0;

                if (current_queue < NUM_QUEUES - 1)
                    current_queue++;
            }
        }
    }

    // Se CPU está livre, escolhe o próximo processo
    if (*cpu_task == NULL) {
        *cpu_task = dequeue_pcb(rq);
        if (*cpu_task != NULL) {
            printf("[MLFQ DEBUG] Processo %d iniciou execução na fila %d.\n",
                   (*cpu_task)->pid, current_queue);
        }
    }
}
