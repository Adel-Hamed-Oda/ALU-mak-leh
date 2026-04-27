# ALU-mak-leh
Expandability be damned, unleash the spaghetti

## General Notes
Whoever doesn't know how to use out_parameters here is an example:
```c
int main(void) {
    int int1 = 1;
    int int2;
    intcpy(int1, &int2);
}

void strcpy(char *input, char *output) {
    // copy integer
}
```

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

Any function that should return errors should use the definitions in "errors.h", this would help in modularity and would make it easier to check from something like
```c
int positive(int number) {
    if (number > 0) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}
```
A consequence of this is that functions that should also have return values will now return it as a pointer from the function's arguments