# ALU-mak-leh
Expandability be damned, unleash the spaghetti

## Pipeline
the pipeline looks like this:
- load programs into memory (either by gui or from files)
- start one of them (depending on priority)
- fetch an instruction using the instruction memory class
- decode the instruction using the instruction manager class
- execute the instruction using the parser class
- repeat until the PC reaches a halt keyword (or something related to the limit, we are still gonna see how that works)
## General Notes
Whoever doesn't know how to use out_parameters here is an example:
```c
int main(void) {
    int int1 = 1;
    int int2;
    intcpy(int1, &int2);
    intcpy(int1, NULL); // if I don't care about this value
}

int strcpy(char *input, char *output) {
    // copy integer, return 0 on success
}
```
Also I know the program manager is way too complicated since there should only be one program per cycle in the memory, but I won't take any chances bsara7a, just save the limits of every program and use part 9 of the general description of part 1:![alt text](<assets/Project Description/image.png>)
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
EXCEPTION positive(int number) {
    if (number > 0) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}
```
A consequence of this is that functions that should also have return values will now return it as a pointer from the function's arguments

The reason we need a sophisticated error handling system is because we are required to handle errors as per the project description, fa m3lesh mafeesh return -1.

There is also this when it comes to failures:
```c
int main(void) {
    // ...

    if (FAIL(strcpy)) {
        // do something about failure
    }

    if (!FAIL(strcpy)) {
        // do something about success
    }

    // ...
}
```