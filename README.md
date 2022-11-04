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

To build: ```cd src/build && make```
To build debug: ```cd src/build && make debug```

```Warning!!!: this repo is not complete!!!```

Next Steps:
1. Add moves for boolean type (load/store bytes to/from memory)
2. Free memory from the AST in semantic.c in post order
3. heap allocation