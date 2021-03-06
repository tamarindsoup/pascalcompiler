#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define TITLE(s) printf("%s\nName\tAddr\tType\n"	\
			"------------------------\n",s)

typedef struct tmp{
  struct tmp *next;
  Item item;
} Stack;

static Stack *stack = NULL;

char *scope[] = {
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
  for(ptr=stack; ptr!=NULL; ptr=ptr->next)
    iprint(&ptr->item);
  putchar('\n');
}

static Addr
addrlookup(Type type)
{
  Stack *ptr;
  for(ptr=stack;
      ptr!=NULL;
      ptr=ptr->next)
    if(ptr->item.type==type)
      return ptr->item.addr + 1;
  return 0;
}

int
insert(char *name, Addr address, Type type)
{
  Stack *ptr;
  if((ptr=(Stack*)malloc(sizeof(Stack)))==NULL)
    return -1;
  ptr->next = stack;
  if((ptr->item.name=(char*)malloc((strlen(name)+1)*sizeof(char)))==NULL)
    return -1;
  strcpy(ptr->item.name,name);
  ptr->item.type = type;
  ptr->item.addr = (type == PROC) ? address : addrlookup(type);
  stack = ptr;
  TITLE("INSERT");
  print();
  return 0;
}

Item
*lookup(char *name)
{
  Stack *ptr;
  TITLE("LOOKUP");
  for(ptr=stack;ptr!=NULL;ptr=ptr->next)
    if(!strcmp(ptr->item.name,name)){
      iprint(&ptr->item);
      putchar('\n');
      return &(ptr->item);
    }
  printf("%s\tFAILED\tFAILED\n\n",name);
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
  print();
}
