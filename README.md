# PL-0 Compiler

## Introduction

This project is a compiler for the PL/0 programming language. It was written entirely in C.  

This was a semester-long project for the COP 3402 *(Systems Software)* class at UCF taught by Dr. Euripides Montagne.

## Function

This PL/0 compiler includes a lexical analyzer, parser (syntax analyzer), machine code generator. It also includes a PM/0 virtual machine that is designed to support features such as function calls, passing parameters, and recursion.  

Given a program written in PL/0 programming language, the compiler will check the syntax, generate a symbol table, generate machine code, and finally run the machine code.

## Usage

1. [Download the project](#how-to-download) to your computer. 
2. In the linux command line, navigate to the ``PL0-Compiler`` directory that you have downloaded *(or cloned)*.
3. From there, navigate to the compiler directory using `cd Compiler`
4. To compile:
    - Run the following command: ``make``
    - Alternatively use: ``gcc driver.c lex.c vm.c parser.c -lm``
5. To run:
    - Run the following command: `./a.out ./Input\ Files/[Input File Name] [Directives]`

### Directives

The directives supported by the compiler are as follows:

```
  -l    Print out the generated lexeme list
  -a    Print out the generated assembly language code
  -v    Print out the stack trace of the virtual machine execuation
  -s    Print out the generated symbol table
```

## How to Download

There are two ways this can be done:
1. ~~Download the project from this link~~ ***(Not yet available)***
2. Use the command ``git clone https://github.com/NaimShaqqou/PL0-Compiler``

## More About PL/0

We are using the "tiny" PL/0 language for this compiler. It is a very simple programming language that is used for learning the details of constructing a compiler. The compiler functions as a recursive descent parser *(this can be visualized by looking at the grammar below)*.

The compiler follows the EBNF shown as follows:
```
  program ::= block "." . 
  block ::= const-declaration  var-declaration  procedure-declaration statement.	
  const-declaration ::= ["const" ident ":=" number {"," ident ":=" number} ";"].	
  var-declaration  ::= [ "var "ident {"," ident} “;"].
  procedure-declaration ::= { "procedure" ident ";" block ";" }.
  statement   ::= [ ident ":=" expression
                  | "call" ident  
	      	        | "begin" statement { ";" statement } "end" 
	      	        | "if" condition "then" statement ["else" statement]
		              | "while" condition "do" statement
		              | "read" ident
		              | "write" expression
	      	        | ε ] .  
  condition ::= "odd" expression 
	  	          | expression  rel-op  expression.  
  rel-op ::= "=="|“!="|"<"|"<="|">"|">=“.
  expression ::= [ "+"|"-"] term { ("+"|"-") term}.
  term ::= factor {("*"|"/"|”%”) factor}. 
  factor ::= ident | number | "(" expression ")“ .
  number ::= digit {digit}.
  ident ::= letter {letter | digit}.
  digit ;;= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9“.
  letter ::= "a" | "b" | … | "y" | "z" | "A" | "B" | ... | "Y" | "Z".
```
