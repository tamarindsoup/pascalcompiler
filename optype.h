/*
	Virtual machine word definition 
		Sccs(@(#)optype.h	1.1  1/24/91)
*/

/*
    Register is equivalent to int
*/
typedef int REG;

/*
    One word is composed of instruction code, base register,
    index register, address field
*/
typedef struct {
    int	opcode;
    REG	basereg;
    REG	indexreg;
    REG address;
} OPCODE;
