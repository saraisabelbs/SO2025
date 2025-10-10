#include "rr.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>

#define QUANTUM_MS 500

//help
//Cada processo roda no CPU no máximo 500ms (TICKS_MS já é o "tick", tu tens de contar).
//Se não acabou, volta para o fim da fila.
void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    static uint32_t slice = 0; // Contador do tempo já gasto pelo processo atual no quantum

    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS; // Incrementa o tempo total gasto pelo processo
        slice += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            // O processo terminou
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t)); // Notifica o fim
            free(*cpu_task);   // Liberta memória
            *cpu_task = NULL;  // Liberta CPU
            slice = 0;         // Reinicia o contador de quantum
        }
        else if (slice >= QUANTUM_MS) {
            // O quantum acabou mas o processo não terminou → volta ao fim da fila
            enqueue_pcb(rq, *cpu_task);
            *cpu_task = NULL;
            slice = 0;
        }
    }

    if (*cpu_task == NULL) {
        *cpu_task = dequeue_pcb(rq); // Tira o próximo processo da fila para a CPU

    }

}
