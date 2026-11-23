/*
 * scheduler.h
 *
 *  Created on: Nov 6, 2026
 *      Author: Duy Khanh
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define SCH_MAX_TASKS   40
#define NO_TASK_ID      0
#define TICK_MS         10
#define INVALID_ID 		255
typedef uint32_t TaskId;
extern uint32_t sch_time_ms;
void SCH_Update(void);
void SCH_Init();
TaskId SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Dispatch_Tasks(void);
bool SCH_Delete_Task(TaskId taskID);
uint32_t SCH_Gettime();


#endif /* INC_SCHEDULER_H_ */
