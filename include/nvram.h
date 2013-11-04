/*
 * Copyright 2013, Brian McKenzie. <mckenzba@gmail.com>
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

#ifndef _NVRAM_H_
#define _NVRAM_H_

/* NVRAM variable struct */
typedef struct _nvram_variable {
        char *name;
        char *setting;
        int overridden;
} nvram_variable_t;

/* NVRAM variable node struct */
typedef struct _nvram_variable_node {
        struct _nvram_variable_node *next;
        nvram_variable_t value;
} nvram_variable_node_t;

/* NVRAM variable list struct */
typedef struct _nvram_variable_list {
        nvram_variable_node_t *head, **tail;
} nvram_variable_list_t;

/* Initial NVRAM variable(s) (used for initialization) */
extern nvram_variable_t gNvramDefaultVariables[];

/* Global NVRAM variable(s) (Holds all nvram variable data) */
extern nvram_variable_list_t *gNvramVariables;

/* NVRAM low-level api prototypes */
int nvram_init(nvram_variable_t *vars, size_t size);
nvram_variable_list_t *nvram_initialize_list(void);
nvram_variable_node_t *nvram_create_node(const char *name, const char *setting, int overridden);
void nvram_append_node(nvram_variable_list_t *list, nvram_variable_node_t *node);
void nvram_remove_node(nvram_variable_list_t *list, nvram_variable_node_t *node);

/* NVRAM high-level api prototypes */
void nvram_variable_set(nvram_variable_list_t *list, const char *name, const char *setting);
int nvram_variable_unset(nvram_variable_list_t *list, const char *name);
nvram_variable_t nvram_read_variable_info(nvram_variable_list_t *list, const char *name);
void nvram_dump_list(nvram_variable_list_t *list);

#endif /* !_NVRAM_H_ */