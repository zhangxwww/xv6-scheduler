#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

/* --------------------------------------------------- */
// definitions of a Red-Black Tree(RBT)
/*
#define BLACK 0
#define RED 1

struct Node {
    struct Node* left;
    struct Node* right;
    struct Node* parent;
    int color; // 0 for BLACK, 1 fo RED
    struct proc* data;
};

struct Node **root = 0;

struct Node** createRBTree() {
    root = (struct Node**)malloc(sizeof(struct Node*));
    *root = 0;
    return root;
}

struct Node* minimum(struct Node* tree) {
    if (tree == 0) {
        return 0;
    }
    while (tree->left != 0) {
        tree = tree->left;
    }
    return tree;
}

void rotateLeft(struct Node** root, struct Node* x) {
    struct Node* y = x->right;
    x->right = y->left;
    if (y->left != 0) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if (x->parent == 0) {
        *root = y;
    }
    else {
        if (x->parent->left ==x) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
    }

    y->left = x;
    x->parent = y;
}

void rotateRight(struct Node** root, struct Node* y) {
    struct Node* x = y->left;
    y->left = x->right;
    if (x->right != 0) {
        x->right->parent = y;
    }
    x->parent = y->parent;

    if (y->parent == 0) {
        *root = x;
    }
    else {
        if (y == y->parent->right) {
            y->parent->right = x;
        }
        else {
            y->parent->left = x;
        }
    }

    x->right = y;
    y->parent = x;
}

void insertFixup(struct Node** root, struct Node* node) {
    struct Node* parent, *gparent;
    while ((parent = node->parent) && ( parent->color == RED)) {
        gparent = parent->parent;
        if (parent == gparent->left) {
            struct Node *uncle = gparent->right;
            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;
                parent->color = BLACK;
                gparent->color = RED;
                node = gparent;
                continue;
            }
            if (parent->right == node) {
                struct Node* temp;
                rotateLeft(root, parent);
                temp = parent;
                parent = node;
                node = temp;
            }
            parent->color = BLACK;
            gparent->color = RED;
            rotateRight(root, gparent);
        }
        else {
            struct Node* uncle = gparent->left;
            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;
                parent->color = BLACK;
                gparent->color = RED;
                node = gparent;
                continue;
            }

            if (parent->left == node) {
                struct Node* temp;
                rotateRight(root, parent);
                temp = parent;
                parent = node;
                node = temp;
            }
            parent->color = BLACK;
            gparent->color = RED;
            rotateLeft(root, gparent);
        }
    }
}

void insert_(struct Node** root, struct Node* node) {
    struct Node* y = 0;
    struct Node* x = *root;

    while (x != 0) {
        y = x;
        if (node->data->maxTime < x->data->maxTime) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    node->parent = y;
    if (y != 0) {
        if (node->data->maxTime < y->data->maxTime) {
            y->left = node;
        }
        else {
            y->right = node;
        }
    }
    else {
        *root = node;
    }
    node->color = RED;
    insertFixup(root, node);
}

int insert(struct Node** root, struct proc* p) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    if (node != 0) {
        node->data = p;
        node->left = 0;
        node->right = 0;
        node->parent = 0;
        node->color = BLACK;
        insert_(root, node);
        return 0;
    }
    else {
        return -1;
    }
}
void deleteFixup(struct Node** root, struct Node* node, struct Node* parent) {
    struct Node* other;
    while ((!node || node->color == BLACK) && node != *root) {
        if (parent->left == node) {
            other = parent->right;
            if (other->color == RED) {
                other->color = BLACK;
                parent->color = RED;
                rotateLeft(root, parent);
                other = parent->right;
            }
            if ((!other->left || other->left->color ==BLACK) &&
                (!other->right || other->right->color == BLACK)) {
                other->color = RED;
                node = parent;
                parent = node->parent;
            }
            else {
                if (!other->right || other->right->color == BLACK) {
                    other->left->color = BLACK;
                    other->color = RED;
                    rotateRight(root, other);
                    other = parent->right;
                }
                other->color = parent->color;
                parent->color = BLACK;
                other->right = BLACK;
                rotateLeft(root, parent);
                node = *root;
                break;
            }
        }
        else {
            other = parent->left;
            if (other->color == RED) {
                other->color = BLACK;
                parent->color = RED;
                rotateRight(root, parent);
                other = parent->left;
            }
            if ((!other->left || other->left->color == BLACK) &&
                (!other->right || other->right->color == BLACK)) {
                other->color = RED;
                node = parent;
                parent = node->parent;
            }
            else {
                if (!other->left || other->left->color == BLACK) {
                    other->right->color = BLACK;
                    other->color = RED;
                    rotateLeft(root, other);
                    other = parent->left;
                }
                other->color = parent->color;
                parent->color = BLACK;
                other->left = BLACK;
                rotateRight(root, parent);
                node = *root;
                break;
            }
        }
    }
    if (node) {
        node->color = BLACK;
    }
}

void delete(struct Node** root, struct Node* node) {
    struct Node* child, *parent;
    int color;

    if ((node->left != 0) && (node->right != 0)) {
        struct Node *replace = node;
        replace = replace->right;
        while (replace->left != 0) {
            replace = replace->left;
        }
        if (node->parent) {
            if (node->parent->left == node) {
                node->parent->left = replace;
            }
            else {
                node->parent->right = replace;
            }
        }
        else {
            *root = replace;
        }

        child = replace->right;
        parent = replace->parent;
        color = replace->color;

        if (parent == node) {
            parent = replace;
        }
        else {
            if (child) {
                child->parent = parent;
            }
            parent->left = child;
            replace->right = node->right;
            node->right->parent = replace;
        }

        replace->parent = node->parent;
        replace->color = node->color;
        replace->left = node->left;
        node->left->parent = replace;

        if (color == BLACK) {
            deleteFixup(root, child, parent);
        }
        free(node);
        return;
    }

    if (node->left != 0) {
        child = node->left;
    } else {
        child = node->right;
    }

    parent = node->parent;
    color = node->color;

    if (child) {
        child->parent = parent;
    }

    if (parent) {
        if (parent->left == node) {
            parent->left = child;
        }
        else {
            parent->right = child;
        }
    }
    else {
        *root = child;
    }

    if (color == BLACK) {
        deleteFixup(root, child, parent);
    }
    free(node);
}


int popMin(struct Node** root, struct proc** val) {
    struct Node* node = 0;
    if (root) {
        node = minimum(*root);
    }
    if (node == 0) {
        return 1;
    }
    else {
        *val = node->data;
        delete(root, node);
    }
    return 0;
}
*/
/* ---------------------------------------------------- */

