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

inline void opcode_push(BYTE* upper, BYTE* lower)
{
        SP -= 2;
        memory[SP] = *lower;
        memory[SP + 1] = *upper;
}

inline void opcode_pop(BYTE* upper, BYTE* lower)
{
        *lower = memory[SP];
        *upper = memory[SP + 1];
        SP += 2;
}

inline void opcode_xthl(void)
{
        BYTE btemp = memory[SP];
        BYTE btemp2  = memory[SP + 1];
        memory[SP] = L;
        memory[SP + 1] = H;
        L = btemp;
        H = btemp2;
}