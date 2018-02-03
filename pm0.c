/**
Author: Ross Wagner

This code is an implementation of a virtual machine, Psedo machine 0 (PM0),
designed to run a limited instruction set,

To-do
-----
tokenize
toekns to instructions
put instructions code[]
start interpreting code[] exicuting the instructions


Problems
---------
unwanted characters in tokens causing prolems with str to int conversion


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pm0.h"

// constants for getLine
#define OK 0
#define NO_INPUT 1
#define TOO_LONG 2
#define MAX_LINE_LEN 16

// constants for the vm
#define CMD_LEN 3 // lenght of comands
#define NUM_OP 23 // the number of different instructions
#define NUM_PARAM 4 // number of peramiters in each instuction
#define NUMREG 16
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVLES 3
#define MAX_STACK_HEIGHT 2000


static unsigned reg[NUMREG];
unsigned stack[MAX_STACK_HEIGHT];
static instruction code[MAX_CODE_LENGTH];
static char ops[NUM_OP][CMD_LEN] = {
"" , "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio",
"neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss",
"Leq", "GTR", "geq"
};

unsigned bp; //  base pointer
instruction ir; // instruction register
unsigned pc; // program counter
unsigned sp; // stack pointer

// function declarations
int getLine( char*, size_t, FILE*);
void fetch();
FILE *fileStuff(char**);
void init(FILE*);
void tokenize(char ** /*(*params)[NUM_PARAM]*/, char []);


int main(int argc, char **argv){

  if(argc != 2){
    printf("Usage: ./pm0 <filename>.txt\n" );
    return -1;
  }

  FILE *fid = fileStuff(argv);
  if(fid == NULL){
    return -1;
  }

  init(fid);


  return 0;
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
  int last, ch, extra;

  // get line with buffer overrun protection
  fflush(stdout);

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

  // Otherwise remove newline and give string back to caller
  buff[strlen(buff) - 1] = '\0';
  return OK;

}


/*
loads the next instruction from code[pc] into ir;
*/
void fetch(){
  ir =  code[pc++];
}

/*
this function hadels the opening of the file

@parameter argv, comand line arguemnt. argv[1] should be the desiered file name

@return file, a pointer to the open file containing the instructions for the vm
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
*/
void init(FILE *fid){
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

  // read from input file. put in code[]
  // expected input: 4 numbers on a line each seperated by a space
  //<op code> <register> <lexical level> <peramiter. could be many things>
  instruction temp;
  char line[MAX_LINE_LEN];
  int index = 0;

  while(getLine(line, MAX_LINE_LEN, fid) == OK && index < MAX_CODE_LENGTH){
    // break into 4 ints

    char * params[NUM_PARAM];

    tokenize(params, line);

    instruction temp;
    //char *end;
    temp.op = (int) strtol(params[0], (char **)NULL, 10);
    printf("%s", ops[temp.op]);//debug



  }


}

/*
tokenizes string line based on space character

@parameter params, destination of parameters that have been tokenized
@parameter ilne, string to be tokenized
*/
void tokenize(char ** params /*[NUM_PARAM]*/, char line[]){

  char s[MAX_LINE_LEN];
  strcpy(s, line);
  char* token = strtok(s," ");
  int index = 0;
  //printf("HERE\n");//debug
  while(token && index < NUM_PARAM){
    strcpy (params[index], token);
    token = strtok(NULL," ");
  }

}