/* ------------------------------------------------------------ */
// definition of the priority queue implemented with binary heap
// Array to store the process heap data
struct proc *procHeap[64];
int heapSize = 0;

void initHeap() {
    for (int i = 0; i < 64; i++) {
        procHeap[i] = 0;
    }
}

void siftdown(int startPos, int pos) {
    struct proc * newItem = procHeap[pos];
    while (pos > startPos) {
        int parentPos = (pos - 1) >> 1;
        struct proc * parent = procHeap[parentPos];
        if (newItem->execTime < parent->execTime) {
            procHeap[pos] = parent;
            pos = parentPos;
            continue;
        }
        break;
    }
    procHeap[pos] = newItem;
}

void siftup(int pos) {
    int endPos = heapSize;
    int startPos = pos;
    struct proc *newItem = procHeap[pos];
    int childPos = 2 * pos + 1;
    while (childPos < endPos) {
        int rightPos = childPos + 1;
        if ((rightPos < endPos) && !(procHeap[childPos] < procHeap[rightPos])) {
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
    if (heapSize < 64) {
        procHeap[heapSize++] = item;
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
        struct proc *root = procHeap[0];
        procHeap[0] = 0;
        heapSize = 0;
        return root;
    }
    struct proc *last = procHeap[heapSize - 1];
    struct proc *root = procHeap[0];
    procHeap[0] = last;
    procHeap[--heapSize] = 0;
    siftup(0);
    return root;
}

uint burstStartTime;
int numProcs = 0;

#include "statistics.h"

extern int time_slot_count;
extern int cpu_running_time_slot_count;
extern int reset;

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

void setprocname(const char* name) {
  safestrcpy(myproc()->name, name, siezof(name));
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->createTime = ticks;
  p->execTime = 0;
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  numProcs++;
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;
  heappush(p);
  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;
  heappush(np);
  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  curproc->execTime += ticks - burstStartTime;
  numProcs--;

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;
      struct proc* p;
      p = heappop();
      if (p != 0) {
        //cprintf("cpu: %d, proc: %s\n", c->apicid, p->name);
        // Switch to chosen process.  It is the process's job
        // to release ptable.lock and then reacquire it
        // before jumping back to us.
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;
        burstStartTime = ticks;
        c->proc->maxTime = ((ticks - c->proc->createTime)) / numProcs;
        swtch(&(c->scheduler), p->context);
        switchkvm();

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
      }
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  struct proc *p = myproc();
  p->state = RUNNABLE;
  heappush(p);
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == SLEEPING && p->chan == chan) {
      p->state = RUNNABLE;
      heappush(p);
    }
  }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
        heappush(p);
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int get_total_time_slot_count(){
	return time_slot_count;
}
int get_total_cpu_running_time_slot_count(){
	return cpu_running_time_slot_count;
}

int init(){
  reset = 1;
  return 0;
}

int wait2(int *retime, int *rutime, int *stime) {
  struct proc *p;
  int havekids, pid;
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for zombie children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != myproc())
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        *retime = p->retime;
        *rutime = p->rutime;
        *stime = p->stime;
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->retime = 0;
        p->rutime = 0;
        p->stime = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || myproc()->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(myproc(), &ptable.lock);  //DOC: wait-sleep
  }
}

/*
  This method will run every clock tick and update the statistic fields for each proc
*/
void updatestatistics() {
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    switch(p->state) {
      case SLEEPING:
        p->stime++;
        break;
      case RUNNABLE:
        p->retime++;
        break;
      case RUNNING:
        p->rutime++;
        break;
      default:
        ;
    }
  }
  release(&ptable.lock);
}
