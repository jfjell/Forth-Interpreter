
char # constant alive
bl constant empty
80 constant width
24 constant height
width height * constant size
create screen size allot
create aux size allot

: clear size empty fill ;

: render size 0 do screen i chars + C@ emit loop cr ;

: index height mod 0 max swap width mod 0 max swap width * + ;

: alive? index screen + c@ alive = ;

: set index + c! ;

: #neighbours ( x y -- n )
    0 -rot
    2 -1 do 
        2 -1 do
            i 0<> j 0<> or if 
                2dup i + swap j + swap alive? if
                    rot 1+ -rot
                then
            then
        loop
    loop
    2drop
;

: update 
    aux clear
    height 0 do
        width 0 do
            i j #neighbours 
            dup 3 = if
                drop alive
            else 
                dup 2 < swap 3 > or if
                    empty
                else
                    i j index screen + c@
                then 
            then 
            aux i j set
        loop
    loop
    aux screen size move 
;

: init 
    screen clear
    aux clear
    \ Do a row of 10 pixels
    10 0 do
        alive screen 35 i + height 2 / set
    loop
;
    
: sleep 800000 0 do loop ;

: game-of-life
    init
    cr
    begin true while
        update
        render
        sleep
    repeat
;
