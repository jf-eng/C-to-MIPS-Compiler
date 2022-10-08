# Description
This repository contains coursework submission for an Imperial College Module, Instruction Architecture and Compilers. The codebase implements a pre-processed ANSI C (C90) to MIPS1 assembly language compiler using C++ according to the specifications defined [here](https://github.com/LangProc/langproc-2021-cw/blob/master/c_compiler.md). 

Bash scripts were also written to automate testing against all of the seen test cases.

This submission scored 90% overall, passing 85% of all seen and unseen tests.

# Features
This compiler supports the following features.
### Basic
* a file containing just a single function with no arguments
* variables of `int` type
* local variables
* arithmetic and logical expressions
* if-then-else statements
* while loops
### Intermediate
* files containing multiple functions that call each other
* functions that take up to four parameters
* for loops
* arrays declared globally (i.e. outside of any function in your file)
* arrays declared locally (i.e. inside a function)
* reading and writing elements of an array
* recursive function calls
* the `enum` keyword
* `switch` statements
* the `break` and `continue` keywords
### Advanced
* variables of `double`, `float`, `char`, `unsigned`, structs, and pointer types
* calling externally-defined functions (i.e. the file being compiled declares a function, but its definition is provided in a different file that is linked in later on)
* functions that take more than four parameters
* mutually recursive function calls
* locally scoped variable declarations (e.g. a variable that is declared inside the body of a while loop, such as `while(...) { int x = ...; ... }`.
* the `typedef` keyword
* the `sizeof(...)` function (which takes either a type or a variable)
* taking the address of a variable using the `&` operator
* dereferencing a pointer-variable using the `*` operator
* pointer arithmetic
* character literals, including escape sequences like `\n`
* strings (as NULL-terminated character arrays)
* declaration and use of structs

# Usage
## Program Build
The compiler can be built using the following command in the root directory.

```bash
make bin/c_compiler
```

The program can then be invoked using the flag `-S`, with the source file and output file specified. The output file will contain the MIPS1 assembly code.

```bash
bin/c_compiler -S [source-file.c] -o [dest-file.s]
```
## Program Execution
To test the generated MIPS1 assembly code, the code is linked against a C run-time and executed on a MIPS processor emulated by `qemu-mips`. The following example usage is taken directly from the coursework specifications.

For instance, suppose I have a file called `test_program.c` that contains:

    int f() { return 10; }
    
and another file called `test_program_driver.c` that contains:

    int f();
    int main() { return !( 10 == f() ); }
    
I run the compiler on the test program, like so:

    bin/c_compiler -S test_program.c -o test_program.s
    
I then use GCC to assemble the generated assembly program (`test_program.s`), like so:

    mips-linux-gnu-gcc -mfp32 -o test_program.o -c test_program.s
    
I then use GCC to link the generated object file (`test_program.o`) with the driver program (`test_program_driver.c`), to produce an executable (`test_program`), like so:

    mips-linux-gnu-gcc -mfp32 -static -o test_program test_program.o test_program_driver.c

I then use QEMU to simulate the executable on MIPS, like so:

    qemu-mips test_program
    
This command should produce the exit code `0`.

## Testing
Bash scripts were written to automate testing of the compiler against all provided test cases. To use the script, run the following command in the root directory.
```bash
# USAGE: Supply file path to desired .c file for testing. File path can be either a folder or individual .c file
# If no file path given, all test cases will be executed.
./test.sh [INSERT OPTIONAL FILE PATH HERE]
```
# Contributors
- [Derek Lai](https://github.com/dereklai1)
- Eng Jian Fu
