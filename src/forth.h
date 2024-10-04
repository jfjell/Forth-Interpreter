#ifndef FORTH_H_
#define FORTH_H_

#include <setjmp.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#ifndef PAD_CHARS
# define PAD_CHARS 256
#endif
#ifndef CSTRING_MAX_CHARS
# define CSTRING_MAX_CHARS 128
#endif
#ifndef CSTRING_MAX_CHARS_MASK
# define CSTRING_MAX_CHARS_MASK 0x7f
#endif 

/* Single celled numbers are defined to be the same size 
 * as pointers
 */
typedef intptr_t     int_;
typedef uintptr_t    uint_;
#define MAX_N        INTPTR_MAX
#define MAX_UN       UINTPTR_MAX

/* If word size is larger than 64 bits, then double
 * cell numbers are the same size as single cell numbers
 */
#if     (INT16_MAX > MAX_N)
 typedef int16_t  long_;
 typedef uint16_t ulong_;
#elif   (INT32_MAX > MAX_N)
 typedef int32_t  long_;
 typedef uint32_t ulong_;
#elif (INT64_MAX > MAX_N)
 typedef int64_t  long_;
 typedef uint64_t ulong_;
#else /* Largest size possible is 64 bits */
 typedef int64_t  long_;
 typedef uint64_t ulong_;
# define MAX_D MAX_N
# define MAX_UD MAX_UN
#endif

#define CELL      (sizeof(int_))
#ifndef MAX_D
# define MAX_D     (((long_)MAX_N   << (CELL*CHAR_BIT)) | MAX_UN)
#endif
#ifndef MAX_UD
# define MAX_UD    (((ulong_)MAX_UN << (CELL*CHAR_BIT)) | MAX_UN)
#endif

#define err(...) do { \
        printf(__VA_ARGS__); \
        except(); \
    }while(0)

enum forth_bool
{
    FALSE = 0,
    TRUE  = ~0
};

enum interp_state
{
    INTERPRET = FALSE,
    COMPILE   = TRUE
};

enum xt_flag
{
    DELETED   = 1 << 7,
    PRIMITIVE = 1 << 15,
    IMMEDIATE = 1 << 14,
};

extern char          pad[PAD_CHARS];
extern int_          state;
extern int_          base;
extern const char    *file;
extern int           line;

void    reset(void);
void    except(void);
void    abort_(void);
void    interpret(void);
void    environment(void);
void    execute(void);
void    literal(void);
void    jmp_if_zero(void);
void    jmp(void);
void    abort_(void);
void    cell(void);
void    state_(void);
void    base_(void);
void    bye(void);
void    number(void);
void    quit(void);

char *  c_to_cnt(const char *);
char *  cnt_to_c(const char *);
int     cnt_strcmp(const char *, const char *);

#endif
