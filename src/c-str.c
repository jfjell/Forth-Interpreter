#include <string.h>
#include <ctype.h>
#include "forth.h"
#include "stack.h"

static int ch_to_val(int);

char * c_to_cnt(const char *c_str)
{
    static char buffer[CSTRING_MAX_CHARS+1] = {0};
    memset(buffer, 0, CSTRING_MAX_CHARS+1);

    int_ sz = 0;
    while(c_str[sz] && sz != CSTRING_MAX_CHARS) {
        buffer[sz+1] = c_str[sz];
        sz++;
    }
    buffer[0] = sz;
    return buffer;
}

char * cnt_to_c(const char *c_addr)
{
    static char buffer[CSTRING_MAX_CHARS+1] = {0};
    memset(buffer, 0, CSTRING_MAX_CHARS+1);

    int_ sz = *(c_addr++) & CSTRING_MAX_CHARS_MASK;
    for(int i = 0; i < sz; ++i) {
        buffer[i] = *(c_addr++);
    }
    buffer[sz] = '\0';
    return buffer;
}

int cnt_strcmp(const char *c_addr1, const char *c_addr2)
{
    if(*c_addr1 != *c_addr2)
        return *c_addr1 - *c_addr2;
    const int size = *c_addr1;
    
    ++c_addr1;
    ++c_addr2;
    for(int c = 0; c < size; ++c)
    {
        if(toupper(c_addr1[c]) != toupper(c_addr2[c]))
            return c_addr1[c] - c_addr2[c];
    }
    return 0;
}

void number(void)
{
    int_ size    = pop();
    char *c_addr = (char *)pop();
    long_ d      = pop2();

    int sign;
    if((sign = (*c_addr == '-')))
    {
        ++c_addr;
        --size;
    }
    
    do
    {
        const int val = ch_to_val(*c_addr);
        if(val == -1)
            break;
        d = (d * base) + val;
        ++c_addr;
    }while(--size);

    push2(sign ? -d : d);
    push((int_)c_addr);
    push(size);
}

static inline int in_range(int x, int low, int high)
{
    return x >= low && x <= high;
}

/* On failure to convert; returns -1
 */
static int ch_to_val(int ch)
{
    if(in_range(ch, '0', '9') && base + '0' > ch)
        return ch - '0';
    else if(in_range(ch, 'A', 'Z') && (base - 10 + 'A') > ch)
        return ch - 'A' + 10;
    else if(in_range(ch, 'a', 'z') && (base - 10 + 'a') > ch)
        return ch - 'a' + 10;
    else
        return -1;
}
