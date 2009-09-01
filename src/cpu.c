/*
    This file is part of SI - A primitive, but simple Space Invaders emulator.
    Copyright 1998 - 2009 Jens Mühlenhoff <j.muehlenhoff@gmx.de>

    SI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "si_types.h"
#include "si.h"

/*
#define PUSH(b)  memory[SP - 1] = (b); SP--;;;  
#define PULL()   memory[SP++];;;;          
#define PUSH(b)  memory[SP] = (b); SP--  
#define PULL()   memory[++SP]         
*/

#define GET_PSW()  ((*CARRY ? 0x01 : 0) | (*FLAG_X1 ? 0x02 : 0) |\
                   (*PARITY ? 0x04 : 0) | (*FLAG_X2 ? 0x08 : 0) |\
                   (*AUX_CARRY ? 0x10 : 0) | (*FLAG_X3 ? 0x20 : 0) |\
                   (*ZERO ? 0x40 : 0) | (*SIGN ? 0x80 : 0))

WORD _DEBUG = 0;

WORD CODE_BEGIN = 0;
WORD CODE_END = 0;
WORD WORKRAM_BEGIN = 0;
WORD WORKRAM_END = 0;
WORD VIDRAM_BEGIN = 0;
WORD VIDRAM_END = 0;
WORD SPECIALRAM = 0;
WORD SPECIALROM = 0;
WORD SPECIALROM_SIZE = 0;

WORD PC = 0x0001; /* Program Counter */
WORD old_cmd1;
WORD old_cmd2;
WORD old_cmd3;
WORD old_cmd4;
WORD old_cmd5;
WORD INTPC = 0;

int ERROR = 0;

/*
   Register : SP = Stack Pointer, PSW = Processor Status Word,
   A = Accumulator, B, C, D, E, H, L
*/

WORD SP = 0x23FF;
BYTE PSW,A,B,C,D,E,H,L;
BYTE SP_UPPER, SP_LOWER;

/* Flags */
WORD flag[8] = {0,1,0,0,0,0,0,0};
/*flag[0] = 0   Carry Flag          */
/*flag[1] = 0   Unused              */
/*flag[2] = 0   Parity Flag         */
/*flag[3] = 0   Unused              */
/*flag[4] = 0   Auxillary Cary Flag */
/*flag[5] = 0   Unused              */
/*flag[6] = 0   Zero Flag           */
/*flag[7] = 0   Sign Flag           */

WORD* CARRY = &flag[0];
WORD* FLAG_X1 = &flag[1];
WORD* PARITY = &flag[2];
WORD* FLAG_X2 = &flag[3];
WORD* AUX_CARRY = &flag[4];
WORD* FLAG_X3 = &flag[5];
WORD* ZERO = &flag[6];
WORD* SIGN = &flag[7];
WORD INT = 1;

/*
WORD wdebug = 0;
WORD BC = 0;
uclock_t test_time = 0;
*/

#include "cpu_tools.h"
#include "cpu_mov_ld_st.h"
#include "cpu_arithmetic.h"
#include "cpu_flow_control.h"
#include "cpu_stack.h"
#include "cpu_special.h"

inline void decode_dd(BYTE instruction, BYTE **upper, BYTE **lower)
{
        switch(instruction &  0x30) // Bits 5 + 4
        {
                case 0x00:
                        *upper = &B;
                        *lower = &C;
                        break;

                case 0x10:
                        *upper = &D;
                        *lower = &E;
                        break;

                case 0x20:
                        *upper = &H;
                        *lower = &L;
                        break;

                case 0x30:
                        *lower = NULL;
                        *upper = NULL;
                        break;
        } 
}

inline void decode_dd_special(BYTE instruction, BYTE **upper, BYTE **lower)
{
        switch(instruction &  0x30) // Bits 4 + 3
        {
                case 0x00:
                        *upper = &B;
                        *lower = &C;
                        break;

                case 0x10:
                        *upper = &D;
                        *lower = &E;
                        break;

                case 0x20:
                        *upper = &H;
                        *lower = &L;
                        break;

                case 0x30:
                        *upper = &A;
                        *lower = &PSW;
                        break;
        } 
}

inline void decode_sss(BYTE instruction, BYTE** source)
{
        switch(instruction & 0x07)
        {
                case 0x00:
                        *source = &B;
                        break;

                case 0x01:
                        *source = &C;
                        break;

                case 0x02:
                        *source = &D;
                        break;

                case 0x03:
                        *source = &E;
                        break;

                case 0x04:
                        *source = &H;
                        break;

                case 0x05:
                        *source = &L;
                        break;

                case 0x06:
                        *source = NULL;
                        break;

                case 0x07:
                        *source = &A;
                        break;
        }
}

