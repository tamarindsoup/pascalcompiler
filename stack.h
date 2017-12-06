typedef int Addr;

typedef enum {
  GLOB,
  LOC,
  PROC
} Type;

typedef struct{
  char *name;
  Addr addr;
  Type type;
} Item;

int insert(char*, Addr, Type);
Item* lookup(char*);
void delete(void);
