#include <stdio.h>
#include <stdlib.h>

#include "gen.h"

#define OUTPUT "result.code"

typedef struct tmp{
  struct tmp *next;
  OPCODE op;
} Queue;

typedef struct tmp2{
  struct tmp2 *next;
  Queue *undef; 
  REG linenum;
} Stack;

static Queue *head = NULL, *tail = NULL;

static Stack *stack = NULL;

static int linenum;

static FILE *fp = NULL;

static void lflush(void){
  
  Queue *ptr;

  if( !fp )
    if( !(fp = fopen(OUTPUT, "w")) ){
      fprintf(stderr, "Cannot open file: %s", OUTPUT);
      exit(1);
    }
  
  while( head ){
    if(head->op.address == UNDEF)
      break;
    fwrite(&head->op, sizeof(OPCODE), 1, fp);
    ptr=head;
    head=head->next;
    free(ptr);
  }
  
}

int generate( int opcode, REG base, REG index, REG address){
  
  if( !head && address != UNDEF){
    
    if( !fp )
      if( !(fp = fopen(OUTPUT, "w")) ){
	fprintf(stderr,"Cannot open file: %s", OUTPUT);
	exit(1);
      }
    
    fwrite(&opcode, sizeof(opcode), 1, fp);
    fwrite(&base, sizeof(base), 1, fp);
    fwrite(&index, sizeof(index), 1, fp);
    fwrite(&address, sizeof(address), 1, fp);
    
  }
  
  else{
    Queue *ptr;
    
    if( !(ptr = (Queue*)malloc(sizeof(Queue))) )
      exit(1);
    
    ptr->op.opcode = opcode;
    ptr->op.basereg = base;
    ptr->op.indexreg = index;
    ptr->op.address = address;
    ptr->next = NULL;
  
    if( !head ){
      head = tail = ptr;
    }
    else{
      tail->next = ptr;
      tail = ptr;
    }

    if( address == UNDEF ){
      Stack *ptr2;

      if( !(ptr2 = (Stack*)malloc(sizeof(Stack))) )
	exit(1);

      ptr2->linenum = linenum;
      ptr2->undef = ptr;
      ptr2->next = stack;
      stack = ptr2;
    
    }
  }

  return linenum++;
  
}

void backpatch( int num, REG addr){
  
  Stack *ptr, *prev = NULL;
  
  for(ptr = stack; ptr; prev = ptr, ptr = ptr->next){

    if(ptr->linenum == num){
      ptr->undef->op.address = addr;

      //delete
      if(prev){
	prev->next = ptr->next;
	free(ptr);
      }
      //pop
      else{
	stack = stack->next;
	free(ptr);
      }
      break;
    }
    
  }
    
  lflush();
  
}

void output(){
  lflush();
  fclose(fp);
}
    
  

