/**
Author: Ross Wagner

This code is an implementation of a virtual machine, Psedo machine 0 (PM0),
designed to run a limited instruction set,

To-do
-----
put instructions code[]
start interpreting code[] exicuting the instructions

readme file

Problems
---------


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
#define CMD_LEN 4 // lenght of comands
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
"\0" , "lit\0", "rtn\0", "lod\0", "sto\0", "cal\0", "inc\0", "jmp\0", "jpc\0", "sio\0",
"neg\0", "add\0", "sub\0", "mul\0", "div\0", "odd\0", "mod\0", "eql\0", "neq\0", "lss\0",
"leq\0", "gtr\0", "geq\0"
};

// special registers
unsigned bp; //  base pointer
instruction ir; // instruction register
unsigned pc; // program counter
unsigned sp; // stack pointer

// flags
int hault = 0;

// function declarations
//execute()
int getLine( char*, size_t, FILE*);
void fetch();
FILE *fileStuff(char**);
void init(FILE*);
char ** tokenize(const char *);


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

  while(!hault){
    fetch();
    //execute();
  }

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

@parameter fid, pointer to a file where input will be red from
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

    char ** params;//[NUM_PARAM];

    params = tokenize(line);

    instruction temp;
    //char *end;
    temp.op = (int) strtol(params[0], (char **)NULL, 10);
    temp.r = (int) strtol(params[1], (char **)NULL, 10);
    temp.l = (int) strtol(params[2], (char **)NULL, 10);
    temp.m = (int) strtol(params[3], (char **)NULL, 10);

    char *op = ops[temp.op];
    printf("%d %s %d %d %d\n",index, op,temp.r,temp.l,temp.m);
    code[index++] = temp;

    // dealocate memory used with params
    char ** it;
    for(it=params; it && *it; ++it){
      free(*it);
    }
    free(params);
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

  /*

  while(token && (index < NUM_PARAM)){
    strcpy (params[index++], token);
    token = strtok(NULL," ");
  }*/

}
