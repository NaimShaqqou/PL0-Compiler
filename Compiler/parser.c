/*******************************************************
 * Homework #3 (PL/0-Compiler)
 *
 * Created by: Naim Shaqqou & Blake Douglas
 *
 * COP3402, Fall 2021   Instructor: Euripides Montagne
 *******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex;
symbol *table;
int tIndex;

int level; // current level
lexeme cur; // current token
int curIndex = 0;
int error = 0;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// helper functions to implement parser
void program(lexeme *list);
void block(lexeme *list);
void const_declaration(lexeme *list);
int var_declaration(lexeme *list);
void procedure_declaration(lexeme *list);
void statement(lexeme *list);
void mark();
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);

// helper functions
// Returns if symbol should be marked or not
void mark()
{
	for (int i = tIndex - 1; i > -1; i--)
	{
		if(table[i].mark)
			continue;

			if (table[i].mark == 0 && table[i].level < level)
				return;

		if (table[i].level == level)
		{
			table[i].mark = 1;
		}
	}
}

// Checks if var has been declared before
int multipleDeclarationCheck(lexeme cur)
{
	for (int i = 0; i < tIndex; i++)
	{
		if (strcmp(cur.name, table[i].name) == 0)
		{
			if (table[i].mark == 0 && table[i].level == level)
				return i;
		}
	}
	// found nothing
	return -1;
}

// Searches table for specified symbol and returns its index
int findSymbol(lexeme cur, int kind)
{
	int max = -1;

	// linear pass through table
	for (int i = 0; i < tIndex; i++)
	{
		if (strcmp(table[i].name, cur.name) == 0 && table[i].kind == kind && table[i].mark == 0)
			if (max == -1)
				max = i;
			else if(table[i].level > table[max].level)
				max = i;
	}
	return max;
}

// Gets next token in list
int next(lexeme *list)
{
	curIndex++;

	size_t n = sizeof(list) / sizeof(list[0]);

	if (curIndex == n)
	{
		return 0;
	}
	else
	{
		cur = list[curIndex];
		return 1;
	}
}


instruction *parse(lexeme *list, int printTable, int printCode)
{
	// Allocate max space for input and symbols
	code = (instruction *)malloc(MAX_CODE_LENGTH * sizeof(instruction));
	table = (symbol *)malloc(MAX_SYMBOL_COUNT * sizeof(symbol));

	tIndex = cIndex = 0;
	cur = list[0];

	// Adds lexeme list to symbol table
	program(list);

	if (error == 1)
		return NULL;
	// Prints symbol table if called
	if (printTable)
		printsymboltable();
	// Prints assembly code if called
	if(printCode)
		printassemblycode();

	/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
		when you test your code otherwise IT WILL SEGFAULT in
		vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
		WHEN COPYING IT TO THE PAS */
	code[cIndex].opcode = -1;

	return code;
}

// beginning of grammar
void program (lexeme *list)
{
	// emit JMP
	emit(7, 0, 0);

	addToSymbolTable(3, "main", 0, 0, 0, 0);

	level = -1;

	block(list);
	if (error == 1)
		return;

	if (cur.type != periodsym) // error 1
	{
		printparseerror(1);
		error = 1;
		return;
	}

	// emit halt
	emit(9, 0, 3);

	// fix addresses
	for (int line = 0; line < cIndex; line++)
		if (code[line].opcode == 5)
			code[line].m = table[code[line].m].addr;
	code[0].m = table[0].addr;
}

void block(lexeme *list)
{
	level++;
	int procedure_idx = tIndex - 1;

	const_declaration(list);
	if (error == 1)
		return;

	int x = var_declaration(list);
	if (error == 1)
		return;

	procedure_declaration(list);
	if (error == 1)
		return;

	table[procedure_idx].addr = cIndex * 3;

	if (level == 0)
	{
		// emit INC
		emit(6, 0, x);
	}
	else
	{
		emit(6, 0, x + 4);
	}

	statement(list);
	if (error == 1)
		return;

	mark();

	level--;
}

// Checks for constants in grammar
void const_declaration(lexeme *list)
{
	if (cur.type == constsym)
	{
		do {
			next(list);
			if (cur.type != identsym)
			{
				printparseerror(2);
				error = 1;
				return;
			}

			int symidx = multipleDeclarationCheck(cur);
			if (symidx != -1)
			{
				printparseerror(18);
				error = 1;
				return;
			}

			char identName[12];
			strcpy(identName, cur.name);

			next(list);

			if (cur.type != assignsym)
			{
				printparseerror(2);
				error = 1;
				return;
			}

			next(list);

			if (cur.type != numbersym)
			{
				printparseerror(2);
				error = 1;
				return;
			}

			addToSymbolTable(1, identName, cur.value, level, 0, 0);
			next(list);
		} while (cur.type == commasym);

		if (cur.type != semicolonsym)
		{
			if (cur.type == identsym) // error 13
			{
				printparseerror(13);
			} else { // error 14
				printparseerror(14);
			}

			error = 1;
			return;
		}

		next(list);
	}
}

