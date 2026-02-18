/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

const uint16_t inst16_table[] = {
  {0x0}, //NOP
};

enum{
    e_NOP,

};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,
    // TO-DO: Add more types as needed
} Op_Code_t;


int main()
{
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2)
    {
        instruction = (Op_Code_t*) &flash_mem[idx];
        if (instruction->op16 == inst16_table[e_NOP])
        {
            printf("NOP\n");
        }else if((instruction->op16 & 0xFC00) == 0x2400){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r1<<4)|instruction->type2.r4;

            printf("EOR R%d, R%d\n",Rd,Rr);
        }else if((instruction->op16 & 0xF000)==0xE000){
            uint8_t k = ((instruction->op16>>4)& 0xF0)|(instruction->op16&0x0F);
            uint8_t d = ((instruction->op16>>4)&0x0F);
            uint8_t Rd = 16+d;
            printf("LDI R%d, 0x%02X\n",Rd, k);
        }else if((instruction->op16 & 0xF000)==0x9000){
            uint8_t Rd = ((instruction->op16>>4)&0x1F);
            printf("LD R%d,x+\n", Rd);
        }else if((instruction->op16&0xF000)==0x3000){
            uint8_t Rd = (instruction->op16&0xF0);
            uint8_t k = ((instruction->op16>>4)&0xF0)|(instruction->op16&0x0F); 
            Rd+=16;
            printf("CPI R%d, 0x%02X\n",Rd,k);
        }else if((instruction->op16&0xFC07)==0xF401){
            int8_t k = (instruction->op16&0x03F8)>>3;
            if(k&0x40){
                k|=0x80;
            }
            printf("BRNE %d\n",k);
        }
        else
        {
            printf("unknown\n");
        }
    }
    return 0;
}