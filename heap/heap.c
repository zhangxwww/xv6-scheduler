#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "heap.h"

// Array to store the process heap data
struct proc *procHeap[64];
int heapSize = 0;

void initHeap() {
    for (int i = 0; i < 64; i++) {
        procHeap[i] = 0;
    }
}

void siftdown(startPos, pos) {
    newItem = procHeap[pos];
    while (pos > startpos) {
        parentPos = (pos - 1) >> 1;
        parent = heap[parentPos];
        if (newItem->priority < parent.priority) {
            procHeap[pos] = parent;
            pos = parentPos;
            continue;
        }
        break;
    }
    procHeap[pos] = newItem
}

void siftup(pos) {
    endPos = heapSize;
    startPos = pos;
    newItem = procHeap[pos];
    childPos = 2 * pos + 1;
    while (childPos < endPos) {
        rightPos = childPos + 1;
        if ((rightPos < endPos) && !(heap[childPos] < heap[rightPos])) {
            childPos = rightPos;
        }
        procHeap[pos] = procHeap[childPos];
        pos = childPos;
        childPos = 2 * pos + 1;
    }
    procHeap[pos] = newItem;
    siftdown(startPos, pos);
}

// return the size of the heap
// if successfully insert the new data
// else return -1
int heappush(struct proc *item) {
    if (heapSize < MAX_HEAP_LENGTH) {
        procHeap[++heapSize] = item;
        siftdown(0, heapSize - 1);
        return heapSize;
    }
    else {
        return -1;
    }
}

// return the minimum element
// meanwhile deleting it from the heap
// if the heap is empty return NULL(0)
struct proc* heappop() {
    if (heapSize == 0) {
        return 0;
    }
    if (heapSize == 1) {
        root = procHeap[0];
        heap[0] = 0;
        heapSize = 0;
        return root;
    }
    last = procHeap[heapSize - 1];
    root = procHeap[0];
    procHeap[0] = last;
    procHeap[heapSize--] = 0;
    siftup(0);
    return item;
}