// Checks grammar for variable names
int var_declaration(lexeme *list)
{
	int numVars = 0;
	if (cur.type == varsym)
	{
		do {
			numVars++;
			next(list);

			if (cur.type != identsym)
			{
				printparseerror(3);
				error = 1;
				return 0;
			}

			int symidx = multipleDeclarationCheck(cur);
			if (symidx != -1) // error 18
			{
				printparseerror(18);
				error = 1;
				return 0;
			}

			if (level == 0)
				addToSymbolTable(2, cur.name, 0, level, numVars - 1, 0);
			else
				addToSymbolTable(2, cur.name, 0, level, numVars + 3, 0);

			next(list);

		} while (cur.type == commasym);

		if (cur.type != semicolonsym)
		{
			if (cur.type == identsym) // error 13
			{
				printparseerror(13);
			} else { // error 14
				printparseerror(14);
			}

			error = 1;
			return 0;
		}
		next(list);
	}
	// Returns total number of identifiers
	return numVars;
}

// Checks grammar for procedure calls
void procedure_declaration(lexeme *list)
{
	while (cur.type == procsym)
	{
		next(list);

		if (cur.type != identsym)
		{
			printparseerror(4);
			error = 1;
			return;
		}

		int symidx = multipleDeclarationCheck(cur);
		if (symidx != -1) // error 18
		{
			printparseerror(18);
			error = 1;
			return;
		}

		addToSymbolTable(3, cur.name, 0, level, 0, 0);
		next(list);

		if (cur.type != semicolonsym) // error 4
		{
			printparseerror(4);
			error = 1;
			return;
		}

		next(list);

		block(list);
		if (error == 1)
			return;

		if (cur.type != semicolonsym) // error 14
		{
			printparseerror(14);
			error = 1;
			return;
		}

		next(list);

		// emit RTN
		emit(2, 0, 0);
	}
}

// Checks validity of a statement in the grammar
void statement(lexeme *list)
{
	// Clarifies identifier order
	if (cur.type == identsym)
	{
		int symIdx = findSymbol(cur, 2);
		if (symIdx == -1)
		{
			if (findSymbol(cur, 1) != findSymbol(cur, 3))
			{
				printparseerror(6);
			} else {
				printparseerror(19);
			}

			error = 1;
			return;
		}

		next(list);

		if (cur.type != assignsym) // error 5
		{
			printparseerror(5);
			error = 1;
			return;
		}

		next(list);

		expression(list);
		if (error == 1)
			return;

		// emit STO
		emit (4, level - table[symIdx].level, table[symIdx].addr);
		return;
	}
	// Checks order of beginsym
	if (cur.type == beginsym)
	{
		do {
			next(list);
			statement(list);
			if (error == 1)
				return;
		} while (cur.type == semicolonsym);

		if (cur.type != endsym)
		{
			// error 15
			if (cur.type == identsym || cur.type == beginsym || cur.type == ifsym || cur.type == whilesym || cur.type == readsym || cur.type == writesym ||cur.type == callsym)
			{
				printparseerror(15);
			} else { // error 16
				printparseerror(16);
			}

			error = 1;
			return;
		}

		next(list);
		return;
	}
	// Checks ordering of if statement
	if (cur.type == ifsym)
	{
		next(list);
		condition(list);
		if (error == 1)
			return;

		int jpcIdx = cIndex;

		// emit JPC
		emit (8, 0, 0);

		if (cur.type != thensym) // error 8
		{
			printparseerror(8);
			error = 1;
			return;
		}

		next(list);

		statement(list);
		if (error == 1)
			return;

		if (cur.type == elsesym)
		{
			int jmpIdx = cIndex;

			// emit JMP
			emit (7, 0, 0);

			code[jpcIdx].m = cIndex * 3;
			next(list);
			statement(list);
			if (error == 1)
				return;

			code[jmpIdx].m = cIndex * 3;
		} else {
			code[jpcIdx].m = cIndex * 3;
		}

		return;
	}
	// Checks order of while statement
	if (cur.type == whilesym)
	{
		next(list);

		int loopIdx = cIndex;

		condition(list);
		if (error == 1)
			return;

		if (cur.type != dosym)
		{
			printparseerror(9);
			error = 1;
			return;
		}

		next(list);

		int jpcIdx = cIndex;

		//emit JPC
		emit (8, 0, jpcIdx);

		statement(list);
		if (error == 1)
			return;

		// emit jmp
		emit(7, 0, loopIdx * 3);

		code[jpcIdx].m = cIndex * 3;

		return;
	}
	// Checks order of readsym
	if (cur.type == readsym)
	{
		next(list);
		if (cur.type != identsym)
		{
			printparseerror(5);
			error = 1;
			return;
		}

		int symIdx = findSymbol(cur, 2);
		if (symIdx == -1)
		{
			if (findSymbol(cur, 1) != findSymbol(cur, 3))
				printparseerror(6);
			else
				printparseerror(19);

			error = 1;
			return;
		}

		next(list);

		// emit READ
		emit(9, 0, 2);

		// emit STO
		emit (4, level - table[symIdx].level, table[symIdx].addr);

		return;
	}
	// Checks order of writesym
	if (cur.type == writesym)
	{
		next(list);
		expression(list);
		if (error == 1)
			return;

		// emit write
		emit(9, 0, 1);
	}
	// Checks order of callsym
	if (cur.type == callsym)
	{
		next(list);
		int symIdx = findSymbol(cur, 3);
		if (symIdx == -1)
		{
			if (findSymbol(cur, 1) != findSymbol(cur, 2))
				printparseerror(7);
			else
				printparseerror(19);

			error = 1;
			return;
		}

		next(list);

		// emit CAL
		emit (5, level - table[symIdx].level, symIdx);
	}
}

