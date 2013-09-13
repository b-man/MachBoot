/*
 * Copyright 2013, winocm. <winocm@icloud.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   If you are going to use this software in any form that does not involve
 *   releasing the source to this project or improving it, let me know beforehand.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Thread Manager
 */

#include "genboot.h"
#include "armv.h"

#define STACK_SIZE        4096

thread_t *CurrentThread;

static thread_t *bootstrap_thread, *irq_thread;
static int thread_seed = 0;

/**
 * arm_set_threadpid_user_readonly
 *
 * Set current thread identifier to the cthread identifier.
 */
static void arm_set_threadpid_user_readonly(uint32_t* address) {
    __asm__ __volatile__("mcr p15, 0, %0, c13, c0, 3" :: "r"(address));
}

/**
 * arm_set_threadpid_priv_readwrite
 *
 * Set current thread identifier to the specified thread_t.
 */
static void arm_set_threadpid_priv_readwrite(uint32_t* address) {
    __asm__ __volatile__("mcr p15, 0, %0, c13, c0, 4" :: "r"(address));
}

/**
 * arm_set_current_thread
 *
 * Set current thread to the specified thread_t.
 */
void arm_set_current_thread(thread_t *thread)
{
    /* Set the current thread. */
    CurrentThread = thread;
    
    arm_set_threadpid_priv_readwrite((uint32_t*)CurrentThread);
}

/**
 * arm_thread_create
 *
 * Do machine-dependent initialization for a thread.
 */
thread_t* arm_thread_create(char* name, uint32_t magic)
{
    thread_t* new_thread = malloc(sizeof(thread_t));
    assert(new_thread != NULL);
    
    /* Zero it. */
    bzero(new_thread, sizeof(thread_t));
    
    /* Set the members. */
    new_thread->preemption_count = 1;
    new_thread->unique_id = magic + thread_seed;
    new_thread->name = name;
    
    /* Set linked list values. */
    new_thread->task_list.prev = new_thread;
    new_thread->task_list.next = new_thread;
    
    /* Set task state. */
    new_thread->state = kStateStopped;
    
    /* Set thread stack. */
    uintptr_t* stack_ptr = (uintptr_t*)malloc(STACK_SIZE * 2);
    assert(stack_ptr);
    
    new_thread->machine.sp = (uint32_t)(stack_ptr + STACK_SIZE);
    
    /* Increment the ThreadSeed. */
    thread_seed++;
    
    return new_thread;
}

/**
 * thread_enter_critical_section
 *
 * Enter a critical section.
 */
void thread_enter_critical_section(void)
{
    thread_t* thr = _arm_get_current_thread();
    assert(thr != NULL);
    
    /* Increment preemption count and disable interrupts */
    thr->preemption_count++;
    _splhigh();
    
    return;
}

/**
 * thread_leave_critical_section
 *
 * Leave a critical section.
 */
void thread_leave_critical_section(void)
{
    thread_t* thr = _arm_get_current_thread();
    assert(thr != NULL);
    
    /* Decrement preemption count and enable interrupts */
    thr->preemption_count--;
    _spllo();
    
    assert(thr->preemption_count > -1);
    
    return;
}

/**
 * thread_internal_add_after
 *
 * Add the thread to the linked list.
 */
static void thread_internal_add_after(thread_t* a, thread_t* b)
{
    thread_t *prev, *next;
    assert(a && b);
    
    /* Enter a critical section. */
    thread_enter_critical_section();
    
    /* Set it. */
    prev = b;
    next = b->task_list.next;
    prev->task_list.next = a;
    a->task_list.prev = prev;
    next->task_list.prev = a;
    a->task_list.next = next;
    
    /* Leave. */
    thread_leave_critical_section();
    return;
}

/**
 * thread_internal_add_before
 *
 * Add the thread to the linked list.
 */
static void thread_internal_add_before(thread_t* a, thread_t* b)
{
    thread_t *prev, *next;
    assert(a && b);
    
    /* Enter a critical section. */
    thread_enter_critical_section();
    
    /* Set it. */
    prev = b;
    next = b->task_list.prev;
    prev->task_list.next = a;
    a->task_list.prev = prev;
    next->task_list.prev = a;
    a->task_list.next = next;
    
    /* Leave. */
    thread_leave_critical_section();
    return;
}

/**
 * thread_start
 *
 * Start and execute a new thread.
 */
extern void _Call_continuation(void *continuation, void* args);

int thread_start(thread_t* new, void* function, void* arg)
{
    /* Enter a critical section. */
    thread_enter_critical_section();
    
    /* Has the current thread stopped? If it is, start it. */
    assert(new);
    
    if(new->state == kStateStopped) {
        /* Set registers and go. */
        new->machine.r[4] = (uint32_t)function;
        new->machine.r[5] = (uint32_t)arg;
        
        /* Call the continuation function. */
        new->machine.lr = (uint32_t)&_Call_continuation;
        
        /* The thread is now running. */
        new->state = kStateRunning;
        
        /* Go. */
        thread_internal_add_before(new, CurrentThread);
        _Switch_context(new);
        
        thread_leave_critical_section();
        return 0;
    }
    
    /* We're leaving. */
    thread_leave_critical_section();
    return -1;
}

/**
 * thread_init
 *
 * Start and initialize the thread subsystem.
 */
void thread_init(void)
{
    /* Set up core tasks. */
    assert((bootstrap_thread = arm_thread_create("bootstrap", 'strp')) != NULL);
    assert((irq_thread = arm_thread_create("irq task", 'irqt')) != NULL);
    
    /* We are the bootstrap thread. */
    arm_set_current_thread(bootstrap_thread);
}

/**
 * thread_internal_remove_thread
 *
 * Remove the thread from the linked lists.
 */
static void thread_internal_remove_thread(thread_t* thr)
{
    thread_t *prev, *next;
    assert(thr);
    
    /* Enter a critical section. */
    thread_enter_critical_section();
    
    /* Remove it. */
    prev = thr->task_list.prev;
    next = thr->task_list.next;
    prev->task_list.next = next;
    next->task_list.prev = prev;
    thr->task_list.next = thr;
    thr->task_list.prev = thr;
    
    /* Get out. */
    thread_leave_critical_section();
    return;
}

/**
 * thread_kill
 *
 * Kill the current thread.
 */
void thread_kill(void)
{
    thread_enter_critical_section();
    
    /* Get new task. */
    thread_t *next, *current = _arm_get_current_thread();
    assert(current);
    next = current->task_list.next;
    assert(next);
    
    /* We can't break the only task. */
    if(next == current) {
        thread_leave_critical_section();
        panic("killing last task? wat?\n");
    }
    
    /* Remove us. */
    current->state = kStateStopped;
    thread_internal_remove_thread(current);
    
    /* Go to next thread. */
    _Switch_context(next);
}

/**
 * thread_continue
 *
 * Continue execution of a thread.
 */
void thread_continue(void *(function)(void*), void* args)
{
    /* Enter critical section. */
    thread_leave_critical_section();
    
    /* Run function. */
    assert(function);
    function(args);
    
    /* Bye. */
    thread_enter_critical_section();
    
    /* Commit suicide, we're done here. */
    thread_kill();
}
