# simpleJavaCompiler

```WIP: this repo is still a work in progress```

This is a compiler for Apple Silicon arm64. The language specification for ```Simple Java``` is as defined by David Galles in his book ```Modern Compiler Design```: https://www.amazon.com/Modern-Compiler-Design-David-Galles/dp/1576761053

Starter code for this project can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
The main difference in this project from the book is that I have modified the compiler to compile to arm64 architecture.
With that being said, I also have made changes to adhere to the standard memory size for each primitive type:
```int: 4 bytes```
```boolean: 1 byte```
```char: 1 byte (chars have not been implemented yet)```
```pointers to memory: 8 bytes ie arrays or class objects```


Despite the name, ```Simple Java Compiler```, this is not a java compiler, which would compile to byte code that runs on the java virtual machine. This compiler has some keywords and syntax similar to java, and thus is probably where the name came from.

Dependency: ```bison v 3.8.3```, ```flex```

To build: ```cd src/build && make```
To build debug: ```cd src/build && make debug```
To compile a simple java program run src/build/sjc < file name >

Sample syntax can be found in test directory

```Warning!!!: this repo is not complete!!!```

Next Steps:
1. Add ```delete``` keyword, and implementation
    A. My main focus at the moment is working on the allocate and free functions that work under the hood with new and delete keywords. I am currently writing these functions in C, and I will use the assembly that the C files generates for the new and delete keywords in my compiler. I only just started these functions and I am still a far way from completion. The first heap implementation I am using is a free list that uses the first fit strategy. This is a linked list of free blocks in memory. I will elaborate more on the implementation when I am done.
2. Free memory from the AST in semantic.c in post order (Free AAT and all data structures as well)
    -> Memory clean up accross program
3. Optimzing assembly code with more sophisticated tiling (larger tiles).
4. Add ```char``` type, ```char[]```, and ```strings```
5. Optimize generated assembly
