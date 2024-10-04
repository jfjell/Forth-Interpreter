#include <string.h>
#include "dict.h"
#include "forth.h"
#include "stack.h"
#include "io.h"
#include "arith.h"

static int_    dp0[DICTIONARY_CELLS] = {0};
static int_   *dp = dp0;
static int_   *lp = (int_ *)0;

static void   define(const char *);
static int_ * traverse(const char *, int_ *);

void primitive(const char *c_string, void (*virtual)(void), int opt)
{
    define(c_to_cnt(c_string));
    dp[-1] |= opt | PRIMITIVE;
    memcpy(dp++, &virtual, sizeof(void (*)(void)));
}

void create(void)
{
    push(BL);
    word();
    define((char *)pop());
}

void compile_xt(int_ xt)
{
    *(dp++) = xt;
}

void xt(void)
{
    char *cp = (char *)pop();
    cp += CELL;
    cp += (*cp + 1) & CSTRING_MAX_CHARS_MASK;
    push((int_)cp);
    aligned();

    push((int_)(cp + pop()));
}

void forget(void)
{
    push(BL);
    word();
    push((int_)traverse((const char *)pop(), lp));
    int_ *def = (int_*)pop();
    if(def != (void *)0)
        *(char *)(&def[1]) |= DELETED;
}

void colon(void)
{
    create();
    state = COMPILE;
}

void semicolon(void)
{
    *(dp++) = 0;
    *(dp++) = 0;
    state = INTERPRET;
}

void find(void)
{
    push((int_)traverse((char *)pop(), lp));
    const int_ def = pop();
    push(def);

    if(def != 0)
        xt();
}

void align(void)
{
    char *cp = (char *)dp;
    push((int_)cp);
    aligned();
    cp += pop();
    dp = (int_*)cp;
}

static void define(const char *c_addr)
{
    align();

    if(traverse(c_addr, lp))
        printf("%s redefined\n", cnt_to_c(c_addr));

    // Write link and update latest definition
    int_ *def = dp;
    *(dp++) = (int_)lp;
    lp = def;

    // Copy the string into the definition
    const int length = *(c_addr++) & CSTRING_MAX_CHARS_MASK;
    char *cp = (char *)dp;
    *(cp++) = length;
    memcpy(cp, c_addr, length);
    cp = &cp[length];
    push((int_)cp);
    aligned();
    cp += pop();
    dp = (int_*)cp;

    // Write default XT value
    *(dp++) = 0;
}

static int_ * traverse(const char *c_addr, int_ *def)
{
    if(!def)
        return (int_ *)0;

    const char *c_addr2 = (const char *)(&def[1]);
    if(!*c_addr2)
        return (int_ *)0;

    if((def != lp || state != COMPILE) && !cnt_strcmp(c_addr, c_addr2))
        return def;
    else
        return traverse(c_addr, (int_ *)*def);
}

void * trace(const int_ *ip)
{
    int_ *def = lp;
    while(def > ip)
    {
        def = (int_ *)*def;
    }
    return def ? &def[1] : 0;
}

void dp_(void)
{
    push((int_)&dp);
}

void lp_(void)
{
    push((int_)&lp);
}