void condition(lexeme *list)
{
	if (cur.type == oddsym)
	{
		next(list);
		expression(list);
		if (error == 1)
			return;

		// emit odd
		emit(2, 0, 6);
	} else {
		expression(list);
		if (error == 1)
			return;

		if (cur.type == eqlsym)
		{
			next(list);
			expression(list);
			if (error == 1)
				return;

			// emit eql
			emit(2, 0, 8);
		} else if (cur.type == neqsym) {
			next(list);
			expression(list);
			if (error == 1)
				return;

			// emit neq
			emit(2, 0, 9);
		} else if (cur.type == lsssym) {
			next(list);
			expression(list);
			if (error == 1)
				return;

			// emit lss
			emit(2, 0, 10);
		} else if (cur.type == leqsym) {
			next(list);
			expression(list);
			if (error == 1)
				return;

			// emit leq
			emit (2, 0, 11);
		} else if (cur.type == gtrsym) {
			next(list);
			expression(list);
			if (error == 1)
				return;

			// emit gtr
			emit (2, 0, 12);
		} else if (cur.type == geqsym) {
			next(list);
			expression(list);
			if (error == 1)
				return;

			emit(2, 0, 13);
		} else {
			printparseerror(10);
			error = 1;
			return;
		}
	}
}

void expression(lexeme *list)
{
	if (cur.type == subsym)
	{
		next(list);

		term(list);
		if (error == 1)
			return;

		// emit neg
		emit(2, 0, 1);

		while (cur.type == addsym || cur.type == subsym)
		{
			if (cur.type == addsym)
			{
				next(list);
				term(list);
				if (error == 1)
					return;

				// emit add
				emit(2, 0, 2);
			} else {
				next(list);
				term(list);
				if (error == 1)
					return;

				// emit sub
				emit(2, 0, 3);
			}
		}
	} else {
		if (cur.type == addsym)
		{
			next(list);
		}

		term(list);
		if (error == 1)
			return;
		while (cur.type == addsym || cur.type == subsym)
		{
			if (cur.type == addsym)
			{
				next(list);
				term(list);
				if (error)
					return;

				// emit add
				emit(2, 0, 2);
			} else {
				next(list);
				term(list);
				if (error)
					return;

				// emit sub
				emit(2, 0, 3);
			}
		}
	}

	if (cur.type == lparensym || cur.type == addsym || cur.type == subsym || cur.type == multsym || cur.type == modsym || cur.type == divsym)
	{
		printparseerror(17);
		error = 1;
		return;
	}
}

void term(lexeme *list)
{
	factor(list);
	if (error)
		return;

	while (cur.type == multsym || cur.type == divsym || cur.type == modsym)
	{
		if (cur.type == multsym)
		{
			next(list);
			factor(list);
			if(error)
				return;

			// emit mul
			emit(2, 0, 4);
		} else if (cur.type == divsym) {
			next(list);
			factor(list);
			if (error)
				return;

			// emit div
			emit(2, 0, 5);
		} else {
			next(list);
			factor(list);
			if (error)
				return;

			// emit mod
			emit(2, 0, 7);
		}
	}
}

void factor(lexeme *list)
{
	if (cur.type == identsym)
	{
		int symIdx_var = findSymbol(cur, 2);
		int symIdx_const = findSymbol(cur, 1);
		if (symIdx_var == -1 && symIdx_const == -1)
		{
			if (findSymbol(cur, 3) != -1)
				printparseerror(11);
			else
				printparseerror(19);

			error = 1;
			return;
		}

		if (symIdx_var == -1)
		{
			// emit lit
			emit(1, 0, table[symIdx_const].val);
		} else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level) {
			// emit lod
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr);
		} else {
			// emit lit
			emit(1, 0, table[symIdx_const].val);
		}
		next(list);
	} else if (cur.type == numbersym) {
		// emit lit
		emit(1, 0, cur.value);
		next(list);
	} else if (cur.type == lparensym) {
		next(list);
		expression(list);
		if (error)
			return;

		if (cur.type != rparensym)
		{
			printparseerror(12);
			error = 1;
			return;
		}
		next(list);
	} else {
		printparseerror(11);
		error = 1;
		return;
	}
}

// Sets data to the struct for VM
void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// Adds info to symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// Prints all error codes
void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

// Prints the formatted symbol table
void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}
// Prints formatted assembly code below symbol table
void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		// Find OP name from OP code
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			// Find math op from m code if op = 2
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
