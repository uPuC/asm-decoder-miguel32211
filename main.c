/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 0xD2, 
    0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 0x01, 0x17,
    0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

const uint16_t inst16_table[] = {
  0x0, //NOP
  0x2400, //CLR - EOR
  0xE000, //LDI
  0x9000, //LD
  0x3000, // CPI
  0xF401, //BRNE
  0x9700, // SBIW
  0xD000, //RECALL
  0x0800, // SBC
  0x1C00, // ADC
  0x0100,//MOVW
  0x5000, // SUBI
  0x6000, // ORI
  0x1400, //CP
  0x2C00, //MOV
  0x9400, // INC
};

enum{
    e_NOP,
    e_CLR,
    e_LDI,
    e_LD,
    e_CPI,
    e_BRNE,
    e_SBIW,
    e_RECALL,
    e_SBC,
    e_ADC,
    e_MOVW,
    e_SUBI,
    e_ORI,
    e_CP,
    e_MOV,
    e_INC,
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // LD
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // EOR - CLR, SBC, ADC, CP
    struct{
        uint16_t k4:4;
        uint16_t d4:4;
        uint16_t kk4:4;
        uint16_t op4:4;
    }type3; // LDI, CPI, SUBI, ORI
    struct{
        uint16_t op3:3;
        uint16_t k6:7;
        uint16_t op6:6;
    }type4; // BRNE
    struct{
        uint16_t k4:4;
        uint16_t d2:2;
        uint16_t k2:2;
        uint16_t op8:8;
    }type5; //SBIW
    struct{
        uint16_t k12:12;
        uint16_t op4:4;
    }type6; //RCALL
    struct{
        uint16_t r4:4;
        uint16_t d4:4;
        uint16_t op8:8;
    }type7; //MOVW
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type8; //INC
    
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
        }else if((instruction->op16 & 0xFC00) == inst16_table[e_CLR]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = instruction->type2.r4 | instruction->type2.r1;

            if(Rd ==Rr){
                printf("CLR R%d\n",Rd);
            }
        }else if((instruction->op16 & 0xF000)==inst16_table[e_LDI]){
            uint8_t Rd = instruction->type3.d4;
            Rd+=16;

            uint8_t k = instruction->type3.k4 | instruction->type3.kk4;
            printf("LDI R%d, 0x%02X\n",Rd, k);

        }else if((instruction->op16 & 0xFE00)== inst16_table[e_LD]){
            uint8_t Rd = (instruction->type1.d5);
            uint8_t x = instruction->type1.op4;

            if(x==0xC){
                printf("LD R%d,X\n", Rd);
            }else if(x==0xD){
                printf("LD R%d,X+\n", Rd);
            }else{
                printf("LD R%d,-X\n", Rd);
            }       
        }else if((instruction->op16&0xF000)== inst16_table[e_CPI]){
            uint8_t Rd = instruction->type3.d4;
            Rd+=16;
            uint8_t k = instruction->type3.k4 | instruction->type3.kk4; 
            
            printf("CPI R%d, 0x%02X\n",Rd,k);

        }else if((instruction->op16&0xFC07)== inst16_table[e_BRNE]){
            int8_t k = instruction->type4.k6;

            if(k&0x40){
                k|=0x80;
            }
            printf("BRNE %d\n",k);

        }else if((instruction->op16&0xFF00)== inst16_table[e_SBIW]){
            uint8_t Rd = instruction->type5.d2;
            Rd = 2*Rd+24;

            uint8_t k = instruction->type5.k4 | instruction->type5.k2;

            printf("SBIW R%d, %02X\n", Rd, k); //*** */ */

        }else if((instruction->op16&0xF000) == inst16_table[e_RECALL]){
            int16_t k = instruction->type6.k12;

            if(k&0x0800){
                k|=0xF000;
            }

            printf("RCALL %d\n", k);

        }else if((instruction->op16&0xFC00)==inst16_table[e_SBC]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r1)|instruction->type2.r4;

            printf("SBC R%d, R%d\n", Rd, Rr);

        }else if((instruction->op16 & 0xFC00) == inst16_table[e_ADC]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r1)| instruction->type2.r4;

            printf("ADC R%d, R%d\n",Rd,Rr);

        }else if((instruction->op16&0xFF00) == inst16_table[e_MOVW]){
            uint8_t Rd = instruction->type7.d4;
            Rd*=2;

            uint8_t Rr = instruction->type7.r4;
            Rr*=2;

            printf("MOVW R%d, R%d\n", Rd, Rr);

        }else if((instruction->op16&0xF000)== inst16_table[e_SUBI]){
            uint8_t Rd = instruction->type3.d4;
            Rd+=16;

            uint16_t k = instruction->type3.k4 | instruction->type3.kk4;
            
            printf("SUBI R%d, %02X\n", Rd, k);

        }else if((instruction->op16&0xF000)==inst16_table[e_ORI]){
            uint8_t Rd = instruction->type3.d4;
            Rd+=16;

            uint8_t k = (instruction->type3.k4)|(instruction->type3.kk4);

            printf("ORI R%d, %02X\n", Rd, k);

        }else if((instruction->op16&0xFC00)==inst16_table[e_CP]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r4) | (instruction->type2.r1); 

            printf("CP R%d, R%d\n", Rd, Rr);

        }else if((instruction->op16&0xFC00)==inst16_table[e_MOV]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r4) | instruction->type2.r1;

            printf("MOV R%d, R%d\n",Rd, Rr);
        }else if((instruction->op16&0xFE00)==inst16_table[e_INC]){
            uint8_t Rd = instruction->type8.d5;
            printf("INC R%d\n",Rd);
        }
        else
        {
            printf("unknown\n");
        }
        
    }
    return 0;
}