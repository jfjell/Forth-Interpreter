: fact ?dup 0= if 1 else dup 1- recurse * then ;
: fib dup 1 > if 1- dup 1- recurse swap recurse + then ;
