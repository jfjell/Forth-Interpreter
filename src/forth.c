#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include "forth.h"
#include "stack.h"
#include "dict.h"
#include "arith.h"
#include "io.h"

char        pad[PAD_CHARS];
int_        state = INTERPRET;
int_        base  = 10;
int         line  = 1;
const char *file; 

static int  init(int, char **);
static void start(void);

static int_   *ip = 0;
static int_    literal_xt;
static jmp_buf jmp_throw;

#define VERSION "FrogForth v0.0.1"
#define HELP VERSION "\n" \
    "Usage: fgf <options> <file(s)\n" \
    "Options:\n" \
    "--help:       Display this message and exit\n" \
    "--version:    Display version information and exit\n"
#define HEADER VERSION "\n" \
    "Type 'bye' to exit"


int main(int argc, char *argv[])
{
    if(init(argc, argv) != 0)
        return -1;
    
    ip = 0;
    start();

    return 0;
}

void environment(void)
{
    const uint_ size  = (uint_)pop();
    const char *query = (const char *)pop();

    if(!strncmp(query, "/COUNTED-STRING", size))
        push(CSTRING_MAX_CHARS);
    else if(!strncmp(query, "/HOLD", size))
        push(PAD_CHARS);
    else if(!strncmp(query, "PAD", size))
        push(PAD_CHARS);
    else if(!strncmp(query, "ADDRESS-UNIT-BITS", size))
        push(CELL * CHAR_BIT);
    else if(!strncmp(query, "FLOORED", size))
        push(((1 / 2) == (-1 / 2)) ? TRUE : FALSE);
    else if(!strncmp(query, "MAX_CHAR", size))
        push(UCHAR_MAX);
    else if(!strncmp(query, "MAX-D", size))
        push2(MAX_D);
    else if(!strncmp(query, "MAX-UD", size))
        push2(MAX_UD);
    else if(!strncmp(query, "MAX-N", size))
        push(MAX_N);
    else if(!strncmp(query, "MAX-UN", size))
        push(MAX_UN);
    else if(!strncmp(query, "RETURN-STACK-CELLS", size))
        push(R_STACK_CELLS);
    else if(!strncmp(query, "STACK-CELLS", size))
        push(STACK_CELLS);
    else
        push(FALSE);
}
int init(int argc, char *argv[])
{
    reset();

    primitive("DROP",          drop,                     0);
    primitive("PICK",          pick,                     0);
    primitive("ROLL",          roll,                     0);
    primitive(">R",            r_push,                   0);
    primitive("R>",            r_pop,                    0);
    primitive("-",             subtract,                 0);
    primitive("*",             multiply,                 0);
    primitive("/MOD",          division_modulo,          0);
    primitive("LSHIFT",        lshift,                   0);
    primitive("RSHIFT",        rshift,                   0);
    primitive("AND",           bit_and,                  0);
    primitive("<",             less,                     0);
    primitive("!",             store,                    0);
    primitive("@",             fetch,                    0);
    primitive("ALIGNED",       aligned,                  0);
    primitive(":",             colon,                    0);
    primitive(";",             semicolon,                IMMEDIATE);
    primitive("FIND",          find,                     0);
    primitive("FORGET",        forget,                   0);
    primitive("ALIGN",         align,                    0);
    primitive("EXECUTE",       execute,                  0);
    primitive("KEY",           key,                      0);
    primitive("WORD",          word,                     0);
    primitive("EMIT",          emit,                     0);
    primitive("C!",            char_store,               0);
    primitive("C@",            char_fetch,               0);
    primitive("ABORT",         abort_,                   0);
    primitive("U<",            less_unsigned,            0);
    primitive("ENVIRONMENT?",  environment,              0);
    primitive("ACCEPT",        accept,                   0); 
    primitive("CELL",          cell,                     0);
    primitive("STATE",         state_,                   0);
    primitive("BASE",          base_,                    0);
    primitive("PAD",           pad_,                     0);
    primitive("-PAD",          pad0_,                    0);
    primitive(">IN",           in_,                      0);
    primitive("SOURCE",        source,                   0);
    primitive("DP",            dp_,                      0);
    primitive("LP",            lp_,                      0);
    primitive(">NUMBER",       number,                   0);
    primitive("QUIT",          quit,                     0);
    primitive("INCLUDED",      included,                 0);
    primitive("DEPTH",         depth,                    0);
    primitive("UM/MOD",        um_modulo,                0);
    
    // Implementation defined words are prefixed with '::' 
    primitive("::LITERAL",     literal,                  0);
    primitive("::JMP-IF-ZERO", jmp_if_zero,              0);
    primitive("::JMP",         jmp,                      0);
    primitive("::XT",          xt,                       0);
    primitive("::CREATE",      create,                   0);
    primitive("::CS-PUSH",     cs_push,                  0);
    primitive("::CS-POP",      cs_pop,                   0);
    primitive("EOL",           eol,                      0);
    primitive("D*",            double_multiply,          0);
    primitive("UD*",           double_multiply_unsigned, 0);
    primitive("D/MOD",         double_division_modulo,   0);
    primitive("BYE",           bye,                      0);
    
    // Ugly solution to needing the XT of literal when compiling
    // constants into definitions
    push((int_)c_to_cnt("::LITERAL"));
    find();
    literal_xt = pop();

    if(setjmp(jmp_throw) != 0)
        return -1;
    include("core.fs");
    
    if(setjmp(jmp_throw) != 0)
        return 0;

    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "--version") == 0)
            return puts(VERSION), 0;
        else if(strcmp(argv[i], "--help") == 0)
            return puts(HELP), 0;
        else
            include(argv[i]);
    }

    return 0;
}

