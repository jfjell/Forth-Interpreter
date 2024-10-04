#ifndef DICT_H_
#define DICT_H_

#include "forth.h"

#ifndef DICTIONARY_CELLS
# define DICTIONARY_CELLS 16384
#endif

/* 
    The dictionary grows from lower to higher addresses
    Layout of a dictionary entry in memory (cell-aligned)
    <Lower addresses...>
    <Link>
    <Name>
    <XT>
    <Data>
    <Higher addresses...>

    Link:
        Next cell-aligned address after the end of the name field, gives 
        the address of the preceding definition
    Name:
        First byte gives the length of the word, followed by the
        characters for the name
    XT:
        Contains information about whether or not the word is
        immediate or primitive (or deleted)
    Data:
        In the case of a primitive word, a function pointer.
        In the case of a user-defined word, contains an array of
        XTs in the definition, terminated by a double null.
*/

void    primitive(const char *, void (*)(void), int);
void    create(void);
void    xt(void);
void    colon(void);
void    semicolon(void);
void    find(void);
void    forget(void);
void    align(void);
void    compile_xt(int_);
void    dp_(void);
void    lp_(void);

/* Given an address, finds the head of the closest overlying 
 * word definition, if none is found, returns NULL
 */
void *  trace(const int_ *);

#endif
