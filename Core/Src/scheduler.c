/*
 * scheduler.c
 *
 *  Created on: Nov 6, 2026
 *      Author: Duy Khanh
 */
#include <scheduler.h>


uint32_t sch_time_ms =0;
typedef struct sTask{
    void   (*pTask)(void);
    uint32_t Delay;
    uint32_t Period;
    uint8_t  RunMe;
    TaskId TaskID;
    struct sTask* next;
} sTask;


static sTask SCH_Task_Pool[SCH_MAX_TASKS];
sTask* pFreeHead=NULL;
sTask* pActiveHead=NULL;


static uint8_t  SCH_count = 0;

uint32_t SCH_Gettime(){
	return sch_time_ms;
}
void SCH_Init(void) {
    pActiveHead=NULL;
    pFreeHead =&SCH_Task_Pool[0];


    for (uint8_t i = 0; i < SCH_MAX_TASKS - 1; i++) {
        SCH_Task_Pool[i].TaskID = i;
        SCH_Task_Pool[i].next = &SCH_Task_Pool[i + 1];
    }
    SCH_Task_Pool[SCH_MAX_TASKS - 1].TaskID = SCH_MAX_TASKS - 1;
    SCH_Task_Pool[SCH_MAX_TASKS - 1].next = NULL;
}
static void SCH_Insert_Active_List(sTask *pNewTask, uint32_t delayTicks){
	pNewTask->RunMe = 0;

	if(pActiveHead==NULL){
		pNewTask->Delay = delayTicks;
		pNewTask->next = NULL;
		pActiveHead= pNewTask;
		return;
	}
	sTask* curr=pActiveHead;
	sTask* Pev=NULL;
	while (curr != NULL && delayTicks >= curr->Delay) {
	        delayTicks -= curr->Delay;
	        Pev = curr;
	        curr = curr->next;
	 }
	pNewTask->Delay = delayTicks;
	pNewTask->next = curr;
	if (curr != NULL) {
	        curr->Delay -= delayTicks;
	}

	if (Pev == NULL) {
	        pActiveHead = pNewTask;
	} else {
	        Pev->next = pNewTask;
	}
}
TaskId SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	if(pFunction==NULL || pFreeHead==NULL) return INVALID_ID;

	sTask* newNode = pFreeHead;
	pFreeHead = pFreeHead->next;

	newNode->Period=PERIOD/TICK_MS;
	newNode->pTask=pFunction;
	uint32_t delayms = DELAY/TICK_MS;
	SCH_Insert_Active_List(newNode, delayms);

	SCH_count++;
	return newNode->TaskID;
}

bool SCH_Delete_Task(TaskId taskID){
	if(pActiveHead==NULL || taskID >= SCH_MAX_TASKS) return false;

	sTask* curr = pActiveHead;
	sTask* Prev = NULL;

	while(curr!=NULL && curr->TaskID!=taskID){
		Prev = curr;
		curr=curr->next;
	}
	if(curr==NULL){
		return false;
	}

	if (curr->next != NULL) {
			sTask* NodeNext =curr->next;
	        NodeNext->Delay += curr->Delay;
	}
	if (Prev == NULL) {
	        pActiveHead = curr->next;
	    } else {
	        Prev->next = curr->next;
	    }

	    // Trả node về Free List (O(1))
	    curr->next = pFreeHead;
	    pFreeHead = curr;

	    return true;
}

void SCH_Update(void) {
	sch_time_ms ++;
	if (pActiveHead != NULL) {
        if (pActiveHead->Delay > 0) {
            pActiveHead->Delay--;
        }
        if (pActiveHead->Delay == 0) {
            pActiveHead->RunMe = 1;
        }
    }
}
void SCH_Dispatch_Tasks(void) {
    if (pActiveHead != NULL && pActiveHead->RunMe > 0) {
        sTask *pTaskToRun = pActiveHead;


        (*pTaskToRun->pTask)();


        pActiveHead = pActiveHead->next;


        if (pTaskToRun->Period > 0) {
            // Task định kỳ: Tái chèn lại vào Active List với Delay = Period
            SCH_Insert_Active_List(pTaskToRun, pTaskToRun->Period);
        } else {
            // Task chạy 1 lần: Thu hồi node trả về Free List
            pTaskToRun->next = pFreeHead;
            pFreeHead = pTaskToRun;
            SCH_count--;
        }
    }
}
