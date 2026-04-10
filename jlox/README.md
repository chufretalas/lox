# jlox

## Usage

### REPL mode
```bash
./gradlew --console=plain run
```

### File mode
```bash
./gradlew --console=plain run --args='../../sample_scripts/script.lox'
```

### Running AST Generator
```bash
javac app/src/main/java/dev/chufretalas/tool/GenerateAst.java && java -cp app/src/main/java/ dev.chufretalas.tool.GenerateAst app/src/main/java/dev/chufretalas/lox
```

## jlox grammar

```
program          → declaration* EOF ;

declaration      → classDecl | funDecl | varDecl | statement ;

classDecl        → "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function* "}" ;

funDecl          → "fun" function ;

function         → IDENTIFIER "(" parameters? ")" block ;

parameters       → IDENTIFIER ( "," IDENTIFIER )* ;

varDecl          → "var" IDENTIFIER ( "=" expression )? ";" ;

statement        → exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block ;

ifStmt           → "if" "(" expression ")" statement ( "else" statement )? ;

whileStmt        → "while" "(" expression ")" statement ;

forStmt          → "for" "(" ( varDecl | exprStmt | ";" )
                   expression? ";"
                   expression? ")" statement ;

block            → "{" declaration* "}" ;

printStmt        → "print" expression ";" ;

returnStmt       → "return" expression? ";" ;

breakStmt        → "break" ";"
continueStmt     → "continue" ";"

exprStmt         → expression ";" ;

expression       → comma_expression ;
comma_expression → assignment ( "," assignment )* ;
assignment       → ( call "." )? IDENTIFIER "=" assignment | ternary ;
ternary          → logic_or ( "?" expression ":" ternary )? ;
logic_or         → logic_and ( "or" logic_and )* ;
logic_and        → equality ( "and" equality )* ;
equality         → comparison ( ( "!=" | "==" ) comparison )* ;
comparison       → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term             → factor ( ( "-" | "+" ) factor )* ;
factor           → unary ( ( "/" | "*" ) unary )* ;
unary            → ( "!" | "-" ) unary | call ;
call             → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
primary          → "true" | "false" | "nil"
                  | NUMBER | STRING
                  | "(" expression ")"
                  | IDENTIFIER
                  | anonFunction
                  | "super" "." IDENTIFIER ;
anonFunction     → "fun" "(" parameters? ")" block ;
                  
arguments        → assignment ( "," assignment )* ;
```

## Error productions

```
e_leading_operator → ( "," ternary)
                    | ( "?" expression ":" ternary )
                    | ( ":" ternary )
                    | ( "!=" | "==" ) comparison
                    | ( ( ">" | ">=" | "<" | "<=" ) term )
                    | (  "+" factor )
                    | ( ( "/" | "*" ) unary );
```
