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

#define BR 0
#define ADD 1
#define LDB 2
#define STB 3
#define JSR 4
#define AND 5
#define LDW 6
#define STW 7
#define RTI 8
#define XOR 9
#define JMP 12
#define SHF 13
#define LEA 14
#define TRAP 15

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
    uint32_t ffff;
    sextNum = 0;
    sign = num & (1 << (bitlength - 1));
    ffff = 0xFFFF;
    if (sign) {
        ffff = ffff << (bitlength - 1);
        sextNum = ffff | num;
    } else {
        sextNum = num;
    }
    sextNum = sextNum & 0xFFFF;
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

int get_a(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x0020;
    value = instr & regAnd;
    value = value >> 5;
    return value;
}

void set_new_cc(uint32_t newValue) {
    uint32_t mostSigBitAnd = 0x8000;
    uint32_t result;
    if (newValue == 0) {
        CURRENT_LATCHES.N = 0;
        CURRENT_LATCHES.Z = 1;
        CURRENT_LATCHES.P = 0;
        return;
    }
    result = newValue & mostSigBitAnd;
    if (result) {
        CURRENT_LATCHES.N = 1;
        CURRENT_LATCHES.Z = 0;
        CURRENT_LATCHES.P = 0;
    } else {
        CURRENT_LATCHES.N = 0;
        CURRENT_LATCHES.Z = 0;
        CURRENT_LATCHES.P = 1;
    }
    return;
}

