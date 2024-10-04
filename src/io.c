#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include "io.h"
#include "forth.h"
#include "dict.h"
#include "stack.h"

#define MAX_TRACE_DEPTH 16

char tib[TERM_CHARS];
int_ in = 0;

static FILE *fp;

void print_stacktrace(const int_ *ip)
{
    const int_ *addr = ip;

    int n; 
    for(n = 0; n < MAX_TRACE_DEPTH; ++n)
    {
        const char *def = (const char *)trace(addr);
        if(def)
            printf("0x%.*" PRIxPTR ": %s\n", (int)sizeof(void *)*2,
                (uint_)def, def ? cnt_to_c(def) : "?");
        if(rstack_is_empty())
            break;
        addr = (const int_ *)r_pop_v();
    }

    if(n == MAX_TRACE_DEPTH && !rstack_is_empty())
        printf("...\n");
}

void include(const char *path)
{
    push((int_)path);
    push(strlen(path));
    included();
}

void included(void)
{

    char  path[FILENAME_MAX] = {0};

    const uint_ size   = pop();
    const char *c_addr = (const char *)pop();
    
    // Reformat the given char array as a C string
    memcpy(path, c_addr, size >= FILENAME_MAX ? FILENAME_MAX : size);

    // Save input
    char p_tib[TERM_CHARS]; 
    memcpy(p_tib, tib, TERM_CHARS);
    const int_  p_in = in;
    FILE *p_fp       = fp;

    fp = fopen(path, "rb");
    if(!fp)
        err("fopen(): \"%s\": %s\n",
                path, strerror(errno));

    const int_ p_line  = line;
    const char *p_file = file;
    line = 1;
    file = path;

    while(!feof(fp))
    {
        refill(tib, TERM_CHARS);
        
        in = 0;
        interpret();

        ++line;
    }
    fclose(fp);

    // Restore input
    memcpy(tib, p_tib, TERM_CHARS);
    in = p_in;
    fp = p_fp;

    line = p_line;
    file = p_file;
}

void key(void)
{
    push(getchar());
}

int refill(char *buffer, int size)
{
    int c = 0;
    for( ; c < size; ++c)
    {
        const int ch = fgetc(fp);
        if(ch == EOF || ch == '\0' || ch == '\n')
            break;
        buffer[c] = ch;
    }
    buffer[c] = '\0';
    return c;
}

void accept(void)
{
    FILE *p_fp = fp;
    fp = stdin;
    const int_ n = pop();
    char *buffer = (char *)pop();

    const int_ r = refill(buffer, n);
    
    fp = p_fp;
    push(r);
}

void word(void)
{
    static char buffer[CSTRING_MAX_CHARS+1] = {0};
    
    const int delim = pop();
    const int chars = strlen(tib);
    
    // Skip leading delimiters
    while(tib[in] == delim && in < chars)
    {
        ++in;
    }
    const int start = in;
    // Skip forward until the end
    while(tib[in] != delim && tib[in] != '\n' && in < chars)
    {
        ++in;
    }
    
    // Copy marked area into transient buffer
    const int length = ((in - start) > CSTRING_MAX_CHARS) ? 
        CSTRING_MAX_CHARS : (in - start);
    memcpy(&buffer[1], &tib[start], length);
    buffer[0] = length;

    push((int_)buffer);
}

void eol(void)
{
    push('\n');
}

void emit(void)
{
    putchar(pop());
}

void source(void)
{
    push((int_)tib);
    push(strlen(tib));
}

void pad_(void)
{
    push((int_)&pad[PAD_CHARS-1]);
}

void pad0_(void)
{
    push((int_)pad);
}

void in_(void)
{
    push((int_)&in);
}

void reset_io(void)
{
    memset(tib, 0, TERM_CHARS);
    in = 0;
    fp = stdin;
}
