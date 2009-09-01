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

inline void opcode_jmp(void)
{
        PC = (read_memory(PC + 1) << 8) | read_memory(PC);
}

inline void opcode_call(void)
{
        WORD newpc = (read_memory(PC + 1) << 8) | read_memory(PC);
        PC += 2;
        SP -= 2;
        memory[SP] = PC & 0xFF;
        memory[SP + 1] = PC >> 8;
        PC = newpc;
}

inline void opcode_ret(void)
{
        PC = memory[SP] | (memory[SP + 1] << 8);
        SP += 2;
}

inline void opcode_pchl(void)
{
        PC = (H << 8) | L;
}
