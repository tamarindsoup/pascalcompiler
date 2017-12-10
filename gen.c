#include <stdio.h>
#include <stdlib.h>

#include "gen.h"

#define OUTPUT "result.code"

/* static char *op[] = { */
/*   [LIT] "lit", */
/*   [LOD] "lod", */
/*   [STO] "sto", */
/*   [MVX] "mvx", */
/*   [OPR] "opr", */
/*   [INT] "int", */
/*   [CAL] "cal", */
/*   [RTN] "rtn", */
/*   [JMP] "jmp", */
/*   [JPC] "jpc", */
/*   [GET] "get", */
/*   [PUT] "put", */
/*   [GETC] "getc", */
/*   [PUTC] "putc", */
/* }; */

typedef struct tmp{
  struct tmp *next;
  OPCODE op;
  int num;
} Queue;

static Queue *head = NULL, *tail = NULL;

static int linenum;
static FILE *fp = NULL;

static void lflush(void){
  
  Queue *ptr;
  
  while( head != NULL ){
    if(head->op.address == UNDEF)
      break;
    fwrite(&head->op, sizeof(OPCODE), 1, fp);
    ptr=head;
    head=head->next;
    free(ptr);
  }
  
}

int generate( int opcode, REG base, REG index, REG address){
  
  if(head == NULL && address != UNDEF){
    
    if(fp == NULL)
      if( (fp = fopen(OUTPUT, "w")) == NULL ){
	fprintf(stderr,"Cannot open file: %s", OUTPUT);
	exit(1);
      }
    
    fwrite(&opcode, sizeof(opcode), 1, fp);
    fwrite(&base, sizeof(base), 1, fp);
    fwrite(&index, sizeof(index), 1, fp);
    fwrite(&address, sizeof(address), 1, fp);
    
  }
  
  else{
    Queue *tmp;
    
    if( (tmp = (Queue*)malloc(sizeof(Queue))) == NULL  )
      exit(1);
    
    tmp->op.opcode = opcode;
    tmp->op.basereg = base;
    tmp->op.indexreg = index;
    tmp->op.address = address;
    tmp->num = linenum;
    tmp->next = NULL;
  
    if( head == NULL ){
      head = tail = tmp;
    }
    else{
      tail->next = tmp;
      tail = tmp;
    }
  }

  return linenum++;
  
}

void backpatch( int num, REG addr){

  Queue *ptr;
  
  for(ptr = head; ptr->num < num; ptr = ptr->next);

  if(ptr->num == num)
    ptr->op.address = addr;

  lflush();
  
}

void output(){
  fclose(fp);
}
    
  

