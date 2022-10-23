# simpleJavaCompiler

```WIP: this repo is still a work in progress```

This is a compiler for Apple Silicon arm64. The language specification for ```Simple Java``` is as defined by David Galles in his book ```Modern Compiler Design```: https://www.amazon.com/Modern-Compiler-Design-David-Galles/dp/1576761053

Starter code for this project can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
The main difference in this project from the book is that I have modified it to compile to arm64 architecture.

To build: ```cd src/build && make```
To build debug: ```cd src/build && make debug```

```Warning!!!: this repo is not complete!!!```

Next Steps:
1. Print integer function
2. Free memory from the AST in semantic.c in post order