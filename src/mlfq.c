#include "mlfq.h"
#include "fifo.h"   // para usar fifo_scheduler
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    // Implementação provisória: usa FIFO como fallback
    fifo_scheduler(current_time_ms, rq, cpu_task);
}