inline void decode_ddd(BYTE instruction, BYTE** source)
{
        switch(instruction & 0x38)
        {
                case 0x00:
                        *source = &B;
                        break;

                case 0x08:
                        *source = &C;
                        break;

                case 0x10:
                        *source = &D;
                        break;

                case 0x18:
                        *source = &E;
                        break;

                case 0x20:
                        *source = &H;
                        break;

                case 0x28:
                        *source = &L;
                        break;

                case 0x30:
                        *source = NULL;
                        break;

                case 0x38:
                        *source = &A;
                        break;
        }
}

inline int check_flag_condition(BYTE instruction)
{
        switch(instruction & 0x38)
        {
                case 0x00:
                        return !*ZERO;
                        break;

                case 0x08:
                        return *ZERO;
                        break;

                case 0x10:
                        return !*CARRY;
                        break;

                case 0x18:
                        return *CARRY;
                        break;

                case 0x20:
                        return !*PARITY;
                        break;

                case 0x28:
                        return *PARITY;
                        break;

                case 0x30:
                        return !*SIGN;
                        break;

                case 0x38:
                        return *SIGN;
                        break;
                        
                default:
                        printf("Error: Unkown flag condition\n");
                        return 0;
        }
}

inline int decode_00(BYTE instruction, int cycle_count)
{
        BYTE *upper;
        BYTE *lower;
        BYTE sp_upper;
        BYTE sp_lower;
        if(!instruction) /* NOP */
        {
                cycle_count -= 4;
        }
        else
        if((instruction & 0x0F) == 0x01) /* LXI */
        {
                decode_dd(instruction, &upper, &lower);
                if((!upper) || (!lower))
                {
                        upper = &sp_upper;
                        lower = &sp_lower;
                        sp_upper = SP >> 8;
                        sp_lower = SP & 0xFF;
                }
                opcode_lxi(upper, lower);
                if(upper == &sp_upper)
                {
                        SP = (sp_upper << 8) | sp_lower;
                }
                cycle_count -= 10;
        }
        else
        if((instruction & 0x0F) == 0x09) /* DADI */
        {
                decode_dd(instruction, &upper, &lower);
                if((!upper) || (!lower))
                {
                        upper = &sp_upper;
                        lower = &sp_lower;
                        sp_upper = SP >> 8;
                        sp_lower = SP & 0xFF;
                }
                opcode_dad(upper, lower);
                if(upper == &sp_upper)
                {
                        SP = (sp_upper << 8) | sp_lower;
                }
                cycle_count -= 10;
        }
        else
        if((instruction & 0x07) == 0x02) /* LD/ST */
        {
                if((instruction & 0x08) == 0x08)
                {
                        /* LD */
                        if((instruction & 0x20) == 0x20)
                        {
                                /* Direct */
                                if((instruction & 0x10) == 0x00)
                                {
                                        opcode_lhld();
                                        cycle_count -= 16;
                                }
                                else
                                {
                                        opcode_lda();
                                        cycle_count -= 13;
                                }
                        }
                        else
                        {
                                /* Indirect */
                                if((instruction & 0x10) == 0x00)
                                {
                                        opcode_ldax(&B, &C);
                                        cycle_count -= 7;
                                }
                                else
                                {
                                        opcode_ldax(&D, &E);
                                        cycle_count -= 7;
                                }
                        }
                }
                else
                {
                        /* ST */
                        if((instruction & 0x20) == 0x20)
                        {
                                /* Direct */
                                if((instruction & 0x10) == 0x00)
                                {
                                        opcode_shld();
                                        cycle_count -= 16;
                                }
                                else
                                {
                                        opcode_sta();
                                        cycle_count -= 13;
                                }
                        }
                        else
                        {
                                /* Indirect */
                                if((instruction & 0x10) == 0x00)
                                {
                                        opcode_stax(&B, &C);
                                        cycle_count -= 7;
                                }
                                else
                                {
                                        opcode_stax(&D, &E);
                                        cycle_count -= 7;
                                }
                        }
                }
        }
        else
        if((instruction & 0x07) == 0x03) /* INX / DCX */
        {
                decode_dd(instruction, &upper, &lower);
                if((!upper) || (!lower))
                {
                        upper = &sp_upper;
                        lower = &sp_lower;
                        sp_upper = SP >> 8;
                        sp_lower = SP & 0xFF;
                }
                if((instruction & 0x08) == 0x08)
                {
                        opcode_dcx(upper, lower);
                }
                else
                {
                        opcode_inx(upper, lower);
                }
                if(upper == &sp_upper)
                {
                        SP = (sp_upper << 8) | sp_lower;
                }
                cycle_count -= 5;
        }
        else
        if((instruction & 0x06) == 0x04) /* INR / DCR */
        {
                decode_ddd(instruction, &lower);
                if((instruction & 0x01) == 0x01)
                {
                        opcode_dcr(lower);
                }
                else
                {
                        opcode_inr(lower);
                }
                if(!lower)
                {
                        cycle_count -= 5;
                }
                cycle_count -= 5;
        }
        else
        if((instruction & 0x07) == 0x06) /* MVI */
        {
                decode_ddd(instruction, &lower);
                opcode_mvi(lower);
                cycle_count -= 7;
                if(!lower)
                {
                        cycle_count -= 3;
                }
        }
        else
        if((instruction & 0x27) == 0x07) /* ROTATE */
        {
                if((instruction & 0x08) == 0x00) /* LEFT */
                {
                        if((instruction & 0x10) == 0x00) /* NO CARRY */
                        {
                                opcode_ral();
                        }
                        else /* CARRY */
                        {
                                opcode_rlc();
                        }
                }
                else /* RIGHT */
                {
                        if((instruction & 0x10) == 0x00) /* NO CARRY */
                        {
                                opcode_rar();
                        }
                        else /* CARRY */
                        {
                                opcode_rrc();
                        }
                }
                cycle_count -= 4;
        }
        else
        if((instruction & 0x27) == 0x27) /* SPECIAL */
        {
                switch(instruction &  0x18) // Bits 4 + 3
                {
                        case 0x00: /* DAA */
                                opcode_daa();
                                break;
        
                        case 0x08: /* CMA */
                                opcode_cma();
                                break;
        
                        case 0x10: /* STC */
                                opcode_stc();
                                break;
        
                        case 0x18: /* CMC */
                                opcode_cmc();
                                break;
                } 
                cycle_count -= 4;
        }
        else
        {
                printf("Opcode not implemented: %2X", instruction);
                return -1000;
        }
        return cycle_count;
}