void update_next_latches(void) {
    int i;
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC;
    for (i = 0; i < 8; i++) {
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    return;
}

int check_n_bit(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x0800;
    value = instr & regAnd;
    if (value) {
        return 1;
    }
    return 0;
}

int check_z_bit(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x0400;
    value = instr & regAnd;
    if (value) {
        return 1;
    }
    return 0;
}

int check_p_bit(uint32_t instr) {
    uint32_t value;
    uint32_t regAnd = 0x0200;
    value = instr & regAnd;
    if (value) {
        return 1;
    }
    return 0;
}

uint32_t create_word(uint32_t memAddress) {
    uint32_t word;
    uint32_t leastSigByte;
    uint32_t mostSigByte;
    leastSigByte = MEMORY[memAddress/2] [0];
    mostSigByte = MEMORY[memAddress/2] [1];
    word = mostSigByte << 8;
    word = word | leastSigByte;
    return word;
}

uint32_t get_least_sig_byte(uint32_t word) {
    uint32_t least_sig_byte;
    uint32_t wordAnd = 0x00FF;
    least_sig_byte = word & wordAnd;
    return least_sig_byte;
}

uint32_t get_most_sig_byte(uint32_t word) {
    uint32_t most_sig_byte;
    uint32_t wordAnd = 0xFF00;
    most_sig_byte = word & wordAnd;
    return most_sig_byte;
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

void get_a_test(uint32_t instr) {
    int testVal;
    testVal = get_a(instr);
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
    uint32_t dr;
    uint32_t drValue;
    uint32_t sr1;
    uint32_t sr1Value;
    uint32_t operand2;
    uint32_t operand2Value;
    int a;
    dr = get_dr1(instr);
    sr1 = get_sr1(instr);
    sr1Value = CURRENT_LATCHES.REGS[sr1];
    a = get_a(instr);
    if (a) {
        operand2 = get_imm5(instr);
        operand2Value = operand2;
        operand2Value = sext(5, operand2Value);
    } else {
        operand2 = get_sr2(instr);
        operand2Value = CURRENT_LATCHES.REGS[operand2];
    }
    drValue = sr1Value + operand2Value;
    drValue = drValue & 0xFFFF;
    printf("drValue(%d) = sr1Value(%d) + operand2Value(%d)\n", drValue, sr1Value, operand2Value);
    CURRENT_LATCHES.REGS[dr] = drValue;
    set_new_cc(drValue);
    return;
}

void and(uint32_t instr){
    uint32_t isImmediate = get_a(instr);
    uint32_t sr1;
    uint32_t dr;
    uint32_t drValue;
    uint32_t sr1Value;
    uint32_t operand2;
    sr1 = get_sr1(instr);
    sr1Value = CURRENT_LATCHES.REGS[sr1];
    dr = get_dr1(instr);
    if (isImmediate) {
        operand2 = get_imm5(instr);
        operand2 = sext(5, operand2);
    } else {
        operand2 = get_sr2(instr);
        operand2 = CURRENT_LATCHES.REGS[operand2];
    }
    drValue = sr1Value & operand2;
    drValue = drValue & 0xFFFF;
    CURRENT_LATCHES.REGS[dr] = drValue;
    set_new_cc(drValue);
}

void br(uint32_t instr){
    //check nzp bits
    int instrN = check_n_bit(instr);
    int instrZ = check_z_bit(instr);
    int instrP = check_p_bit(instr);
    uint32_t pcoffset9 = sext(9, get_pcoffset9(instr));
    int latchesN = CURRENT_LATCHES.N;
    int latchesZ = CURRENT_LATCHES.Z;
    int latchesP = CURRENT_LATCHES.P;
    if (instrN && latchesN) {
        CURRENT_LATCHES.PC += pcoffset9;
    } else if (instrZ && latchesZ) {
        CURRENT_LATCHES.PC += pcoffset9;
    } else if (instrP && latchesP) {
        CURRENT_LATCHES.PC += pcoffset9;
    }
    return;
}

void jmp(uint32_t instr){
    uint32_t baseR = get_baseR(instr);
    uint32_t newPC = CURRENT_LATCHES.REGS[baseR];
    CURRENT_LATCHES.PC = newPC;
    return;
}

void jsr(uint32_t instr){
    uint32_t useRegister = (instr >> 11) & 0x1;
    uint32_t baseR;
    uint32_t baseRvalue;
    uint32_t offset;
    CURRENT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
    if (useRegister) {
        baseR = get_baseR(instr);
        baseRvalue = CURRENT_LATCHES.REGS[baseR];
        CURRENT_LATCHES.PC = baseRvalue;
        return;
    } else {
        offset = instr & 0x7FF;
        offset = sext(11, offset);
        offset *= 2;
        CURRENT_LATCHES.PC += offset;
        return;
    }
}

void ldb(uint32_t instr){
    uint32_t dr = get_dr1(instr);
    uint32_t baseR = get_baseR(instr);
    uint32_t baseRValue;
    uint32_t boffset6 = get_boffset6(instr);
    uint32_t memAddress;
    int newDrValue;
    int byteSelection;
    baseRValue = CURRENT_LATCHES.REGS[baseR];
    memAddress = baseRValue + sext(6, boffset6);
    byteSelection = memAddress % 2;
    newDrValue = MEMORY[memAddress / 2] [byteSelection];
    CURRENT_LATCHES.REGS[dr] = newDrValue;
    set_new_cc(newDrValue);
    return;
}

void ldw(uint32_t instr){
    uint32_t dr = get_dr1(instr);
    uint32_t baseR = get_baseR(instr);
    uint32_t baseRValue;
    uint32_t boffset6 = get_boffset6(instr);
    uint32_t memAddress;
    int newDrValue;
    baseRValue = CURRENT_LATCHES.REGS[baseR];
    boffset6 = sext(6, boffset6);
    boffset6 = boffset6 << 1;
    memAddress = baseRValue + boffset6;
    newDrValue = create_word(memAddress);
    CURRENT_LATCHES.REGS[dr] = newDrValue;
    set_new_cc(newDrValue);
    return;
}

void lea(uint32_t instr){
    uint32_t offset;
    uint32_t dr;
    dr = get_dr1(instr);
    offset = get_pcoffset9(instr);
    offset = sext(9, offset);
    CURRENT_LATCHES.REGS[dr] = CURRENT_LATCHES.PC + (offset*2);
}

void shf(uint32_t instr){
    uint32_t dr = get_dr1(instr);
    uint32_t sr = get_sr1(instr);
    uint32_t amount4 = instr & 0xF;
    uint32_t typeOfShift = (instr >> 4) & 0x3;
    uint32_t drValue;
    uint32_t srValue;
    srValue = CURRENT_LATCHES.REGS[sr];
    if (typeOfShift == 0){              //LSHF
        drValue = srValue << amount4;
        CURRENT_LATCHES.REGS[dr] = drValue;
    } else if (typeOfShift == 1) {      //RSHFL
        drValue = srValue >> amount4;
        CURRENT_LATCHES.REGS[dr] = drValue;
    } else if (typeOfShift == 3) {      //RSHFA
        drValue = srValue >> amount4;
        drValue = sext(16 - amount4, drValue);
        CURRENT_LATCHES.REGS[dr] = drValue;
    }
}

void not(uint32_t instr) {
    uint32_t dr = get_dr1(instr);
    uint32_t sr1 = get_sr1(instr);
    uint32_t sr1Value = CURRENT_LATCHES.REGS[sr1];
    CURRENT_LATCHES.REGS[dr] = ~sr1Value;
    return;
}

void ret() {
    CURRENT_LATCHES.PC = CURRENT_LATCHES.REGS[7];
    return;
}

void stb(uint32_t instr){
    uint32_t sr = get_dr1(instr);
    uint32_t srValue;
    uint32_t leastSigByteSrValue;
    uint32_t baseR = get_sr1(instr);
    uint32_t baseRValue;
    uint32_t boffset6 = get_boffset6(instr);
    uint32_t memAddress;
    boffset6 = sext(6, boffset6);
    srValue = CURRENT_LATCHES.REGS[sr];
    baseRValue = CURRENT_LATCHES.REGS[baseR];
    memAddress = baseRValue + boffset6;
    leastSigByteSrValue = get_least_sig_byte(srValue);
    MEMORY[memAddress / 2] [0] = leastSigByteSrValue;
    return;
}

void stw(uint32_t instr){
    uint32_t sr = get_dr1(instr);
    uint32_t srValue;
    uint32_t leastSigByteSrValue;
    uint32_t mostSigByteSrValue;
    uint32_t baseR = get_baseR(instr);
    uint32_t baseRValue;
    uint32_t boffset6 = get_offset6(instr);
    uint32_t memAddress;
    boffset6 = sext(6, boffset6);
    boffset6 = boffset6 << 1;
    srValue = CURRENT_LATCHES.REGS[sr];
    baseRValue = CURRENT_LATCHES.REGS[baseR];
    memAddress = baseRValue + boffset6;
    leastSigByteSrValue = get_least_sig_byte(srValue);
    mostSigByteSrValue = get_most_sig_byte(srValue);
    MEMORY[memAddress / 2] [0] = leastSigByteSrValue;
    MEMORY[memAddress / 2] [1] = mostSigByteSrValue;
    return;
}

void trap(uint32_t instr){
    uint32_t trapvect8;
    uint32_t newPc;
    trapvect8 = get_trapvector8(instr);
    trapvect8 = trapvect8 << 1;
    newPc = create_word(trapvect8);
    CURRENT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
    CURRENT_LATCHES.PC = newPc;
    return;
}

void xor(uint32_t instr){
    uint32_t isImmediate = get_a(instr);
    uint32_t sr1;
    uint32_t dr;
    uint32_t drValue;
    uint32_t sr1Value;
    uint32_t operand2;
    uint32_t x, y;
    sr1 = get_sr1(instr);
    sr1Value = CURRENT_LATCHES.REGS[sr1];
    dr = get_dr1(instr);
    if (isImmediate) {
        operand2 = get_imm5(instr);
        operand2 = sext(5, operand2);
    } else {
        operand2 = get_sr2(instr);
        operand2 = CURRENT_LATCHES.REGS[operand2];
    }
    x = sr1Value;
    y = operand2;
    drValue = (x|y) & ~(x&y);      //should be XOR
    drValue = drValue & 0xFFFF;
    CURRENT_LATCHES.REGS[dr] = drValue;
    set_new_cc(drValue);
}

/***************************************************************/
/*
 Opcode Test Functions
 */

/***************************************************************/



void print_latches(void) {
    int i = 0;
    printf("PC = %x\n", CURRENT_LATCHES.PC);
    printf("N-Z-P = %d-%d-%d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    for (i = 0; i < 8; i++) {
        printf("R%d = %x (%d)\n", i, CURRENT_LATCHES.REGS[i], CURRENT_LATCHES.REGS[i]);
    }
}

void print_current_latches(void) {
    printf("-----Current Latch----\n");
    print_latches();
}

void print_next_latches(void) {
    printf("-----Next Latch----\n");
    print_latches();
    printf("----------------------\n\n");
}

void add_test(uint32_t instr) {
    print_current_latches();
    add(instr);
    print_next_latches();
}

/***************************************************************/
/*
 Processing Instruction
 */

/***************************************************************/

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
    uint32_t opCode = MEMORY[(pc)/2][1] >> 4;
    uint32_t instr = create_instr(pc);
    print_current_latches();
    CURRENT_LATCHES.PC += 2;
    switch(opCode) {
        case ADD :
            printf("case add\n");
            add(instr);
            break;
        case AND :
            printf("case and\n");
            and(instr);
            break;
        case BR :
            printf("case br\n");
            br(instr);
            break;
        case JMP :
            printf("case jmp\n");
            jmp(instr);
            break;
        case JSR :
            printf("case jsr\n");
            jsr(instr);
            break;
        case LDB :
            printf("case ldb\n");
            ldb(instr);
            break;
        case LDW :
            printf("case ldw\n");
            ldw(instr);
            break;
        case LEA :
            printf("case lea\n");
            lea(instr);
            break;
        case SHF :
            printf("case shf\n");
            shf(instr);
            break;
        case STB :
            printf("case stb\n");
            stb(instr);
            break;
        case STW :
            printf("case stw\n");
            stw(instr);
            break;
        case TRAP :
            printf("case trap\n");
            trap(instr);
            break;
        case XOR :
            printf("case xor\n");
            xor(instr);
            break;
    }
    print_next_latches();
    update_next_latches();
}
