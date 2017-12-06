#include <stdio.h>
#include <stdlib.h>

#include "gen.h"

#define OUTPUT "result.code"

static char *op[] = {
  [LIT] "lit",
  [LOD] "lod",
  [STO] "sto",
  [MVX] "mvx",
  [OPR] "opr",
  [INT] "int",
  [CAL] "cal",
  [RTN] "rtn",
  [JMP] "jmp",
  [JPC] "jpc",
  [GET] "get",
  [PUT] "put",
  [GETC] "getc",
  [PUTC] "putc",
};

typedef struct tmp{
  struct tmp *next;
  OPCODE op;
} Queue;

static Queue *head = NULL, *tail = NULL;

void generate( int opcode, REG base, REG index, REG address){
  
  Queue *tmp;
  if( (tmp = (Queue*)malloc(sizeof(Queue))) == NULL  )
       exit(1);
  
  tmp->op.opcode = opcode;
  tmp->op.basereg = base;
  tmp->op.indexreg = index;
  tmp->op.address = address;
  tmp->next = NULL;
  
  if( head == NULL ){
    head = tail = tmp;
  }
  else{
    tail->next = tmp;
    tail = tmp;
  }
  
}

void backpatch( int linenum, REG addr){
  
  Queue *ptr;
  
  for(ptr = head; linenum; linenum--)
    ptr = ptr->next;
  
  ptr->op.address = addr;
  
}

void output(){
  
  Queue *ptr;
  FILE *fp;
  
  if( (fp = fopen(OUTPUT, "w")) == NULL ){
    fprintf(stderr,"Cannot open file: %s", OUTPUT);
    exit(1);
  }
  
  for( ptr = head; ptr; ptr = ptr->next )
    fwrite(&ptr->op, sizeof(OPCODE), 1, fp);

  fclose(fp);

}
    
  

