# ALU-mak-leh
Expandability be damned, unleash the spaghetti

## General Notes

## Asset Notes
[ These are files like images and videos that don't include code ]

## SRC Notes
[ These only apply to files in src ] Every .c file should include at least these 2 things:
```c
#include "includes.h"
#include "abc.h" // abc being the .c file name
```
Every .h file shouldn't include anything, unless we would add something extra ourselves like uint8_t which would require importing certain libraries

you should type
```c
make
```
in the terminal to compile, and then type 
```c
./CA_backend.exe
```
to run the program itself, the make program works by itself _(mahadesh yetfazlek w yghayyar fel Makefile, ha2telo)_