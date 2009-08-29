/*
    This file is part of SI - A primitive, but simple Space Invaders emulator.
	Copyright 1998 - 2009 Jens Mühlenhoff <j.muehlenhoff@gmx.d>

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

//#define PUSH(b)  memory[SP - 1] = (b); SP--;;;  
//#define PULL()   memory[SP++];;;;          
//#define PUSH(b)  memory[SP] = (b); SP--  
//#define PULL()   memory[++SP]         

#define GET_PSW()  ((*CARRY ? 0x01 : 0) | (*FLAG_X1 ? 0x02 : 0) |\
                   (*PARITY ? 0x04 : 0) | (*FLAG_X2 ? 0x08 : 0) |\
                   (*AUX_CARRY ? 0x10 : 0) | (*FLAG_X3 ? 0x20 : 0) |\
                   (*ZERO ? 0x40 : 0) | (*SIGN ? 0x80 : 0))

WORD DEBUG = 0;

WORD CODE_BEGIN = 0;
WORD CODE_END = 0;
WORD WORKRAM_BEGIN = 0;
WORD WORKRAM_END = 0;
WORD VIDRAM_BEGIN = 0;
WORD VIDRAM_END = 0;
WORD SPECIALRAM = 0;
WORD SPECIALROM = 0;
WORD SPECIALROM_SIZE = 0;

DWORD temp, temp2, temp3;
WORD wtemp, wtemp2, wtemp3;
BYTE btemp, btemp2, btemp3;
BYTE test_zero = 0;

WORD PC = 0x0001; /* Program Counter */
WORD INTPC = 0;

/*
   Register : SP = Stack Pointer, PSW = Processor Status Word,
   A = Accumulator, B, C, D, E, H, L
*/

WORD SP = 0x23FF;
BYTE PSW,A,B,C,D,E,H,L;

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