inline int decode_10(BYTE instruction, int cycle_count)
{
        BYTE *lower;
        decode_sss(instruction, &lower);
        if((instruction & 0x20) == 0x00) /* ADD / SUB */
        {
                if((instruction & 0x10) == 0x00) /* ADD */
                {
                        if((instruction & 0x08) == 0x00) /* NO CARRY */
                        {
                                opcode_add(lower);
                        }
                        else /* CARRY */
                        {
                                opcode_adc(lower);
                        }
                }
                else /* SUB */
                {
                        if((instruction & 0x08) == 0x00) /* NO CARRY */
                        {
                                opcode_sub(lower);
                        }
                        else /* CARRY */
                        {
                                opcode_sbb(lower);
                        }
                }
                cycle_count -= 4;
        }
        else /* AND / XOR / OR / CMP */
        {
                switch(instruction &  0x18) // Bits 4 + 3
                {
                        case 0x00: /* AND */
                                opcode_ana(lower);
                                break;
        
                        case 0x08: /* XOR */
                                opcode_xra(lower);
                                break;
        
                        case 0x10: /* OR */
                                opcode_ora(lower);
                                break;
        
                        case 0x18: /* CMP */
                                opcode_cmp(lower);
                                break;
                } 
        }
        if(lower)
        {
                return cycle_count - 4;
        }
        else
        {
                return cycle_count - 7;
        }
}

