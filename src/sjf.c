#include "sjf.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>

// Algoritmo Shortest Job First (não preemptivo)
void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            // Task terminou
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            free(*cpu_task);
            *cpu_task = NULL;
        }
    }

    // Se a CPU estiver livre, escolher o job mais curto da fila
    if (*cpu_task == NULL && rq->head) {
        queue_elem_t *curr = rq->head;
        queue_elem_t *prev = NULL;

        queue_elem_t *shortest_prev = NULL;
        queue_elem_t *shortest_elem = rq->head;

        while (curr) {
            if (curr->pcb->time_ms < shortest_elem->pcb->time_ms) {
                shortest_elem = curr;
                shortest_prev = prev;
            }
            prev = curr;
            curr = curr->next;
        }

        // Retira o mais curto da fila
        if (shortest_prev) {
            shortest_prev->next = shortest_elem->next;
        } else {
            rq->head = shortest_elem->next;
        }

        shortest_elem->next = NULL;
        *cpu_task = shortest_elem->pcb;
        free(shortest_elem); // libertar o wrapper da queue
    }
}
