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

static int linenum, blockhead;
static int depth;
static FILE *fp = NULL;

static void lflush(void){
  printf("lflush\n");
  Queue *ptr;
  
  while(head != NULL){
    fwrite(&head->op, sizeof(OPCODE), 1, fp);
    ptr=head;
    head=head->next;
    free(ptr);
  }
  blockhead = 0;
}

void generate( int opcode, REG base, REG index, REG address){
  printf("generate %s\t%d\t%d\t%d\t%d\n",op[opcode],base,index,address,depth);
  if(address == UNDEF){
    if(!blockhead)
      blockhead = linenum;
    depth++;
  }
  
  if(depth){
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
  else{
    
    if(fp == NULL)
      if( (fp = fopen(OUTPUT, "w")) == NULL ){
	fprintf(stderr,"Cannot open file: %s", OUTPUT);
	exit(1);
      }
    
    if(head != NULL)
      lflush();
    
    fwrite(&opcode, sizeof(opcode), 1, fp);
    fwrite(&base, sizeof(base), 1, fp);
    fwrite(&index, sizeof(index), 1, fp);
    fwrite(&address, sizeof(address), 1, fp);
    
  }

  linenum++;
  
}

void backpatch( int num, REG addr){
  printf("backpatch\t%d\t%d\t%d\n",num,addr,blockhead);
  Queue *ptr;
  
  for(ptr = head, num-=blockhead; num; num--)
    ptr = ptr->next;
  
  ptr->op.address = addr;
  
}

void output(){
  printf("output\n");
  lflush();
  fclose(fp);
}
    
  

