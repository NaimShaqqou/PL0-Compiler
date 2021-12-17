/*******************************************************
 * Homework #1 (PM/0-Machine)
 *
 * Created by: Naim Shaqqou & Blake Douglas
 *
 * COP3402, Fall 2021   Instructor: Euripides Montagne
 *******************************************************/

// Import statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Constants
#define MAX_PAS_LENGTH 3000

// create a struct for the IR
typedef struct IR
{
  int op; // opcode
  int l; // lexicographical level or register
  int m; // address, register, or number
}IR;


/****************************
 * Function Implementations *
 ***************************/

// Given print function
void print_execution(int line, char *opname, IR ir, int PC, int BP, int SP, int DP, int *pas, int GP)
{
  int i;// print out instruction and registers
  printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, ir.l, ir.m, PC, BP, SP, DP);
  // print data section
  for (i = GP; i <= DP; i++)
    printf("%d ", pas[i]);
  printf("\n");
  // print stack
  printf("\tstack : ");
  for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
    printf("%d ", pas[i]);
  printf("\n");
}

// Given to find base L levels down
int base(int L, int BP, int *pas)
{
	int arb = BP;	// arb = activation record base
	while (L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

void execute_program(instruction *code, int printFlag)
{
  // Instruction counter - Used to load the program and track number of instructions
  int ic = 0;
  int cIndex = 0;

  // Dynamically allocates pas, initializing it with all 0's
  int *pas = (int *)calloc(MAX_PAS_LENGTH, sizeof(int));

  while (code[cIndex].opcode != -1)
  {
    pas[ic++] = code[cIndex].opcode;
    pas[ic++] = code[cIndex].l;
    pas[ic++] = code[cIndex].m;
    cIndex++;
  }


  // Initializing variables for fetch/execute cycle
  // Global Pointer - Points to data segment
  int gp = ic;

  // Data Pointer - access variables in main
  int dp = ic - 1;

  // Heap Pointer/free - points to heap
  int hp = ic + 40;

  // Base pointer - Points to base of DATA or activation records
  int bp = ic;

  // Program counter
  int pc = 0;

  // Stack pointer - Points to top of Stack
  int sp = MAX_PAS_LENGTH;

  // halt flag to end the program
  int flag = 1;

  // stores the opname of the operation
  char opname[4];

  if (printFlag)
  {
    // print out headers
    printf("  \t   \t\t\tPC\tBP\tSP\tDP\tdata\n");

    // print out initial values
    printf("Initial values:  \t \t%d\t%d\t%d\t%d\n", pc, bp, sp, dp);
  }

  // Creating instruction register
  IR ir;

  // loop for fetch/execute cycle
  while(flag == 1)
  {
    // Fetch current instruction from pas array (fetch cycle)
    ir.op = pas[pc];
    ir.l = pas[pc+1];
    ir.m = pas[pc+2];

    // Holds current line number
    int line = pc / 3;
    // 3 memory locations for each instruction
    pc += 3;

    // Handles all ISA functions based on the pseudocode
    // provided in Appendix B
    switch(ir.op)
    {

      // LIT 0, M Pushes M onto the stack. Must do sp + 1 if room on stack
      case 1:
        strcpy(opname, "LIT");
        if (bp == gp)
        {
          dp = dp + 1;
          pas[dp] = ir.m;
        }
        else
        {
          sp = sp - 1;
          pas[sp] = ir.m;
        }
        break;

      // OPR 0, M Operation to be performed on the data at the top of the Stack
      case 2:
        switch (ir.m)
        {
          // RTN
          case 0:
            strcpy(opname, "RTN");
            sp = bp + 1;
            pc = pas[sp - 3];
            bp = pas[sp - 2];
            break;

          // NEG
          case 1:
            strcpy(opname, "NEG");
            if (bp == gp)
              pas[dp] = -1 * pas[dp];
            else
              pas[sp] = -1 * pas[sp];
            break;

          // ADD
          case 2:
            strcpy(opname, "ADD");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] + pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] + pas[sp - 1];
            }
            break;

          // SUB
          case 3:
            strcpy(opname, "SUB");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] - pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] - pas[sp - 1];
            }
            break;

          // MUL
          case 4:
            strcpy(opname, "MUL");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] * pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] * pas[sp - 1];
            }
            break;

          // DIV
          case 5:
            strcpy(opname, "DIV");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] / pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] / pas[sp - 1];
            }
            break;

          // ODD
          case 6:
            strcpy(opname, "ODD");
            if (bp == gp)
              pas[dp] = pas[dp] % 2;
            else
              pas[sp] = pas[sp] % 2;

            break;

          // MOD
          case 7:
            strcpy(opname, "MOD");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] % pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] % pas[sp - 1];
            }

            break;

          // EQL
          case 8:
            strcpy(opname, "EQL");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] == pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] == pas[sp - 1];
            }
            break;

          // NEQ
          case 9:
            strcpy(opname, "NEQ");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] != pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] != pas[sp - 1];
            }
            break;

          //LSS
          case 10:
            strcpy(opname, "LSS");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] < pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] < pas[sp - 1];
            }
            break;

          // LEQ
          case 11:
            strcpy(opname, "LEQ");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] <= pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] <= pas[sp - 1];
            }
            break;

          // GTR
          case 12:
            strcpy(opname, "GTR");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] > pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] > pas[sp - 1];
            }
            break;

          // GEQ
          case 13:
            strcpy(opname, "GEQ");
            if (bp == gp)
            {
              dp = dp - 1;
              pas[dp] = pas[dp] >= pas[dp + 1];
            }
            else
            {
              sp = sp + 1;
              pas[sp] = pas[sp] >= pas[sp - 1];
            }
            break;

        }
        break;

      // Lod L, M Load value to top of stack from the stack location at offset M
      // from L lexicographical levels down (load value into data segment)
      case 3:
        strcpy(opname, "LOD");
        if (bp == gp)
        {
          dp = dp + 1;
          pas[dp] = pas[gp + ir.m];
        } else {
          if (base(ir.l, bp, pas) == gp)
          {
            sp = sp - 1;
            pas[sp] = pas[gp + ir.m];
          }
          else
          {
            sp = sp - 1;
            pas[sp] = pas[base(ir.l, bp, pas) - ir.m];
          }
        }
        break;


      // STO L, M Store value at top of stack in the stack location at offset M from L lexicographical levels down (store value in data segment)
      case 4:
        strcpy(opname, "STO");
        if (bp == gp)
        {
          pas[gp + ir.m] = pas[dp];
          dp = dp - 1;
        }
        else {
           if (base(ir.l, bp, pas) == gp)
           {
             pas[gp + ir.m] = pas[sp];
             sp = sp + 1;
           }
           else
           {
             pas[base(ir.l, bp, pas) - ir.m] = pas[sp];
             sp = sp + 1;
           }
        }
        break;

      // CAL L, M Call procedure at code index M (generates new activation record and PC <- M)
      case 5:
        strcpy(opname, "CAL");
        pas[sp - 1] = base(ir.l, bp, pas);  // Functional Value (FV)
        pas[sp - 2] = bp;// static link (SL)
        pas[sp - 3] = pc; // Dynamic link (DL)
        pas[sp - 4] = 0; // Return Address (RA)
        bp = sp - 1;
        pc = ir.m;
        break;

      // INC 0, M Allocate M memory words (increment SP by M or increment dp)
      case 6:
        strcpy(opname, "INC");
        if (bp == gp)
          dp = dp + ir.m;
        else
          sp = sp - ir.m;
        break;

      // JMP 0, M Jump to instruction M(PC <- M)
      case 7:
        strcpy(opname, "JMP");
        pc = ir.m;
        break;

      // JPC 0, M Jump to instruction M if top stack or data element is 0
      case 8:
        strcpy(opname, "JPC");
        if (bp == gp)
        {
          if (pas[dp] == 0)
          {
            pc = ir.m;
          }
            dp = dp - 1;
        }
        else
        {
          if (pas[sp] == 0)
          {
            pc = ir.m;
          }
            sp = sp + 1;
        }
        break;

      // SYS 0, 1 Write the top stack element or data element to the screen
      // SYS 0, 2 Read in input from the user and store it on top of the stack (or in data section)
      // SYS 0, 3 End of program (set Halt flag to zero)
      case 9:
        strcpy(opname, "SYS");
        switch(ir.m)
        {
          // write the top stack element or data element to the screen
          case 1:
            if (bp == gp)
            {
              printf("Top of Stack Value: %d\n", pas[dp]);
              dp = dp - 1;
            }
            else
            {
              printf("Top of Stack Value: %d\n", pas[sp]);
              sp = sp + 1;
            }
            break;

          // Read input from user and store it on top of the stack
          case 2:
            printf("Please Enter an Integer: ");
            if (bp == gp)
            {
              dp = dp + 1;
              scanf("%d", &pas[dp]);
            }
            else
            {
              sp = sp - 1;
              scanf("%d", &pas[sp]);
            }
            break;

          // set halt flag to zero (End of program)
          case 3:
            flag = 0;
            break;
        }
        break;
    }

    // print current line
    if(printFlag)
      print_execution(line, opname, ir, pc, bp, sp, dp, pas, gp);
  }

  // free dynamically allocated memory
  free(pas);
}
