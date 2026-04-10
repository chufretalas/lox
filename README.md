# Lox

This code comes from my readthrough of [Crafting Interpreters by Robert Nystrom](https://craftinginterpreters.com/), a fantastic book about building programming languages.

This implements the lox language in two ways: jlox and clox.

Feel free to peruse my implementations as you wish. I'll be using this repository as reference for future projects and since most of the code from this repository was taken directly from the book, I see no problem in other people using it as reference as well.
 
## jlox

jlox is a java based three-walker implementation of lox. 

I did most optional challenges for jlox, so it includes aditional features not present in the base implementation of the book, such as: 
- break and continue
- ternary operators
- anonymous functions
- comma expressions
- multiline comments

## clox

clox is a C stack-machine implementation of lox.

Unfortunetly I wasn't able to do as many optional challenges as I did for jlox, but it includes: 
- in-place stack optimizations for some arithmetic operators 
- support for 24 bit indexing for constants (thus the plethora of _LONG instructions)