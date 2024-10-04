#ifndef IO_H_
#define IO_H_

#include "forth.h"

#ifndef TERM_CHARS
# define TERM_CHARS 256
#endif

#define BL ' '

extern char tib[TERM_CHARS];
extern int_ in;

void    accept(void);
void    key(void);
void    word(void);
void    emit(void);
void    eol(void);
void    source(void);
void    pad_(void);
void    pad0_(void);
void    in_(void);
void    included(void);
int     refill(char *, int);
void    print_stacktrace(const int_ *);
void    include(const char *);
void    reset_io(void);

#endif
