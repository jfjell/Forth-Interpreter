#include "stack.h"
#include "forth.h"

static int_   sp0[STACK_CELLS];
static int_   *sp = sp0;
static int_   rp0[R_STACK_CELLS];
static int_   *rp = rp0;
static int_   csp0[CS_STACK_CELLS];
static int_   *csp = csp0;

void push(int_ x)
{
    if(sp == &sp0[STACK_CELLS])
        err("Stack overflow\n");
    *(sp++) = x;
}

int_ pop(void)
{
    if(sp == sp0)
        err("Stack underflow\n");
    return *(--sp);
}

void push2(long_ x)
{
    const int_ low  = (int_)(x & MAX_UN);
    push(low);
    const int_ high = MAX_N < MAX_D ? (int_)((x & ((ulong_)MAX_UN 
        << (CELL * CHAR_BIT))) >> (CELL * CHAR_BIT)) : 0;
    push(high);
}

long_ pop2(void)
{
    const int_ high  = pop();
    const int_ low   = pop();
    return (long_)low 
         | (MAX_N < MAX_D ? ((long_)high << (CELL * CHAR_BIT)) : 0);
}

void drop(void)
{
    pop();
}

void pick(void)
{
    const int_ n = pop() + 1;
    if(n > (int_)(sp - sp0))
        err("Stack underflow\n");
    push(sp[-n]);
}

void roll(void)
{
    int_ n = pop();
    push(n);
    pick();

    while(n >= 0)
    {
        sp[-(n+2)] = sp[-(n+1)];
        n--;
    }
    drop();
}

void r_push_v(int_ x)
{
    if(rp == &rp0[R_STACK_CELLS])
        err("Return stack overflow\n");
    *(rp++) = x;
}

int_ r_pop_v(void)
{
    if(rp == rp0)
        err("Return stack underflow\n");
    return *(--rp);
}

void r_push(void)
{
    const int_ addr = r_pop_v();
    r_push_v(pop());
    r_push_v(addr);
}

void r_pop(void)
{
    const int_ addr = r_pop_v();
    push(r_pop_v());
    r_push_v(addr);
}

void cs_push(void)
{
    if(csp == &csp0[CS_STACK_CELLS])
        err("Control stack overflow\n");
    *(csp++) = pop();
}

void cs_pop(void)
{
    if(csp == csp0)
        err("Control stack underflow\n");
    push(*(--csp));
}

void depth(void)
{
    push((int_)(sp - sp0));
}

void reset_stack(void)
{
    sp = sp0;
}

void reset_rstack(void)
{
    rp = rp0;
    csp = csp0;
}

int rstack_is_empty(void)
{
    return rp == rp0;
}
