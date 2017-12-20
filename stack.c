#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

/* #define DEBUG */

#ifdef DEBUG
#define TITLE(s) printf("%s\nName\tAddr\tType\n"	\
			"------------------------\n",s)
#define PRINT print()
#else
#define TITLE(s)
#define PRINT
#endif

typedef struct tmp{
  struct tmp *next;
  Item item;
} Stack;

static Stack *stack = NULL;

#ifdef DEBUG
static char *scope[] = {
    [GLOB] "GLOB",
    [LOC] "LOC",
    [PROC] "PROC",
};

static void
iprint(Item *item){
  printf("%s\t%d\t%s\n",
	 item->name,
	 item->addr,
	 scope[item->type]);
}

static void
print(void)
{
  Stack *ptr;
  for(ptr=stack; ptr; ptr=ptr->next)
    iprint(&ptr->item);
  putchar('\n');
}
#endif

static Addr
addrlookup(Type type)
{
  Stack *ptr;
  for(ptr=stack;
      ptr;
      ptr=ptr->next)
    if(ptr->item.type==type)
      return ptr->item.addr + 1;
  return 0;
}

int
insert(char *name, Addr address, Type type)
{
  Stack *ptr;
  if( !(ptr=(Stack*)malloc(sizeof(Stack))) )
    return -1;
  ptr->next = stack;
  if( !(ptr->item.name=(char*)malloc((strlen(name)+1)*sizeof(char))) )
    return -1;
  strcpy(ptr->item.name,name);
  ptr->item.type = type;
  ptr->item.addr = (type == PROC) ? address : addrlookup(type);
  stack = ptr;
  TITLE("INSERT");
  PRINT;

  return 0;
}

Item
*lookup(char *name)
{
  Stack *ptr;
  TITLE("LOOKUP");
  for(ptr=stack; ptr; ptr=ptr->next)
    if(!strcmp(ptr->item.name,name)){
#ifdef DEBUG
      iprint(&ptr->item);
      putchar('\n');
#endif
      return &(ptr->item);
    }
#ifdef DEBUG
  printf("%s\tFAILED\tFAILED\n\n",name);
#endif
  return NULL;
}

void
delete(void){
  Stack *ptr = stack;
  while(ptr->item.type==LOC){
    stack = stack->next;
    free(ptr->item.name);
    free(ptr);
    ptr=stack;
  }
  TITLE("DELETE");
  PRINT;
}
