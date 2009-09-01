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

inline void opcode_in(void)
{
        A = invaders_in(read_memory(PC));
        PC++;
}

inline void opcode_out(void)
{
        invaders_out(read_memory(PC),A);
        PC++;
}

inline void opcode_ei(void)
{
        INT = 1;
}

inline void opcode_di(void)
{
        INT = 0;
}

inline void opcode_daa(void)
{
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
}
  
inline void opcode_cma(void)
{  
        A = ~A;
}

inline void opcode_stc(void)
{  
        *CARRY = 1;
}

inline void opcode_cmc(void)
{  
        *CARRY = ~*CARRY;
}
