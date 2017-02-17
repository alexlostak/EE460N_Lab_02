/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the first partner 
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the first partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***********************/
/* Our instructions */
/***********************/
enum {
    BR, ADD, LDB, STB, JSR, AND, LDW, STW, RTI, XOR, NOP, NOP1, JMP, SHF, LEA, TRAP
};

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF) {
        printf("%d\n", word);
    program_base = word >> 1;
        printf("program_base: %d\n", program_base);
}
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
 /*     printf("program base is %x and ii is %d and word is %x\n", program_base, ii, word);
     */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
   /*   printf("memory[%x][0] = %x\n", program_base + ii, MEMORY[program_base + ii][0] );
      printf("memory[%x][1] = %x\n", program_base + ii, MEMORY[program_base + ii][1] );
    */
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

/***************************************************************/
/* 
 Helper Functions
 */

/***************************************************************/





uint32_t sext(int bitlength, uint32_t num) {
    uint32_t sextNum;
    uint32_t sign;
    uint16_t ffff;
    sextNum = 0;
    sign = num & (1 << (bitlength - 1));
    ffff = 0xFFFF;
    if (sign) {
        ffff = ffff << (bitlength - 1);
        sextNum = ffff | num;
    } else {
        sextNum = num;
    }
    return sextNum;
}

void str_mem(uint32_t strValue, uint32_t addr) {
    uint32_t leastSigValue;
    uint32_t mostSigValue;
    leastSigValue = 0;
    mostSigValue = 0;
    leastSigValue = strValue & 0x00FF;
    mostSigValue = (strValue >> 8) & 0x00FF;
    MEMORY[addr/2][0] = leastSigValue;
    MEMORY[addr/2][1] = mostSigValue;
    return;
}

uint32_t ld_mem(uint32_t addr) {
    uint32_t leastSigValue;
    uint32_t mostSigValue;
    uint32_t returnValue;
    leastSigValue = MEMORY[addr/2][0];
    mostSigValue = MEMORY[addr/2][1];
    returnValue = 0;
    returnValue = (mostSigValue << 8) | leastSigValue;
    return returnValue;
}


uint32_t create_instr(int pc) {
    uint32_t instr;
    uint32_t leastSigInstr;
    uint32_t mostSigInstr;
    leastSigInstr = MEMORY[pc/2] [0];
    mostSigInstr = MEMORY[pc/2] [1];
    instr = mostSigInstr << 8;
    instr = instr | leastSigInstr;
    return instr;
}

uint32_t get_dr1(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x0E00;
    value = instr & regAnd;
    value = value >> 9;
    return value;
}

uint32_t get_sr1(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x01C0;
    value = instr & regAnd;
    value = value >> 6;
    return value;
}

uint32_t get_sr2(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x007;
    value = instr & regAnd;
    return value;
}

uint32_t get_imm5(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x001F;
    value = instr & regAnd;
    return value;
}

uint32_t get_baseR(uint32_t instr) {
    uint32_t value = get_sr1(instr);
    return value;
}

uint32_t get_offset6(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x003F;
    value = instr & regAnd;
    return value;
}

uint32_t get_boffset6(uint32_t instr) {
    uint32_t value = get_offset6(instr);
    return value;
}

uint32_t get_pcoffset9(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x01FF;
    value = instr & regAnd;
    return value;
}

uint32_t get_trapvector8(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x00FF;
    value = instr & regAnd;
    return value;
}

uint32_t get_amount4(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x000F;
    value = instr & regAnd;
    return value;
}


/***************************************************************/
/*
 Helper Test Functions
 */

/***************************************************************/

void mem_test() {
    int ld;
    int strld;
    ld = ld_mem(0x3000);
    str_mem(12, 0x4000);
    strld = ld_mem(0x4000);
    printf("ld: %x\n strld: %d\n", ld, strld);
    return;
}

void create_instr_test(int pc) {
    uint32_t testInstr = create_instr(pc);
    printf("%x\n", testInstr);
    return;
}

void get_dr1_test(uint32_t instr) {
    uint32_t testVal;
    testVal = get_dr1(instr);
    printf("%x\n", instr);
    printf("%d\n", testVal);
    return;
}

void get_sr1_test(uint32_t instr) {
    uint32_t testVal;
    testVal = get_sr1(instr);
    printf("%x\n", instr);
    printf("%d\n", testVal);
    return;
}

void get_sr2_test(uint32_t instr) {
    uint32_t testVal;
    testVal = get_sr2(instr);
    printf("%x\n", instr);
    printf("%d\n", testVal);
    return;
}

void get_imm5_test(uint32_t instr) {
    uint32_t testVal;
    testVal = get_imm5(instr);
    printf("%x\n", instr);
    printf("%d\n", testVal);
    return;
}

/***************************************************************/
/*
 Opcode Functions
 */

/***************************************************************/



void add(uint32_t instr){
    //determine dr
        //check bits 11 9
    //determine sr1
        //check bits 8 6
    //determine imm5 or sr2
        //check
    
}

void and(){
    
}

void br(){
    
}

void jmp(){
    
}

void jsr(){
    
}

void ldb(){
    
}

void ldw(){
    
}

void lea(){
    
}

void rti(){
    
}

void shf(){
    
}

void stb(){
    
}

void stw(){
    
}

void trap(){
    NEXT_LATCHES.PC = 0;
}

void xor(){
    
}

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */
    int pc = CURRENT_LATCHES.PC;
    uint16_t opCode = MEMORY[(pc)/2][1] >> 4;
    uint32_t instr = create_instr(pc);
    get_imm5_test(instr);
    switch(opCode) {
        case ADD :
            add(instr);
        case AND :
            and();
        case BR :
            br();
        case JMP :
            jmp();
        case JSR :
            jsr();
        case LDB :
            ldb();
        case LDW :
            ldw();
        case LEA :
            lea();
        case RTI :
            rti();
        case SHF :
            shf();
        case STB :
            stb();
        case STW :
            stw();
        case TRAP :
            trap();
        case XOR :
            xor();
    }
}
