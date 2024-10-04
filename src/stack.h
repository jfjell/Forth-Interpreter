#ifndef STACK_H_
#define STACK_H_

#include "forth.h"

#ifndef STACK_CELLS
# define STACK_CELLS 512
#endif

#ifndef R_STACK_CELLS
# define R_STACK_CELLS 1024
#endif

#ifndef CS_STACK_CELLS
# define CS_STACK_CELLS 64
#endif

void     push(int_);
int_     pop(void);
void     push2(long_);
long_    pop2(void);
void     drop(void);
void     pick(void);
void     roll(void);
void     depth(void);

void     r_push_v(int_);
int_     r_pop_v(void);
void     r_push(void);
void     r_pop(void);

void     cs_push(void);
void     cs_pop(void);

void     reset_stack(void);
void     reset_rstack(void);
int      rstack_is_empty(void);

#endif
