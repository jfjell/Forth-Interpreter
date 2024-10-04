#include "forth.h"
#include "stack.h"

void subtract(void)
{
    const int_ y = pop();
    push(pop() - y);
}

void multiply(void)
{
    push(pop() * pop());
}

void um_modulo(void)
{
    const ulong_ y = pop2();
    const ulong_ x = pop2();
    const int_   q = x / y;
    const int_   r = x % y;
    push2(r);
    push2(q);
}

void division_modulo(void)
{
    const int_ y = pop();
    const int_ x = pop();
    const int_ q = x / y;
    const int_ r = x % y;
    push(r);
    push(q);
}

void double_division_modulo(void)
{
    const long_ y = pop2();
    const long_ x = pop2();
    const long_ q = x / y;
    const long_ r = x % y;
    push2(r);
    push2(q);
}

void double_multiply(void)
{
    push2(pop2() * pop2());
}

void double_multiply_unsigned(void)
{
    push2((ulong_)pop2() * (ulong_)pop2());
}

void bit_and(void)
{
    push(pop() & pop());
}

void rshift(void)
{
    const uint_ bits = pop();
    push((uint_)pop() >> bits);
}

void lshift(void)
{
    const uint_ bits = pop();
    push((uint_)pop() << bits);
}

void less(void)
{
    const int_ y = pop();
    push((pop() < y) ? TRUE : FALSE);
}

void less_unsigned(void)
{
    const uint_ y = (uint_)pop();
    push(((uint_)pop() < y) ? TRUE : FALSE);
}

void store(void)
{
    int_ *addr = (int_ *)pop();
    *addr = pop();
}

void fetch(void)
{
    push(*(int_ *)pop());
}

void char_fetch(void)
{
    push(*(char *)pop());
}

void char_store(void)
{
    char *c_addr = (char *)pop();
    *c_addr = (char)pop();
}

void aligned(void)
{
    const int_ addr = pop();
    push((addr % CELL) ? CELL - (addr % CELL) : 0);
}
