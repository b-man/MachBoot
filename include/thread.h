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

#ifndef _THREAD_H_
#define _THREAD_H_

#include "genboot.h"
#include "thread_internal.h"

/* !!! DO NOT TOUCH THIS STRUCTURE !!! */
typedef struct _list_t {
	void *prev, *next;
} list_t;

typedef enum {
	kStateSuspended = 0,
	kStateRunning,
	kStateStopped,
} task_state_t;

typedef struct thread_t {
	machine_thread_t     machine;
	uint32_t             unique_id;
	int                  preemption_count;
	char*                name;
	list_t               task_list;
	task_state_t         state;
} thread_t;

#ifdef __arm__

extern void arm_set_current_thread(thread_t *thread);
extern thread_t* arm_thread_create(char* name, uint32_t magic);
extern thread_t* _arm_get_current_thread(void);

extern void _Switch_context(thread_t* new_thread);
extern void _Processor_idle(void);

#else
#error Threading is only supported on ARM right now.
#endif

extern void thread_init(void);
extern void thread_enter_critical_section(void);
extern void thread_leave_critical_section(void);
extern int thread_start(thread_t* new, void* function, void* arg);

#endif