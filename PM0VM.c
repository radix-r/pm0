/**
* This code is an implementation of a virtual machine, Psedo machine 0 (PM0),  designed to run a limited instruction set,
*
*/

#include <stdio.h>
#include <string.h>
#include "PM0VM.h"

// constants for getLine
#define OK 0
#define NO_INPUT 1
#define TOO_LONG 2
#define MAX_LEN 10

// constants for the vm
#define NUM_OP 23 // the number of different instructions
#define NUMREG 16
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVLES 3
#define MAX_STACK_HEIGHT 2000


static unsigned reg[NUMREG];
unsigned stack[MAX_STACK_HEIGHT];
static instruction code[MAX_CODE_LENGTH];
static char ops[NUM_OP][3] = {
"" , "LIT", "RTN", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO",
"NEG", "ADD", "SUB", "MUL", "DIV", "ODD", "MOD", "EQL", "NEQ", "LSS",
"LEQ", "GTR", "GEQ"
};

unsigned bp; //  base pointer
instruction ir; // instruction register
unsigned pc; // program counter
unsigned sp; // stack pointer



/*
loads the next instruction from code[pc] into ir;
*/
static void fetch(){
  ir =  code[pc++];
}

static void init(){
  // init registers
  bp = 1;
  //ir = ?;
  pc = 0;
  sp = 0;

  // initilize reg and stack to all 0
  int i,j;
  for(i = 0; i < NUMREG; i++){
    reg[i]=0;
  }

  for(j = 0; j < MAX_STACK_HEIGHT; j++){
    stack[j]=0;
  }

  // read from stdin. put in code[]
  // expected input: 4 numbers on a line each seperated by a space
  //<op code> <register> <lexical level> <peramiter. gould be many things>
  instruction temp;
  char* buffer;
  int index = 0;
  while(getLine(NULL, buffer, MAX_LEN)){
    printf("%s\n", buffer);
  }


}

/**
A method of getting input from std in

@paramiter prmpt, a character array the stores the prompt message
@paramiter buff, temporary location of the line read in by fgets
@paramiter sz, max number of characters that fgets will read per line

@return status, OK(0), NO_INPUT(1), and TOO_LONG(2)
*/
static int getLine(char *prmpt, char *buff, size_t sz){
  int ch, extra;

  // get line with buffer overrun protection
  if (prmpt != NULL){
    printf("%s", prmpt);
    fflush(stdout);
  }
  if(fgets(buff, sz, stdin) == NULL){
    return NO_INPUT;
  }

  // If it was too long, there'll be no newline. in that case we flush
  // to end of line so the excess doesnt affect the next call
  if (buff[strlen(buff)-1] != '\n'){
    extra = 1;
    while(((ch = getchar()) != '\n') && (ch!= EOF)){
      extra = 1;
    }
    return (extra == 1) ? TOO_LONG : OK;
  }

  // Otherwise remove newline and give string back to caller
  buff[strlen(buff) - 1] = '\0';
  return OK;

}

int main(int argc, char **argv){


  init();


  return 0;
}
