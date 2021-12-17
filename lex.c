/*****************
 * Group Members:
 * Naim Shaqqou
 * Blake Douglas
 *****************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

// list of acceptable symbols


// helper function that takes in a string and returns 1 if it contains only numbers
int containsNumbers(char * in, int arrSize)
{
	for (int i = 0; i < arrSize; i++)
	{
		if(!isdigit(in[i]))
			return 0;
	}

	return 1;
}

// helper function to determine type of lexeme
token_type typeOfLex(char * in, int arrSize)
{
	if (strcmp(in, "const") == 0)
		return constsym;
	else if (strcmp(in, "var") == 0)
		return varsym;
	else if (strcmp(in, "procedure") == 0)
		return procsym;
	else if (strcmp(in, "begin") == 0)
		return beginsym;
	else if (strcmp(in, "end") == 0)
		return endsym;
	else if (strcmp(in, "while") == 0)
		return whilesym;
	else if (strcmp(in, "do") == 0)
		return dosym;
	else if (strcmp(in, "if") == 0)
		return ifsym;
	else if (strcmp(in, "then") == 0)
		return thensym;
	else if (strcmp(in, "else") == 0)
		return elsesym;
	else if (strcmp(in, "call") == 0)
		return callsym;
	else if (strcmp(in, "write") == 0)
		return writesym;
	else if (strcmp(in, "read") == 0)
		return readsym;
	else if (strcmp(in, ":=") == 0)
		return assignsym;
	else if (strcmp(in, "+") == 0)
		return addsym;
	else if (strcmp(in, "-") == 0)
		return subsym;
	else if (strcmp(in, "*") == 0)
		return multsym;
	else if (strcmp(in, "/") == 0)
		return divsym;
	else if (strcmp(in, "%") == 0)
		return modsym;
	else if (strcmp(in, "==") == 0)
		return eqlsym;
	else if (strcmp(in, "!=") == 0)
		return neqsym;
	else if (strcmp(in, "<") == 0)
		return lsssym;
	else if (strcmp(in, "<=") == 0)
		return leqsym;
	else if (strcmp(in, ">") == 0)
		return gtrsym;
	else if (strcmp(in, ">=") == 0)
		return geqsym;
	else if (strcmp(in, "odd") == 0)
		return oddsym;
	else if (strcmp(in, "(") == 0)
		return lparensym;
	else if (strcmp(in, ")") == 0)
		return rparensym;
	else if (strcmp(in, ",") == 0)
		return commasym;
	else if (strcmp(in, ".") == 0)
		return periodsym;
	else if (strcmp(in, ";") == 0)
		return semicolonsym;
	else if (containsNumbers(in, arrSize))
		return numbersym;
	else
		return identsym;
}


lexeme *lexanalyzer(char *input, int printFlag)
{

	// initialize the index for lexemes
	lex_index = 0;
	list = (lexeme *)malloc(MAX_NUMBER_TOKENS * sizeof(lexeme));

	int j = 0;

	for (int i = 0; i < strlen(input); i++)
	{
		// skip whitespace and spaces
		if (iscntrl(input[i]) || input[i] == ' ')
		{
			continue;
		}

		// if it is a comment
		if(i < strlen(input) - 1 && input[i] == '/' && input[i + 1] == '/')
		{
			// first white space is a newline
			while(!iscntrl(input[i]))
			{
				i++;
			}
		}

		// token starting with letter
		if(isalpha(input[i]))
		{
			// Max identifier is length 11 and resets the char array
			char identifier[12] = "";
			j = 0;
			// keep reading until we hit a non alphanumeric character
			while(isalpha(input[i]) || isdigit(input[i]))
			{
				// check if identifier is longer than 11
				if(j + 1 == MAX_IDENT_LEN)
				{
					printlexerror(4);
					return NULL;
				}

				// place character in identifier string
				identifier[j++] = input[i];

				// check if safe to move to next index
				if(i + 1 < strlen(input) && isdigit(input[i+1]) || isalpha(input[i+1]))
					i++;
				else
					break;
			}

			// check for symbol and add to lexeme list
			// then move to next index
			list[lex_index].type = typeOfLex(identifier, sizeof(identifier));

			// if it is an identifier, we need the name
			if(list[lex_index].type == identsym)
				strcpy(list[lex_index].name, identifier);
			// Reset identifier array
			//char identifier[12] = "";
			lex_index++;
			continue;
		}

		// token starting with number
		else if(isdigit(input[i]))
		{
			// Max number length is 5 and resets number array
			char number[6] = "";
			j = 0;

			// keep reading until non-numeric character
			while(isdigit(input[i]))
			{
				// place number in string
				number[j] = input[i];

				// check if safe to move to next index
				if(i + 1 < strlen(input) && isdigit(input[i+1]))
					i++;
				else
					break;

					// check if number is longer than 5
					if(j + 1 == MAX_NUMBER_LEN)
					{
						printlexerror(3);
						return NULL;
					}
					else
						j++;
			}

			// if number followed by letter then its an error
			if(isalpha(input[i+1]))
			{
				printlexerror(2);
				return NULL;
			}

			list[lex_index].type = numbersym;
			list[lex_index].value = atoi(number);
			lex_index++;
			continue;
		}

		// token starting with symbol
		else
		{
			// symbols that are single characters
			if(input[i] == '(' || input[i] == ')' || input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '%' || input[i] == ',' || input[i] == '.' || input[i] == ';')
			{
				char symbol[2];
				symbol[0] = input[i];
				symbol[1] = '\0';
				list[lex_index].type = typeOfLex(symbol, sizeof(symbol));
				lex_index++;
				continue;
			}

			// less than and less than or equal
			if(input[i] == '<')
			{
				if(i + 1 < strlen(input) && input[i + 1] == '=')
				{
					list[lex_index].type = typeOfLex("<=", 2);
					i++;
				} else if( (i + 1 < strlen(input) && (input[i + 1] == ' ' || isalpha(input[i + 1]) || isdigit(input[i + 1]))) || i+1 < strlen(input)){
					list[lex_index].type = typeOfLex("<", 1);
				} else {
					printlexerror(1);
					return NULL;
				}

				lex_index++;
				continue;
			}

			// greater than and greater than or equal
			if(input[i] == '>')
			{
				if(i + 1 < strlen(input) && input[i + 1] == '=')
				{
					list[lex_index].type = typeOfLex(">=", 2);
					i++;
				} else if( (i + 1 < strlen(input) && (input[i + 1] == ' ' || isalpha(input[i + 1]) || isdigit(input[i + 1]))) || i+1 < strlen(input)) {
					list[lex_index].type = typeOfLex(">", 1);
				} else {
					printlexerror(1);
					return NULL;
				}

				lex_index++;
				continue;
			}

			// not equal
			if(input[i] == '!')
			{
				if(i + 1 < strlen(input) && input[i + 1] == '=')
				{
					list[lex_index].type = typeOfLex("!=", 2);
					i++;
				} else if( (i + 1 < strlen(input) && (input[i + 1] == ' ' || isalpha(input[i + 1]) || isdigit(input[i + 1]))) || i+1 < strlen(input)) {
					list[lex_index].type = typeOfLex("!", 1);
				} else {
					printlexerror(1);
					return NULL;
				}

				lex_index++;
				continue;
			}

			// equals
			if(input[i] == '=')
			{
				if(i + 1 < strlen(input) && input[i + 1] == '=')
				{
					list[lex_index].type = typeOfLex("==", 2);
					i++;
				} else {
					printlexerror(1);
					return NULL;
				}

				lex_index++;
				continue;
			}

			// assign
			if(input[i] == ':')
			{
				if(i + 1 < strlen(input) && input[i + 1] == '=')
				{
					list[lex_index].type = typeOfLex(":=", 2);
					i++;
				} else {
					printlexerror(1);
					return NULL;
				}

				lex_index++;
				continue;
			}
		}


	}

	if (printFlag)
		printtokens();

	return list;
}


void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