void start(void)
{
    puts(HEADER);
    
    setjmp(jmp_throw);

    file = "stdin";
    line = 1;
    for( ; ; )
    {
        refill(tib, TERM_CHARS);

        in = 0;
        interpret();

        printf("ok ");
    }
}

/* Execute the execution token currently
 * on the stack
 */
void execute(void)
{
    int_ *xt   = (int_ *)pop();
    ip         = &xt[1];
    
    if(*xt & PRIMITIVE)
    {
        void (*virtual)(void);
        memcpy(&virtual, ip, sizeof(void (*)(void)));
        virtual();
    } 
    else 
    {
        while(ip && *ip) 
        {
            // Push the address of the next instruction onto the
            // return stack
            r_push_v((int_)++ip);
            
            // Push the instruction to be executed onto the stack,
            // and execute it
            push(ip[-1]);
            execute();
            
            // Return to the address at top of the return stack
            ip = (int_ *)r_pop_v();
        }
    }
}

/* Interpret all the words on the line currently in TIB
 * and return
 */
void interpret(void)
{
    for( ; ; )
    {
        // Read word from the input stream, if its length is 0
        // (reached EOL) then return
        push(BL);
        word();
        const char *c_addr = (const char *)pop();
        if(!*c_addr)
            return;
        
        // Attempt to find the word in the dictionary
        push((int_)c_addr);
        find();
        const int_ *xt = (int_ *)pop();
       
        if(xt)
        {
            // If word found in dictionary is immediate or 
            // state in interpret mode, execute, else compile into 
            // the current definition
            push((int_)xt);
            if(state == INTERPRET || (*xt & IMMEDIATE))
                execute();
            else
                compile_xt(pop());
        }
        else
        {
            // Attempt to convert word into a number, on success,
            // if interpret mode, push number onto the stack, else
            // compile as a literal into the current definition
            push2(0);
            push((int_)&c_addr[1]);
            push(*c_addr);
            number();
            if(pop() != 0)
                err("%s is not a number or definition\n",
                        cnt_to_c(c_addr));
            drop();
            const int_ value = (int_)pop2();

            if(state == INTERPRET)
            {
                push(value);
            }
            else
            {
                compile_xt(literal_xt);
                compile_xt(value);
            }
        }
    }
}

void quit(void)
{
    reset_rstack();
    reset_io();
    longjmp(jmp_throw, 0);
}

void bye(void)
{
    exit(0);
}

void reset(void)
{
    reset_stack();
    reset_rstack();
    reset_io();

    ip    = (int_*)0;
    state = INTERPRET;
}


void except(void)
{
    int offset = 0;
    printf("<%s:%d:%d>:%n %s\n", 
        file, line, (int)in, &offset, tib);
    printf("%*c^\n", (int)(offset+in), ' ');

    print_stacktrace(ip);

    abort_();
}

void abort_(void)
{
    reset();
    longjmp(jmp_throw, 1);
}

/* Push the data in the next cell after the current
 * instruction pointer onto the stack
 */
void literal(void)
{
    const int_ *v = (int_ *)r_pop_v();
    push(*v);
    r_push_v((int_)(&v[1]));
}

void jmp_if_zero(void)
{
    const int_ *addr = (const int_ *)pop();
    if(pop() == 0)
    {
        r_pop_v();
        r_push_v((int_)addr);
    }
}

void jmp(void)
{
    r_pop_v();
    r_push_v(pop());
}

void cell(void)
{
    push(CELL);
}

void state_(void)
{
    push((int_)&state);
}

void base_(void)
{
    push((int_)&base);
}
