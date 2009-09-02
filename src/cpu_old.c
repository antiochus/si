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
#include "cpu_arithmetic.h"

inline int decode(BYTE instruction, int cycle_count)
{
        BYTE btemp, btemp2;
        WORD wtemp;
        int temp, temp2, temp3;

        // fprintf(debug, "%4X", PC - 1);
        if(instruction == 0x94)
        {
                fprintf(stderr, "Hello World!\n");
        }
        switch(instruction)
        {
        case 0x00: /* NOP (No Operation) [XCHG A,A]*/
                cycle_count -= 4;
                break;

        case 0x01: /* LXI BC, 0x???? */
                B = read_memory((PC + 1));
                C = read_memory(PC);
                PC += 2;
                cycle_count -= 10;
                break;

        case 0x02: /* STAX BC */
                temp = (B << 8) | C;
                write_memory(temp,A);
                cycle_count -= 7;
                break;

        case 0x03: /* INX BC */
                temp = ((B << 8) | C);
                temp++;
                B = temp >> 8;
                C = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x04: /* INR B */
                opcode_inr(&B);
                cycle_count -= 5;
                break;

        case 0x05: /* DCR B */
                opcode_dcr(&B);
                cycle_count -= 5;
                break;

        case 0x06: /* MVI B, 0x?? */
                B = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x07: /* RLC */
                btemp = A;
                A = A << 1;
                btemp &= 0x80;
                if(btemp)btemp = 1;
                A |= btemp;
                *CARRY = 0;
                *CARRY |= btemp;
                cycle_count -= 4;
                break;

        //case 0x08: /* DB 0x08 */

        case 0x09: /* DAD BC */
                temp = (B << 8) | C;
                temp2 = (H << 8) | L;
                temp2 += temp;
                H = temp2 >> 8;
                L = temp2 & 0xFF;
                if(temp2 >= 0x10000)*CARRY = 1;
                else *CARRY = 0;
                cycle_count -= 10;
                break;

        case 0x0A: /* LDAX BC */
                A = read_memory(((B << 8) | C));
                cycle_count -= 7;
                break;

        case 0x0B: /* DCX BC */
                temp = ((B << 8) | C);
                temp--;
                B = temp >> 8;
                C = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x0C: /* INR C */
                opcode_inr(&C);
                cycle_count -= 5;
                break;

        case 0x0D: /* DCR C */
                opcode_dcr(&C);
                cycle_count -= 5;
                break;

        case 0x0E: /* MVI C, 0x?? */
                C = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x0F: /* RRC */
                btemp = A;
                A = A >> 1;
                btemp &= 0x01;
                if(btemp)btemp = 0x80;
                A |= btemp;
                if(btemp)*CARRY = 1;
                else *CARRY = 0;
                cycle_count -= 4;
                break;

        //case 0x10: /* DB 0x10 */

        case 0x11: /* LXI DE, 0x???? */
                D = read_memory(PC + 1);
                E = read_memory(PC);
                PC += 2;
                cycle_count -= 10;
                break;

        case 0x12: /* STAX DE */
                temp = (D << 8) | E;
                write_memory(temp,A);
                cycle_count -= 7;
                break;

        case 0x13: /* INX DE */
                temp = ((D << 8) | E);
                temp++;
                D = temp >> 8;
                E = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x14: /* INR D */
                opcode_inr(&D);
                cycle_count -= 5;
                break;

        case 0x15: /* DCR D */
                opcode_dcr(&D);
                cycle_count -= 5;
                break;

        case 0x16: /* MVI D, 0x?? */
                D = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x17: /* RAL */
                btemp = A;
                A = A << 1;
                btemp &= 0x80;
                if(btemp)btemp = 0x01;
                A |= btemp;
                *CARRY = 0;
                *CARRY |= btemp;
                cycle_count -= 4;
                break;

        //case 0x18: /* DB 0x18 */

        case 0x19: /* DAD DE */
                temp = (D << 8) | E;
                temp2 = (H << 8) | L;
                temp2 += temp;
                H = temp2 >> 8;
                L = temp2 & 0xFF;
                if(temp2 >= 0x10000)*CARRY = 1;
                else *CARRY = 0;
                cycle_count -= 10;
                break;

        case 0x1A: /* LDAX DE */
                A = read_memory(((D << 8) | E));
                cycle_count -= 7;
                break;

        case 0x1B: /* DCX DE */
                temp = (D << 8) | E;
                temp--;
                D = temp >> 8;
                E = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x1C: /* INR E */
                opcode_inr(&E);
                cycle_count -= 5;
                break;

        case 0x1D: /* DCR E */
                opcode_dcr(&E);
                cycle_count -= 5;
                break;

        case 0x1E: /* MVI E, 0x?? */
                E = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x1F: /* RAR */
                btemp = A;
                A = A >> 1;
                btemp &= 0x01;
                if(btemp)btemp = 0x80;
                A |= btemp;
                *CARRY = 0;
                *CARRY |= btemp;
                if(*CARRY)*CARRY = 1;
                cycle_count -= 4;
                break;

        //case 0x20: /* DB 20 ???(RIM ???) */

        case 0x21: /* LXI HL, 0x???? */
                H = read_memory(PC + 1);
                L = read_memory(PC);
                PC += 2;
                cycle_count -= 10;
                break;

        case 0x22: /* SHLD 0x???? */
                temp = ((read_memory(PC + 1) << 8) | read_memory(PC));
                write_memory((temp + 1),H);
                write_memory(temp,L);
                PC += 2;
                cycle_count -= 16;
                break;

        case 0x23: /* INX HL */
                temp = ((H << 8) | L);
                temp++;
                H = temp >> 8;
                L = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x24: /* INR H */
                opcode_inr(&H);
                cycle_count -= 5;
                break;

        case 0x25: /* DCR H */
                opcode_dcr(&H);
                cycle_count -= 5;
                break;

        case 0x26: /* MVI H, 0x?? */
                H = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x27: /* DAA */
                if((A & 0x0F) > 9 || (*AUX_CARRY == 1))
                {
                        A += 0x06;
                        *AUX_CARRY = 1;
                }
                else *AUX_CARRY = 0;
                if(A > 0x9F || (*CARRY == 1))
                {
                        A += 0x60;
                        *CARRY = 1;
                }
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 5;
                break;

        //case 0x28: /* DB 0x28 */

        case 0x29: /* DAD HL */
                temp = (H << 8) | L;
                temp += temp;
                H = temp >> 8;
                L = temp & 0xFF;
                if(temp >= 0x10000)*CARRY = 1;
                else *CARRY = 0;
                cycle_count -= 10;
                break;

        case 0x2A: /* LHLD, 0x???? */
                temp = ((read_memory(PC + 1) << 8) | read_memory(PC));
                H = read_memory(temp + 1);
                L = read_memory(temp);
                PC += 2;
                cycle_count -= 16;
                break;

        case 0x2B: /* DCX HL */
                temp = (H << 8) | L;
                temp--;
                H = temp >> 8;
                L = temp & 0xFF;
                cycle_count -= 5;
                break;

        case 0x2C: /* INR L */
                opcode_inr(&L);
                cycle_count -= 5;
                break;

        case 0x2D: /* DCR L */
                opcode_dcr(&L);
                cycle_count -= 5;
                break;

        case 0x2E: /* MVI L, 0x?? */
                L = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x2F: /* CMA */
                A = ~A;
                cycle_count -= 4;
                break;

        //case 0x30: /* DB 0x30 ???(SIM ???) */

        case 0x31: /* LXI SP, 0x???? */
                SP = ((read_memory(PC + 1) << 8) | read_memory(PC));
                PC += 2;
                cycle_count -= 10;
                break;

        case 0x32: /* STA 0x???? */
                write_memory((read_memory(PC + 1) << 8) | read_memory(PC),A);
                PC += 2;
                cycle_count -= 13;
                break;

        case 0x33: /* INX SP */
                SP++;
                cycle_count -= 5;

        case 0x34: /* INR M */
                btemp = read_memory(((H << 8) | L));
                opcode_inr(&btemp);
                write_memory(((H << 8) | L), btemp);
                cycle_count -= 10;
                break;

        case 0x35: /* DCR M */
                btemp = read_memory(((H << 8) | L));
                opcode_dcr(&btemp);
                write_memory(((H << 8) | L), btemp);
                cycle_count -= 10;
                break;

        case 0x36: /* MVI M, 0x?? */
                wtemp = (H << 8) | L;
                write_memory(wtemp,read_memory(PC));
                PC++;
                cycle_count -= 10;
                break;

        case 0x37: /* STC (SET CARRY) */
                *CARRY = 1;
                cycle_count -= 4;
                break;

        //case 0x38 /* DB 0x38 */

        case 0x39: /* DAD SP */
                temp = (H << 8) | L;
                temp += SP;
                H = temp >> 8;
                L = temp & 0xFF;
                if(temp >= 0x10000)*CARRY = 1;
                else *CARRY = 0;
                cycle_count -= 10;
                break;

        case 0x3A: /* LDA, 0x???? */
                temp = (read_memory(PC + 1) << 8) | read_memory(PC);
                A = read_memory(temp);
                PC += 2;
                cycle_count -= 13;
                break;

        case 0x3B: /* DCX SP */
                SP--;
                cycle_count -= 5;
                break;

        case 0x3C: /* INR A */
                opcode_inr(&A);
                cycle_count -= 5;
                break;

        case 0x3D: /* DCR A */
                opcode_dcr(&A);
                cycle_count -= 5;
                break;

        case 0x3E: /* MVI A, 0x?? */
                A = read_memory(PC);
                PC++;
                cycle_count -= 7;
                break;

        case 0x3F: /* CMC */
                *CARRY = ~*CARRY;
                cycle_count -= 4;
                break;

        case 0x40: /* MOV B,B */
                B = B;
                cycle_count -= 4;
                break;

        case 0x41: /* MOV B,C */
                B = C;
                cycle_count -= 4;
                break;

        case 0x42: /* MOV B,D */
                B = D;
                cycle_count -= 4;
                break;

        case 0x43: /* MOV B,E */
                B = E;
                cycle_count -= 4;
                break;

        case 0x44: /* MOV B,H */
                B = H;
                cycle_count -= 4;
                break;

        case 0x45: /* MOV B,L */
                B = L;      
                cycle_count -= 4;
                break;

        case 0x46: /* MOV B,M */
                temp = (H << 8) | L;
                B = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x47: /* MOV B,A */
                B = A;
                cycle_count -= 4;
                break;

        case 0x48: /* MOV C,B */
                C = B;
                cycle_count -= 4;
                break;

        case 0x49: /* MOV C,C */
                C = C;
                cycle_count -= 4;
                break;

        case 0x4A: /* MOV C,D */
                C = D;
                cycle_count -= 4;
                break;

        case 0x4B: /* MOV C,E */
                C = E;
                cycle_count -= 4;
                break;

        case 0x4C: /* MOV C,H */
                C = H;
                cycle_count -= 4;
                break;

        case 0x4D: /* MOV C,L */
                C = L;
                cycle_count -= 4;
                break;

        case 0x4E: /* MOV C,M */
                temp = (H << 8) | L;
                C = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x4F: /* MOV C,A */
                C = A;
                cycle_count -= 4;
                break;

        case 0x50: /* MOV D,B */
                D = B;
                cycle_count -= 4;
                break;

        case 0x51: /* MOV D,C */
                D = C;
                cycle_count -= 4;
                break;

        case 0x52: /* MOV D,D */
                D = D;
                cycle_count -= 4;
                break;

        case 0x53: /* MOV D,E */
                D = E;
                cycle_count -= 4;
                break;

        case 0x54: /* MOV D,H */
                D = H;
                cycle_count -= 4;
                break;

        case 0x55: /* MOV D,L */
                D = L;
                cycle_count -= 4;
                break;

        case 0x56: /* MOV D,M */
                temp = (H << 8) | L;
                D = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x57: /* MOV D,A */
                D = A;
                cycle_count -= 4;
                break;

        case 0x58: /* MOV E,B */
                E = B;
                cycle_count -= 4;
                break;

        case 0x59: /* MOV E,C */
                E = C;
                cycle_count -= 4;
                break;

        case 0x5A: /* MOV E,D */
                E = D;
                cycle_count -= 4;
                break;

        case 0x5B: /* MOV E,E */
                E = E;
                cycle_count -= 4;
                break;

        case 0x5C: /* MOV E,H */
                E = H;
                cycle_count -= 4;
                break;

        case 0x5D: /* MOV E,L */
                E = L;
                cycle_count -= 4;
                break;

        case 0x5E: /* MOV E,M */
                temp = (H << 8) | L;
                E = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x5F: /* MOV E,A */
                E = A;
                cycle_count -= 4;
                break;

        case 0x60: /* MOV H,B */
                H = B;
                cycle_count -= 4;
                break;

        case 0x61: /* MOV H,C */
                H = C;
                cycle_count -= 4;
                break;

        case 0x62: /* MOV H,D */
                H = D;
                cycle_count -= 4;
                break;
                
        case 0x63: /* MOV H,E */
                H = E;
                cycle_count -= 4;
                break;

        case 0x64: /* MOV H,H */
                H = H;
                cycle_count -= 4;
                break;

        case 0x65: /* MOV H,L */
                H = L;
                cycle_count -= 4;
                break;

        case 0x66: /* MOV H,M */
                temp = (H << 8) | L;
                H = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x67: /* MOV H,A */
                H = A;
                cycle_count -= 4;
                break;

        case 0x68: /* MOV L,B */
                L = B;
                cycle_count -= 4;
                break;

        case 0x69: /* MOV L,C */
                L = C;
                cycle_count -= 4;
                break;

        case 0x6A: /* MOV L,D */
                L = D;
                cycle_count -= 4;
                break;

        case 0x6B: /* MOV L,E */
                L = E;
                cycle_count -= 4;
                break;

        case 0x6C: /* MOV L,H */
                L = H;
                cycle_count -= 4;
                break;

        case 0x6D: /* MOV L,L */
                L = L;
                cycle_count -= 4;
                break;

        case 0x6E: /* MOV L,M */
                temp = (H << 8) | L;
                L = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x6F: /* MOV L,A */
                L = A;
                cycle_count -= 4;
                break;

        case 0x70: /* MOV M,B */
                temp = (H << 8) | L;
                write_memory(temp,B);
                cycle_count -= 7;
                break;

        case 0x71: /* MOV M,C */
                temp = (H << 8) | L;
                write_memory(temp,C);
                cycle_count -= 7;
                break;

        case 0x72: /* MOV M,D */
                temp = (H << 8) | L;
                write_memory(temp,D);
                cycle_count -= 7;
                break;

        case 0x73: /* MOV M,E */
                temp = (H << 8) | L;
                write_memory(temp,E);
                cycle_count -= 7;
                break;

        case 0x74: /* MOV M,H */
                temp = (H << 8) | L;
                write_memory(temp,H);
                cycle_count -= 7;
                break;

        case 0x75: /* MOV M,L */
                temp = (H << 8) | L;
                write_memory(temp,L);
                cycle_count -= 7;
                break;

        //case 0x76: /* HLT */

        case 0x77: /* MOV M,A */
                temp = (H << 8) | L;
                write_memory(temp,A);
                cycle_count -= 7;
                break;

        case 0x78: /* MOV A,B */
                A = B;
                cycle_count -= 4;
                break;

        case 0x79: /* MOV A,C */
                A = C;
                cycle_count -= 4;
                break;

        case 0x7A: /* MOV A,D */
                A = D;
                cycle_count -= 4;
                break;

        case 0x7B: /* MOV A,E */
                A = E;
                cycle_count -= 4;
                break;

        case 0x7C: /* MOV A,H */
                A = H;
                cycle_count -= 4;
                break;

        case 0x7D: /* MOV A,L */
                A = L;
                cycle_count -= 4;
                break;

        case 0x7E: /* MOV A,M */
                temp = (H << 8) | L;
                A = read_memory(temp);
                cycle_count -= 7;
                break;

        case 0x7F: /* MOV A,A */
                A = A;
                cycle_count -= 4;
                break;

        case 0x80: /* ADD B */
                btemp = A;
                A += B;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x81: /* ADD C */
                btemp = A;
                A += C;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x82: /* ADD D */
                btemp = A;
                A += D;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x83: /* ADD E */
                btemp = A;
                A += E;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x84: /* ADD H */
                btemp = A;
                A += H;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x85: /* ADD L */
                btemp = A;
                A += L;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x86: /* ADD M */
                btemp = A;
                A += read_memory((H << 8) | L);
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                temp3 = 0;
                parity(A);
                cycle_count -= 7;
                break;

        case 0x8A: /* ADC D */
                btemp = A;
                A = A + D + *CARRY;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp > A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x90: /* SUB B */
                btemp = A;
                A -= B;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A > btemp)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0x96: /* SUB M */
                btemp = A;
                A -= read_memory((H << 8) | L);
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A > btemp)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xA0: /* ANA B */
                A &= B;
                *AUX_CARRY = 0;
                *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xA6: /* ANA M */
                A &= read_memory((H << 8) | L);
                *AUX_CARRY = 0;
                *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xA7: /* ANA A */
                A &= A;
                *AUX_CARRY = 0;
                *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xA8: /* XRA B */
                A ^= B;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xAE: /* XRA M */
                A ^= read_memory((H << 8) | L);
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xAF: /* XRA A */
                A ^= A;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xB0: /* ORA B */
                A |= B;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xB4: /* ORA H */
                A |= H;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xB5: /* ORA L */
                A |= L;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;

        case 0xB6: /* ORA M */
                A |= read_memory((H << 8) | L);
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xB7: /* ORA A */
                A |= A;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 4;
                break;
        case 0xB8: /* CMP B */
                btemp = A - B;
                if((A & 0x0F) < (B & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < B)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == B)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xB9: /* CMP C */
                btemp = A - C;
                if((A & 0x0F) < (C & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < C)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == C)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xBA: /* CMP D */
                btemp = A - D;
                if((A & 0x0F) < (D & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < D)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == D)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xBB: /* CMP E */
                btemp = A - E;
                if((A & 0x0F) < (E & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < E)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == E)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xBC: /* CMP H */
                btemp = A - H;
                if((A & 0x0F) < (H & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < H)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == H)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xBD: /* CMP L */
                btemp = A - L;
                if((A & 0x0F) < (L & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < L)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == L)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xBE: /* CMP M */
                btemp2 = read_memory((H << 8) | L);
                btemp = A - btemp2;
                if((A & 0x0F) < (btemp2 & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < btemp2)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                *ZERO = 0;
                if(A == btemp2)*ZERO = 1;
                parity(btemp);
                cycle_count -= 4;
                break;

        case 0xC0: /* RNZ */
                if(!(*ZERO))
                {
                        PC = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        cycle_count -= 6;
                }
                cycle_count -= 5;
                break;

        case 0xC1: /* POP BC */
                C = memory[SP];
                B = memory[SP + 1];
                SP += 2;
                cycle_count -= 10;
                break;

        case 0xC2:  /* JNZ 0x???? */
                if(!(*ZERO))PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                else PC +=2;
                cycle_count -= 10;
                break;

        case 0xC3:  /* JMP 0x???? */
                PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                cycle_count -= 10;
                break;

        case 0xC4: /* CNZ 0x???? */
                PC += 2;
                if(!(*ZERO))
                {
                        memory[SP - 1] = PC >> 8;
                        memory[SP - 2] = PC & 0xFF;
                        SP -= 2;
                        PC -= 2;
                        PC = (read_memory(PC + 1) << 8) | read_memory(PC);
                        cycle_count -= 6;
                }
                cycle_count -= 11;
                break;

        case 0xC5: /* PUSH BC */
                memory[SP - 1] = B;
                memory[SP - 2] = C;
                SP -= 2;
                cycle_count -= 11;
                break;

        case 0xC6: /* ADI 0x?? */
                btemp = A;
                wtemp = A;
                wtemp += read_memory(PC);
                A += read_memory(PC);
                PC++;
                if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(wtemp >= 0x100)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xC7: /* RST 0 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0;
                cycle_count -= 11;
                break;

        case 0xC8: /* RZ */
                if(*ZERO)
                {
                        PC = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        cycle_count -= 6;
                }
                cycle_count -= 5;
                break;

        case 0xC9: /* RET */
                PC = memory[SP] | (memory[SP + 1] << 8);
                SP += 2;
                cycle_count -= 10;
                break;

        case 0xCA:  /* JZ 0x???? */
                if(*ZERO)PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                else PC +=2;
                cycle_count -= 10;
                break;

        //case 0xCB: /* DB 0xCB */

        case 0xCC: /* CZ 0x???? */
                PC += 2;
                if(*ZERO)
                {
                        memory[SP - 1] = PC >> 8;
                        memory[SP - 2] = PC & 0xFF;
                        SP -= 2;
                        PC -= 2;
                        PC = (read_memory(PC + 1) << 8) | read_memory(PC);
                        cycle_count -= 6;
                }
                cycle_count -= 11;
                break;

        case 0xCD: /* CALL 0x???? */
                PC += 2;
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC -= 2;
                PC = (read_memory(PC + 1) << 8) | read_memory(PC);
                cycle_count -= 17;
                break;

        case 0xCF: /* RST 1 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x08;
                cycle_count -= 11;
                break;

        case 0xD0: /* RNC */
                if(!(*CARRY))
                {
                        PC = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        cycle_count -= 6;
                }
                cycle_count -= 5;
                break;

        case 0xD1: /* POP DE */
                E = memory[SP];
                D = memory[SP + 1];
                SP += 2;
                cycle_count -= 10;
                break;

        case 0xD2:  /* JNC 0x???? */
                if(!(*CARRY))PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                else PC +=2;
                cycle_count -= 10;
                break;

        case 0xD3: /* OUT 0x?? */
                invaders_out(read_memory(PC),A);
                PC++;
                cycle_count -= 10;
                break;

        case 0xD4: /* CNC 0x???? */
                PC += 2;
                if(!(*CARRY))
                {
                        memory[SP - 1] = PC >> 8;
                        memory[SP - 2] = PC & 0xFF;
                        SP -= 2;
                        PC -= 2;
                        PC = (read_memory(PC + 1) << 8) | read_memory(PC);
                        cycle_count -= 6;
                }
                cycle_count -= 11;
                break;

        case 0xD5: /* PUSH DE */
                memory[SP - 1] = D;
                memory[SP - 2] = E;
                SP -= 2;
                cycle_count -= 11;
                break;

        case 0xD6: /* SUI 0x?? */
                btemp = A;
                A -= read_memory(PC);
                PC++;
                if((btemp & 0x0F) < (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp < A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xD7: /* RST 2 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x10;
                cycle_count -= 11;
                break;

        case 0xD8: /* RC */
                if(*CARRY)
                {
                        PC = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        cycle_count -= 6;
                }
                cycle_count -= 5;
                break;

        //case 0xD9: /* DB 0xD9 */

        case 0xDA:  /* JC 0x???? */
                if(*CARRY)PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                else PC +=2;
                cycle_count -= 10;
                break;

        case 0xDB: /* IN 0x?? */
                A = invaders_in(read_memory(PC));
                PC++;
                cycle_count -= 10;
                break;

        //case 0xDD: /* DB 0xDD */

        case 0xDE: /* SBI 0x?? */
                btemp = A;
                A = A - read_memory(PC) - *CARRY;
                PC += 1;
                if((btemp & 0x0F) < (A & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(btemp < A)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xDF: /* RST 3 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x18;
                cycle_count -= 11;
                break;

        case 0xE1: /* POP HL */
                L = memory[SP];
                H = memory[SP + 1];
                SP += 2;
                cycle_count -= 10;
                break;

        case 0xE3: /* XTHL */
                btemp = memory[SP];
                btemp2  = memory[SP + 1];
                SP += 2;
                memory[SP - 1] = H;
                memory[SP - 2] = L;
                SP -= 2;
                L = btemp;
                H = btemp2;
                cycle_count -= 18;
                break;

        case 0xE5: /* PUSH HL */
                memory[SP - 1] = H;
                memory[SP - 2] = L;
                SP -= 2;
                cycle_count -= 11;
                break;

        case 0xE6: /* ANI 0x?? */
                A &= read_memory(PC);
                PC++;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xE7: /* RST 4 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x20;
                cycle_count -= 11;
                break;

        case 0xE9: /* PCHL */
                PC = (H << 8) | L;
                cycle_count -= 5;
                break;

        case 0xEB: /* XCHG HL,DE */
                btemp = H;
                btemp2 = D;
                H = btemp2;
                D = btemp;
                btemp = L;
                btemp2 = E;
                L = btemp2;
                E = btemp;
                cycle_count -= 4;
                break;

        //case 0xED: /* DB 0xED */

        case 0xEF: /* RST 5 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x28;
                cycle_count -= 11;
                break;

        case 0xF1: /* POP PSW */
                PSW = memory[SP];
                set_flags();
                A = memory[SP + 1];
                SP += 2;
                cycle_count -= 10;
                break;
                       
        case 0xF5: /* PUSH PSW */
                PSW = GET_PSW();
                memory[SP - 1] = A;
                memory[SP - 2] = PSW;
                SP -= 2;
                cycle_count -= 11;
                break;

        case 0xF6: /* ORI 0x?? */
                temp = read_memory(PC);
                PC++;
                A |= temp;
                *CARRY = 0;
                *AUX_CARRY = 0;
                *SIGN = 0;
                if((A & 0x80))*SIGN = 1;
                *ZERO = !(A);
                parity(A);
                cycle_count -= 7;
                break;

        case 0xF7: /* RST 6 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x30;
                cycle_count -= 11;
                break;

        case 0xFA:  /* JM 0x???? */
                if(*SIGN)PC = (read_memory(PC + 1) << 8) | read_memory(PC);     
                else PC +=2;
                cycle_count -= 10;
                break;

        case 0xFB: /* EI */
                INT = 1;
                cycle_count -= 4;
                break;

        //case 0xFD: /* DB 0xFD */

        case 0xFE: /* CPI A,0x?? */
                btemp2 = read_memory(PC); 
                btemp = A - btemp2;
                if((A & 0x0F) < (btemp & 0x0F))*AUX_CARRY = 1;
                else *AUX_CARRY = 0;
                if(A < btemp2)*CARRY = 1;
                else *CARRY = 0;
                *SIGN = 0;
                if((btemp & 0x80))*SIGN = 1;
                if(A == btemp2)*ZERO = 1;
                else *ZERO = 0;
                parity(btemp);
                PC++;
                cycle_count -= 7;
                break;

        case 0xFF: /* RST 7 */
                memory[SP - 1] = PC >> 8;
                memory[SP - 2] = PC & 0xFF;
                SP -= 2;
                PC = 0x38;
                cycle_count -= 11;
                break;

        default:
                dump(cycle_count);
                return -1000;
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
                //        dump();
                }
                //dump(cycle_count);
                old_cmd5 = old_cmd4;
                old_cmd4 = old_cmd3;
                old_cmd3 = old_cmd2;
                old_cmd2 = old_cmd1;
                old_cmd1 = memory[PC];
                cycle_count = decode(memory[PC++], cycle_count);
                //if(cycle_count == -1000)
                //{
                //        dump();
                //}
                //printf("cycle_count = %d\n", cycle_count);
                /*if(cycle_count == 0)
                {
                        return 0;
                }*/

        } while(cycle_count > 0);
        
        //fprintf(debug, "\n");

        if(ERROR == 1)
        {
                //dump();
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