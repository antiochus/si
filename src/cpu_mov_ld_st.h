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

inline void opcode_mov(BYTE *dest, BYTE *src)
{
        BYTE btemp;
        if(!src)
        {
                src = &btemp;
                btemp = read_memory((H << 8) | L);
        }
        if(!dest)
        {
                write_memory((H << 8) | L, *src);
        }
        else
        {
                *dest = *src;
        }
}

inline void opcode_lxi(BYTE *upper, BYTE *lower)
{
        *upper = read_memory((PC + 1));
        *lower = read_memory(PC);
        PC += 2;
}

inline void opcode_mvi(BYTE* dest)
{
        if(!dest)
        {
                // write to memory
                write_memory((H << 8) | L, read_memory(PC));
        }
        else
        {
                // write to register
                *dest = read_memory(PC);
        }
        PC++;
}

inline void opcode_lda(void)
{
        A = read_memory((read_memory(PC + 1) << 8) | read_memory(PC));
        PC += 2;
}

inline void opcode_sta(void)
{
        write_memory((read_memory(PC + 1) << 8) | read_memory(PC), A);
        PC += 2;
}

inline void opcode_ldax(BYTE* upper, BYTE* lower)
{
        A = read_memory(((*upper << 8) | *lower));
}

inline void opcode_stax(BYTE* upper, BYTE* lower)
{
        write_memory((*upper << 8) | *lower, A);
}

inline void opcode_lhld(void)
{
        WORD temp = (read_memory(PC + 1) << 8) | read_memory(PC);
        H = read_memory(temp + 1);
        L = read_memory(temp);
        PC += 2;
}

inline void opcode_shld(void)
{
        WORD temp = (read_memory(PC + 1) << 8) | read_memory(PC);
        write_memory((temp + 1), H);
        write_memory(temp, L);
        PC += 2;
}

inline void opcode_xchg(void)
{
        WORD btemp = H;
        WORD btemp2 = D;
        H = btemp2;
        D = btemp;
        btemp = L;
        btemp2 = E;
        L = btemp2;
        E = btemp;
}