inline int decode_11(BYTE instruction, int cycle_count)
{
        BYTE *upper;
        BYTE *lower;
        if((instruction & 0x0B) == 0x01) /* PUSH / POP */
        {
                decode_dd_special(instruction, &upper, &lower);
                if((instruction & 0x04) == 0x04) /* PUSH */
                {
                        if(lower == &PSW)
                        {
                                PSW = GET_PSW();
                        }
                        opcode_push(upper, lower);
                        cycle_count -= 11;
                }
                else
                {
                        opcode_pop(upper, lower);
                        cycle_count -= 10;
                        if(lower == &PSW)
                        {
                                set_flags();
                        }
                }
        }
        else
        if((instruction & 0x27) == 0x26) /* AND / XOR / OR / CMP immediate */
        {
                switch(instruction &  0x18) // Bits 4 + 3
                {
                        case 0x00: /* AND */
                                opcode_ani();
                                break;
        
                        case 0x08: /* XOR */
                                opcode_xri();
                                break;
        
                        case 0x10: /* OR */
                                opcode_ori();
                                break;
        
                        case 0x18: /* CMP */
                                opcode_cpi();
                                break;
                } 
                cycle_count -= 7;
        }
        else
        if((instruction & 0x27) == 0x06) /* ADD / SUB immediate */
        {
                switch(instruction &  0x18) // Bits 4 + 3
                {
                        case 0x00: /* ADI */
                                opcode_adi();
                                break;
        
                        case 0x08: /* ACI */
                                opcode_aci();
                                break;
        
                        case 0x10: /* SUI */
                                opcode_sui();
                                break;
        
                        case 0x18: /* SBI */
                                opcode_sbi();
                                break;
                } 
                cycle_count -= 7;
        }
        else
        if((instruction & 0x3F) == 0x03) /* JMP */
        {
                opcode_jmp();
                cycle_count -= 10;
        }
        else
        if((instruction & 0x3F) == 0x09) /* RET */
        {
                opcode_ret();
                cycle_count -= 10;
        }
        else
        if((instruction & 0x3F) == 0x0D) /* CALL */
        {
                opcode_call();
                cycle_count -= 17;
        }
        else
        if((instruction & 0x3F) == 0x13) /* OUT */
        {
                opcode_out();
                cycle_count -= 10;
        }
        else
        if((instruction & 0x3F) == 0x1B) /* IN */
        {
                opcode_in();
                cycle_count -= 10;
        }
        else
        if((instruction & 0x3F) == 0x23) /* XTHL */
        {
                opcode_xthl();
                cycle_count -= 18;
        }
        else
        if((instruction & 0x3F) == 0x29) /* PCHL */
        {
                opcode_pchl();
                cycle_count -= 5;
        }
        else
        if((instruction & 0x3F) == 0x2B) /* XCHG */
        {
                opcode_xchg();
                cycle_count -= 4;
        }
        else
        if((instruction & 0x3F) == 0x33) /* DI */
        {
                opcode_di();
                cycle_count -= 4;
        }
        else
        if((instruction & 0x3F) == 0x3B) /* EI */
        {
                opcode_ei();
                cycle_count -= 4;
        }
        else
        if(((instruction & 0x01) == 0x00) && ((instruction & 0x06) != 0x06)) /* RET / CALL / JMP CCC */
        {
                switch(instruction & 0x06)
                {
                        case 0x00: /* RET */
                                if(check_flag_condition(instruction))
                                {
                                        opcode_ret();
                                        cycle_count -= 11;
                                }
                                else
                                {
                                        cycle_count -= 5;
                                }
                                break;

                        case 0x04: /* CALL */
                                if(check_flag_condition(instruction))
                                {
                                        opcode_call();
                                        cycle_count -= 17;
                                }
                                else
                                {
                                        // CALL and JMP have an additional argument that must be ignored if not taken
                                        PC += 2;
                                        cycle_count -= 11;
                                }
                                break;

                        case 0x02: /* JMP */
                                if(check_flag_condition(instruction))
                                {
                                        opcode_jmp();
                                        cycle_count -= 10;
                                }
                                else
                                {
                                        // CALL and JMP have an additional argument that must be ignored if not taken
                                        PC += 2;
                                        cycle_count -= 10;  // This is NOT an error, at least the intel specification says so
                                }
                                break;
                }
        }
        else
        {
                printf("Opcode not implemented: %2X", instruction);
                return -1000;
        }
        return cycle_count;
}

inline int decode(BYTE instruction, int cycle_count)
{
        BYTE *bsrc = NULL;
        BYTE *bdest = NULL;
        switch(instruction & 0xC0) /* Upper two bits */
        {
                case 0x00:
                        cycle_count = decode_00(instruction, cycle_count);
                        break;                        
                        
                case 0x40:  /* MOV or HLT */
                        decode_sss(instruction, &bsrc);
                        decode_ddd(instruction, &bdest);
                        if((!bsrc) && (!bdest))
                        {
                                /* HLT */
                                /* TODO: Should halt the CPU */
                                printf("HLT not supported!\n");
                                cycle_count = -1000; 
                        }
                        opcode_mov(bdest, bsrc);
                        cycle_count -= 4;
                        if((!bsrc) || (!bdest))
                        {
                                cycle_count -= 3;
                        }
                        break;

                case 0x80:
                        cycle_count = decode_10(instruction, cycle_count);
                        break;                        

                case 0xC0:
                        cycle_count = decode_11(instruction, cycle_count);
                        break;                        

                default:
                        printf("Unkown opcode %2X", instruction);
                        cycle_count = -1000;
        }
        
        if(ERROR)
        {
                printf("Error condition!");
                cycle_count = -1000;
        }

        return cycle_count;
}

