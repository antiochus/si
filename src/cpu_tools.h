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

#include <stdlib.h>

void dump(int cycle_count)
{

DWORD temp, temp2, temp3;
WORD wtemp; //, wtemp2, wtemp3;
BYTE btemp, btemp2, btemp3;
//BYTE test_zero = 0;

                        wtemp = SP;
                        printf("\n%4X",(PC));
                        printf("\t%2X %2X %2X %2X %2X %2X\t\t",memory[PC], old_cmd1, old_cmd2, old_cmd3, old_cmd4, old_cmd5);
                        printf("%2X%2X%2X%4X",A,B,C,SP);
                        printf("\t%2X%2X%2X%4X",D,E,H,L);
                        temp = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        temp2 = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        temp3 = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        btemp = memory[SP];
                        SP += 1;
                        btemp2 = memory[SP];
                        SP += 1;
                        btemp3 = memory[SP];
                        SP += 1;
                        printf("\tStack%2X%2X%2X%2X%2X%2X%2X%2X%2X",(BYTE)(temp >> 8),(BYTE)(temp & 0xFF),(BYTE)(temp2 >> 8),(BYTE)(temp2 & 0xFF),(BYTE)(temp3 >> 8),(BYTE)(temp3 & 0xFF),btemp,btemp2,btemp3);
                        printf("\t%2X%2X%2X",*PARITY,*CARRY,*AUX_CARRY);
                        printf("%2X%2X%2X",*ZERO,*SIGN,PSW);
                        printf("%2X%2X%2X",*FLAG_X1,*FLAG_X2,*FLAG_X3);
                        printf("%d", cycle_count);
                        SP = wtemp;
/*
                        wtemp = SP;
                        printf("\nOffset %4X\n\n",(PC));
                        printf("Opcodes : %2X %2X %2X %2X %2X %2X\n\n",memory[PC], old_cmd1, old_cmd2, old_cmd3, old_cmd4, old_cmd5);
                        printf("A = %2X\tB = %2X\tC = %2X\tSP = %4X\n",A,B,C,SP);
                        printf("D = %2X\tE = %2X\tH = %2X\tL  = %4X\n\n",D,E,H,L);
                        temp = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        temp2 = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        temp3 = memory[SP] | (memory[SP + 1] << 8);
                        SP += 2;
                        btemp = memory[SP];
                        SP += 1;
                        btemp2 = memory[SP];
                        SP += 1;
                        btemp3 = memory[SP];
                        SP += 1;
                        printf("Stack = %2X %2X %2X %2X %2X %2X %2X %2X %2X\n\n",(BYTE)(temp >> 8),(BYTE)(temp & 0xFF),(BYTE)(temp2 >> 8),(BYTE)(temp2 & 0xFF),(BYTE)(temp3 >> 8),(BYTE)(temp3 & 0xFF),btemp,btemp2,btemp3);
                        printf("PARITY = %2X\tCARRY = %2X\tAUX_CARRY = %2X\n",*PARITY,*CARRY,*AUX_CARRY);
                        printf("ZERO   = %2X\tSIGN  = %2X\tPSW       = %2X\n",*ZERO,*SIGN,PSW);
                        printf("X1     = %2X\tX2    = %2X\tX3        = %2X\n\n",*FLAG_X1,*FLAG_X2,*FLAG_X3);
                        printf("cycle_count = %d\n", cycle_count);
                        SP = wtemp;
*/
}

inline BYTE read_memory(WORD offset)
{
        if(SPECIALROM == 0)
        {
                if(offset <= VIDRAM_END)return memory[offset];
                else
                {
                        printf("Leseversuch an Offset %4X\n",offset);
                        //dump();
                        exit(1);
                }
        }
        else
        {
                if(offset <= (VIDRAM_END + SPECIALROM_SIZE))return memory[offset];
                else
                {
                        printf("Leseversuch an Offset %4X\n",offset);
                        //dump();
                        exit(1);
                }
        }
}

inline void write_memory(WORD offset,BYTE data)
{                              
        if(offset == 0x1A11)
        {
                printf("Schreibversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                //dump();
        }

        if(offset >= WORKRAM_BEGIN && offset <= WORKRAM_END)memory[offset] = data;
        else if(offset >= VIDRAM_BEGIN && offset <= VIDRAM_END)
        {
                memory[offset] = data;
                update_buffer(offset, data);
        }
        else
        {
                printf("Schreibversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                //dump();
                exit(1);
        }
}

inline void set_flags(void)
{
        if(PSW & 0x01)  *CARRY     = 1;  else *CARRY     = 0;
        if(PSW & 0x02)  *FLAG_X1   = 1;  else *FLAG_X1   = 0;
        if(PSW & 0x04)  *PARITY    = 1;  else *PARITY    = 0;
        if(PSW & 0x08)  *FLAG_X2   = 1;  else *FLAG_X2   = 0;
        if(PSW & 0x10)  *AUX_CARRY = 1;  else *AUX_CARRY = 0;
        if(PSW & 0x20)  *FLAG_X3   = 1;  else *FLAG_X3   = 0;
        if(PSW & 0x40)  *ZERO      = 1;  else *ZERO      = 0;
        if(PSW & 0x80)  *SIGN      = 1;  else *SIGN      = 0;

}

inline void parity(BYTE value)
{
        BYTE temp3 = 0;
        if((value & 0x80))temp3++;
        if((value & 0x40))temp3++;
        if((value & 0x20))temp3++;
        if((value & 0x10))temp3++;
        if((value & 0x08))temp3++;
        if((value & 0x04))temp3++;
        if((value & 0x02))temp3++;
        if((value & 0x01))temp3++;
        *PARITY = !(temp3 % 2);
}

inline void wparity(WORD value)
{
        BYTE temp3 = 0;
        if((value & 0x8000))temp3++;
        if((value & 0x4000))temp3++;
        if((value & 0x2000))temp3++;
        if((value & 0x1000))temp3++;
        if((value & 0x0800))temp3++;
        if((value & 0x0400))temp3++;
        if((value & 0x0200))temp3++;
        if((value & 0x0100))temp3++;
        if((value & 0x0080))temp3++;
        if((value & 0x0040))temp3++;
        if((value & 0x0020))temp3++;
        if((value & 0x0010))temp3++;
        if((value & 0x0008))temp3++;
        if((value & 0x0004))temp3++;
        if((value & 0x0002))temp3++;
        if((value & 0x0001))temp3++;
        *PARITY = !(temp3 % 2);
}