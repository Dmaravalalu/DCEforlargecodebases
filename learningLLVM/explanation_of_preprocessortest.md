C and C++ have a preprocessor — a stupid text-substitution tool that runs before the actual compiler. It handles three things: #include (paste this file in here), macros (#define FOO 42 → replace FOO with 42 everywhere), and conditional compilation (#ifdef).

#ifdef X — true if X is defined
#ifndef X — true if X is not defined
#if EXPRESSION — true if expression evaluates non-zero, e.g. #if VERSION >= 3
#else, #elif, #endif — the obvious branching

The -D flag means "act as if #define HELLO appeared at the top of every file." That's how build systems control which features are compiled in.

The preprocessor checks: is the macro HELLO currently defined? If yes, the printf line stays. If no, the preprocessor deletes those lines from the source before the compiler sees them.

How does HELLO get defined? Two ways:

Somewhere in the code:                          #define HELLO
On the command line when invoking the compiler: clang -DHELLO file.c

there are two binary executables as these commands were run:
clang -DHELLO preprocessortest.c -o preprocessortest2
clang preprocessortest.c -o preprocessortest

run ./preprocessortest or ./preprocessortest2 to see the difference in how it was compiled.