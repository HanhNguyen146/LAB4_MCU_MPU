/*
 * scheduler.c
 *
 *  Created on: Nov 14, 2025
 *      Author: hanh
 */

#include "scheduler.h"

int TIME_CYCLE; // Time cycle

sTasks heap[SCH_MAX_TASKS];
uint8_t heap_size = 0;
static uint32_t nextTaskID = 0;

void swap(sTasks *a, sTasks *b) {
    sTasks t = *a;
    *a = *b;
    *b = t;
}

void heapify_up(int i) {
    while(i > 0) {
        int parent = (i - 1) / 2;
        if(heap[i].Delay < heap[parent].Delay) {
            swap(&heap[i], &heap[parent]);
            i = parent;
        } else break;
    }
}

void heapify_down(int i) {
    while(1) {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int smallest = i;

        if(left < heap_size && heap[left].Delay < heap[smallest].Delay)
            smallest = left;
        if(right < heap_size && heap[right].Delay < heap[smallest].Delay)
            smallest = right;

        if(smallest != i) {
            swap(&heap[i], &heap[smallest]);
            i = smallest;
        } else break;
    }
}

void heap_push(sTasks t) {
    heap[heap_size] = t;
    heapify_up(heap_size);
    heap_size++;
}

sTasks heap_pop() {
    sTasks top = heap[0];
    heap_size--;

    if (heap_size > 0) {
        heap[0] = heap[heap_size];
        heapify_down(0);
    }

    return top;
}

void SCH_Init(void){
    heap_size = 0;
}

void SCH_Update(void) {

    if (heap_size == 0) return;

    sTasks t = heap_pop();

    if (t.Delay > 0)
        t.Delay--;

    if (t.Delay == 0) {
        t.RunMe = 1;
        t.Delay = t.Period;   // periodic
    }

    heap_push(t);
}

void SCH_Dispatch_Tasks(void) {

    if (heap_size == 0) return;

    if (heap[0].RunMe > 0) {
        heap[0].RunMe--;
        heap[0].pTask();
    }
}

uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {

    if (heap_size >= SCH_MAX_TASKS)
        return -1;

    sTasks t;
    t.pTask  = pFunction;
    t.Delay  = DELAY / TIME_CYCLE;
    t.Period = PERIOD / TIME_CYCLE;
    t.RunMe  = 0;
    t.TaskID = nextTaskID++;

    heap_push(t);
    return t.TaskID;
}


uint8_t SCH_Delete_Task(uint32_t taskID) {

    if (heap_size == 0) return -1;

    for (int i = 0; i < heap_size; i++) {
        if (heap[i].TaskID == taskID) {

            heap[i] = heap[heap_size - 1];
            heap_size--;
            heapify_down(i);
            heapify_up(i);
            return taskID;
        }
    }
    return -1;
}
