#include "sjf.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>


//Se o CPU está ocupado → deixa acabar.
//está livre → escolhe da fila o processo com menor time_ms.
void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            free((*cpu_task));
            (*cpu_task) = NULL;
        }
    }
    if (*cpu_task == NULL) {
        // Escolher o processo mais curto da fila
        pcb_t *shortest = NULL, *prev = NULL, *curr = rq->head, *prev_shortest = NULL;

        while (curr) {
            if (!shortest || curr->time_ms < shortest->time_ms) {
                shortest = curr;
                prev_shortest = prev;
            }
            prev = curr;
            curr = curr->next;
        }

        if (shortest) {
            // Remover da fila
            if (prev_shortest) prev_shortest->next = shortest->next;
            else rq->head = shortest->next;

            shortest->next = NULL;
            *cpu_task = shortest;
        }
    }
}