WORD cpu(WORD cycles)
{
        int cycle_count = cycles;
        int fdbg = 0;

        do
        {
                //BC++;
                if(!SPECIALROM)
                {
                        if(PC > CODE_END)
                        {
                                printf("PC hat den Gueltigkeitsbereich verlassen\tPC = %4X\n",PC);
                                return -1000;
                        }
                }
                else
                {
                        if(PC > (VIDRAM_END + SPECIALROM_SIZE))
                        {
                                printf("PC hat den Gueltigkeitsbereich verlassen\tPC = %4X\n",PC);
                                return -1000;
                        }
                }

                //if(DEBUG == 2 && wdebug)
                if(_DEBUG == 2)
                {
                        //if(BC > 800000)
                        //{
                        //        fprintf(debug2,"%4X, %2X, %2X, %2X, ", PC, memory[PC], A, GET_PSW());
                        //        fprintf(debug2,"%2X, %2X, %2X, %2X, %2X, %2X, ", B, C, D, E, H, L);
                        //        fprintf(debug2,"%2X %2X %2X %2X %2X %2X\n", memory[SP], memory[SP + 1], memory[SP + 2], memory[SP + 3], memory[SP + 4], memory[SP + 5]);
                        //}
                        //if(BC > 850000)
                        //{
                        //        test_time = uclock();
                        //        while(uclock() < test_time + 1829943);
                        //        return 0;
                        //}
                        //1395520
                        //if(BC > 1395000)
                        //{
                        //fprintf(debug2,"%d",BC);
                        //if(PC == 0x15DE)fprintf(debug2,"HL = %4X",((H << 8) + L));
                        //fprintf(debug2,"PC = %4X, Opcode = %2X, A = %2X, B = %2X, Stack = %2X %2X %2X %2X %2X %2X\n", PC, memory[PC], A, B, memory[SP + 1], memory[SP + 2], memory[SP + 3], memory[SP + 4], memory[SP + 5], memory[SP + 6]);
                        //fprintf(debug2,"%4X, %2X, %2X, %2X, ", PC, memory[PC], A, GET_PSW());
                        //fprintf(debug2,"%2X, %2X, %2X, %2X, %2X, %2X, ", B, C, D, E, H, L);
                        //fprintf(debug2,"%2X %2X %2X %2X %2X %2X\n", memory[SP], memory[SP + 1], memory[SP + 2], memory[SP + 3], memory[SP + 4], memory[SP + 5]);
                        //}
                }
                //if(PC == 0x1785)
                //{
                //        fdbg = 1;
                //}
                if(fdbg)
                {
                        dump();
                }
                old_cmd5 = old_cmd4;
                old_cmd4 = old_cmd3;
                old_cmd3 = old_cmd2;
                old_cmd2 = old_cmd1;
                old_cmd1 = memory[PC];
                cycle_count = decode(memory[PC++], cycle_count);
                if(cycle_count == -1000)
                {
                        dump();
                }
                //printf("cycle_count = %d\n", cycle_count);
                /*if(cycle_count == 0)
                {
                        return 0;
                }*/

        } while(cycle_count > 0);
        
        if(ERROR == 1)
        {
                dump();
                exit(5);
                // return -1000;
        }

        return cycles - cycle_count;
}

void interrupt(void)
{
        memory[SP - 1] = PC >> 8;
        memory[SP - 2] = PC & 0xFF;
        SP -= 2;
        PC = 0x08;
        INT = 0;
}

void NMI(void)
{
        memory[SP - 1] = PC >> 8;
        memory[SP - 2] = PC & 0xFF;
        SP -= 2;
        PC = 0x10;
        INT = 0;
}

void reset(void)
{
        PC = 0x00;
        SP = 0x23FF;
        A = 0x00;
        PSW = 0x00;
        B = 0x00;
        C = 0x00;
        D = 0x00;
        E = 0x00;
        H = 0x00;
        L = 0x00;
        // test_zero = 0;
        INT = 1;
        set_flags();
}