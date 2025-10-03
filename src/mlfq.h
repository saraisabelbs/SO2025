#ifndef MLFQ_H
#define MLFQ_H

#include <stdint.h>
#include "queue.h"

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif
