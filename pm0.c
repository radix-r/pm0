/**
Author: Ross Wagner

This code is an implementation of a virtual machine, Psedo machine 0 (PM0),
designed to run a limited instruction set.

structure of activation frame
<return value> <static link> <dynamic link> <return address> <>

To-do
-----
rework fetch exicute
print stack trace
"|" between activation frames
store output to print later
ptint output
readme file

Problems
---------
uses a lot of global variables
am I still using 16 registers

*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pm0.h"

// constants for getLine
#define OK 0
#define NO_INPUT 1
#define TOO_LONG 2
#define MAX_LINE_LEN 30

// constants for the vm
#define CMD_LEN 4 // lenght of comands
#define NUM_OP 23 // the number of different instructions
#define NUM_PARAM 4 // number of peramiters in each instruction
#define NUMREG 16
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVLES 3
#define MAX_NUM_LEN  11// number can have a most 11 digits, includs potental "-"
#define MAX_STACK_HEIGHT 2000

// global vars
instruction code[MAX_CODE_LENGTH];
int codeLen; // where the number of lines of code will be recorded
int codeLine = 0; // current line in code[] we are at
int hault = 0;
char **output;// string array that stores all output of the vm
char ops[NUM_OP][CMD_LEN] = {
"\0" , "LIT\0", "RTN\0", "LOD\0", "STO\0", "CAL\0", "INC\0", "JMP\0", "JPC\0", "SIO\0",
"NEQ\0", "ADD\0", "SUB\0", "MUL\0", "DIV\0", "ODD\0", "MOD\0", "EQL\0", "NEQ\0", "LSS\0",
"LEQ\0", "GTR\0", "GEQ\0"};
int reg[NUMREG];
int stack[MAX_STACK_HEIGHT];

// special registers
unsigned bp; //  base pointer
instruction ir; // instruction register
unsigned pc; // program counter
unsigned sp; // stack pointer



// function declarations
int base(int, int);
void execute();
int getLine( char*, size_t, FILE*);
int fetch();
FILE *fileStuff(char**);
void init(FILE*);
void printReg();
void printStack();
void printStackTrace();
void printState();
void printStrArray(char **, int);
char ** tokenize(const char *);


int main(int argc, char **argv){

  if(argc != 2){
    printf("Usage: ./pm0 <filename>\n" );
    return -1;
  }

  FILE *fid = fileStuff(argv);
  if(fid == NULL){
    return -1;
  }

  init(fid);

  printf("\n OP\tRg Lx Vl[ PC BP SP]\n");
  while(!hault){

      if(fetch()){
        execute();
        printStackTrace();
        printReg();
      }


  }

  return 0;
}

/*
Find base L levels down

@parameter l, unmber of activation records down
@parameter base, the base of the current activation record

@return index of the base of the frame l levles down from current AR
*/
int base(int l, int base){ // l stand for L in the instruction format

  int b1; //find base L levels down
  b1 = base;
  while (l > 0){
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}


/*
reads operation from ir and preforms apropriate computations
*/
void execute(){
  //determine opcode
  int op = ir.op;

  // exicute apropriate instructions based on op code
  switch(op){
    case 1: // lit
      reg[ir.r] = ir.m;
      break;
    case 2: // rtn
      sp = bp-1;
      bp = stack[sp+3]; // dynamic link
      pc = stack[sp+4]; // return address
      break;
    case 3: // lod
      reg[ir.r] = stack[base(ir.l, bp)+ir.m];
      break;
    case 4: //sto
      stack[base(ir.l, bp)+ir.m] = reg[ir.r];
      break;
    case 5: //cal, creates new activation record
      if(sp+4 > MAX_STACK_HEIGHT){
        printf("Max stack height exceeded. Haulting.\n");
        hault = 1;
        break;
      }
      stack[sp+1] = 0; // space to return value;
      stack[sp+2] = base(ir.l, bp); //static link
      stack[sp+3] = bp; // dynamic link
      stack[sp+4] = pc; // return address
      bp = sp+1;
      pc = ir.m;
      break;
    case 6: // inc
      if(sp+ir.m >= MAX_STACK_HEIGHT){
        printf("Max stack height exceeded. Haulting.\n");
        hault = 1;
        break;
      }
      sp = sp + ir.m;
      break;
    case 7: // jmp
      pc = ir.m;
      break;
    case 8: // jpc
      if(reg[ir.r] == 0){
        pc = ir.m;
      }
      break;
    case 9: // sio
      switch(ir.m){
        case 1: // print
          printf("%d\n", reg[ir.r]);
          break;
        case 2: ; // read input from stdin and put in designated register
          char *str;
          int status = getLine(str, MAX_NUM_LEN, stdin);
          if (status == OK){
            long num = strtol(str, (char **)NULL, 10);
            if(num > INT_MIN && num < INT_MAX){
              reg[ir.r] = (int) num;
            }
          }
          else if(status == TOO_LONG){
            printf("Entered vaule is too long. Haulting\n");
            hault = 1;
          }
          break;
        case 3: //hault
          hault = 1;
          break;
      }
      break;
    case 10: // neg
      reg[ir.r] = -reg[ir.r];
      break;
    case 11: // add
      reg[ir.r] = reg[ir.l] + reg[ir.m];
      break;
    case 12: // sub
      reg[ir.r] = reg[ir.l] - reg[ir.m];
      break;
    case 13: // mult
      reg[ir.r] = reg[ir.l] * reg[ir.m];
      break;
    case 14: // div
      reg[ir.r] = reg[ir.l] / reg[ir.m];
      break;
    case 15: // odd
      reg[ir.r] = reg[ir.l] % 2;
      break;
    case 16: // mod
      reg[ir.r] = reg[ir.l] % reg[ir.m];
      break;
    case 17: // eql
      reg[ir.r] = reg[ir.l] == reg[ir.m];
      break;
    case 18: // neq
      reg[ir.r] = reg[ir.l] != reg[ir.m];
      break;
    case 19: // lss
      reg[ir.r] = reg[ir.l] < reg[ir.m];
      break;
    case 20: // leq
      reg[ir.r] = reg[ir.l] <= reg[ir.m];
      break;
    case 21: // gtr
      reg[ir.r] = reg[ir.l] > reg[ir.m];
      break;
    case 22: // geq
      reg[ir.r] = reg[ir.l] >= reg[ir.m];
      break;
    default:
      printf("Unrecognized op code. Haulting.\n");
      hault = 1;

  }
}

/**
A method of getting input from file. lifted form
https://stackoverflow.com/questions/4023895/how-to-read-string-entered-by-user-in-c

@parameter buff, temporary location of the line read in by fgets
@parameter sz, max number of characters that fgets will read per line
@parameter fid, a file pointer to where we are reading line from

@return status, OK(0), NO_INPUT(1), and TOO_LONG(2)
*/
int getLine(char *buff, size_t sz, FILE* fid){
  int last, ch, extra, i=1;

  // get line with buffer overrun protection
  //fflush(stdout);

  if(fgets(buff, sz, fid) == NULL){
    return NO_INPUT;
  }

  // If it was too long, there'll be no newline. in that case we flush
  // to end of line so the excess doesnt affect the next call
  if ((last = buff[strlen(buff)-1] != '\n') && (last != EOF)){
    extra = 0;
    while(((ch = fgetc(fid)) != '\n') && (ch!= EOF)){
      extra = 1;
    }
    return (extra == 1) ? TOO_LONG : OK;
  }

  // Otherwise remove non digit trailing characters and give string back to caller
  while(last = buff[strlen(buff)-i] < '0' || last > '9' ){
      buff[strlen(buff) - i] = '\0';
  }

  return OK;

}


/*
loads the next instruction from code[pc] into ir;

@return 1 on sucess, 0 on failure
*/
int fetch(){
  if(pc > codeLen){
    printf("PC exceeded code lenght. Haulting.\n");
    hault = 1;
    return 0;
  }

  codeLine = pc;
  ir = code[pc++];// load next instruction

  return 1;

}

/*
this function hadels the opening of the file

@parameter argv, comand line arguemnt. argv[1] should be the desiered file name

@return file, a pointer to the open file containing the instructions for the vm.
returns null on failure
*/
FILE* fileStuff(char **argv){

  char *fileName = argv[1];
  FILE* file = fopen(fileName, "r");

  if(file == NULL){
    printf("Failed to open %s.\n", fileName);
    return NULL;
  }

  return file;

}

/**
initilizes the vm

@parameter fid, pointer to a file where input will be red from
*/
void init(FILE *fid){
  // dummy instruction to set ir to 0
  instruction dummy;
  dummy.op = 0;
  dummy.r = 0;
  dummy.l = 0;
  dummy.m = 0;

  // init registers
  bp = 1;
  ir = dummy;
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

  // read from input file. put in code[]
  // expected input: 4 numbers on a line each seperated by a space
  //<op code> <register> <lexical level> <peramiter. could be many things>
  instruction temp;
  char line[MAX_LINE_LEN];
  int index = 0;

  while(getLine(line, MAX_LINE_LEN, fid) == OK && index < MAX_CODE_LENGTH){
    // break into 4 ints

    char ** params;//[NUM_PARAM];

    params = tokenize(line);

    instruction temp;
    //char *end;
    temp.op = (int) strtol(params[0], (char **)NULL, 10);
    temp.r = (int) strtol(params[1], (char **)NULL, 10);
    temp.l = (int) strtol(params[2], (char **)NULL, 10);
    temp.m = (int) strtol(params[3], (char **)NULL, 10);

    char *op = ops[temp.op];
    // print the translated instruction
    // printf("%d %s %d %d %d\n",index, op,temp.r,temp.l,temp.m);
    code[index++] = temp;

    // dealocate memory used with params
    char ** it;
    for(it=params; it && *it; ++it){
      free(*it);
    }
    free(params);
  }
  // record length of the code
  codeLen = index;

  // print a newline
  //printf("\n");
}

/*
prints instruction in ir
*/
/*void printInstruction(){
  printf("%s\t%d\t%d\t%d\t", ops[ir.op], ir.r, ir.l, ir.m);
}*/

/*
prints the contents of the registers
*/
void printReg(){
  printf("\tRegisters:[ ");
  int i;
  for(i=0; i < NUMREG; i++){
    printf("%3d ", reg[i]);
  }
  printf("]\n");
}

/*
prints the stack where activation frames are seperated by '|' characters
*/
void printStack(){
  // count the activation frames
  int pos = bp;
  int frames = 0;
  int numStrings,i,j,stackIndex = 0;
  int curFrame = 0;
  char **st;


  while(pos > 0){
    pos = stack[pos+3];
    frames++;
  }

/*
  // lower numbers first
  int basePos[frames];
  // save position of bases of frames
  for(j = 0; j < frames; j++){
    basePos[j] = base(frames - j, bp);
  }
*/


  numStrings = sp+frames;// number of elements in the stack + num frames
  st = calloc(1, numStrings*sizeof(*st));// dont forget to free this


  for(i = 0; i < numStrings; i++){

    if(stackIndex == bp){// only gets last frame
      st[i++] = strdup("|");
      //st[i++] = "|";
      //curFrame++;
    }

    char *temp =calloc(1, MAX_NUM_LEN); //size of char = 1, dont forget to free,
    sprintf(temp, "%d", stack[stackIndex++]);// this is a mess
    st[i]= strdup(temp);
    //st[i] = strdup(temp);
    // try with strcpy
    //strcpy(st[i], temp);
/*
    if(stackIndex == basePos[curFrame]){
      st[++i] = "|";
      curFrame++;
    }
*/
  }


  printStrArray(st, numStrings);
  printf("\n");
  // free alocated memory
  /*char ** it;
  for(it=st; it && *it; ++it){
    free(*it);
  }*/
  free(st);

}

/*
prints the current instruction, and the contents of pc, bp, and sp, followed by
the contents of the stack up to sp
*/
void printStackTrace(){
  printf("%-4s%3d%3d%3d[%3d%3d%3d] ", ops[ir.op], ir.r, ir.l, ir.m,pc,bp,sp);
  printStack();
}

/*
prints the content of an array of strings
*/
void printStrArray(char **str, int numStrings){
  int i;
  for (i = 0; i < numStrings; i++)
  {
    printf("%3s ",str[i]);
    fflush(stdout);
  }
}

/*
tokenizes string line based on space character. lifted form
https://stackoverflow.com/questions/8106765/using-strtok-in-c

@parameter input, string to be tokenized

@return result, a string array containing all the tokens
*/
char** tokenize(const char* input){

  char * str = strdup(input) ;
  int count = 0;
  int capacity = NUM_PARAM;
  char ** result = calloc(1,capacity*sizeof(*result));

  char* token = strtok(str," ");

  while(1){
    if (count >= NUM_PARAM){
      result = realloc(result, (capacity*=2)*sizeof(*result));
    }

    result[count++] = token? strdup(token) : token;

    if (!token) break;

    token = strtok(NULL," ");
  }

  free(str);
  return result;
}
