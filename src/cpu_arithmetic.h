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

inline void flag_check_simple(BYTE value)
{
        *CARRY = 0;
        *AUX_CARRY = 0;
        *SIGN = 0;
        if(value & 0x80)
        {
                *SIGN = 1;
        }
        else
        {
                *SIGN = 0;
        }
        parity(value);
}

inline void flag_check_add_no_carry(BYTE before, BYTE after)
{
        if((before & 0x08) > (after & 0x08))
        {
                *AUX_CARRY = 1;
        }
        else 
        {
                *AUX_CARRY = 0;
        }
        if(after & 0x80)
        {
                *SIGN = 1;
        }
        else
        {
                *SIGN = 0;
        }
        if(!after)
        {
                *ZERO = 1;
        }
        else
        {
                *ZERO = 0;
        }
        parity(after);
}

inline void flag_check_add(BYTE before, BYTE after)
{
        flag_check_add_no_carry(before, after);
        if(before > after)
        {
                *CARRY = 1;
        }
        else
        {
                *CARRY = 0;
        }
}

inline void flag_check_sub_no_carry(BYTE before, BYTE after)
{
        if((before & 0x08) < (after & 0x08))
        {
                *AUX_CARRY = 1;
        }
        else 
        {
                *AUX_CARRY = 0;
        }
        if(after & 0x80)
        {
                *SIGN = 1;
        }
        else
        {
                *SIGN = 0;
        }
        if(!after)
        {
                *ZERO = 1;
        }
        else
        {
                *ZERO = 0;
        }
        parity(after);
}

inline void flag_check_sub(BYTE before, BYTE after)
{
        flag_check_sub_no_carry(before, after);
        if(before < after)
        {
                *CARRY = 1;
        }
        else
        {
                *CARRY = 0;
        }
}

inline void opcode_inr(BYTE *target)
{
        BYTE btemp;
        BYTE btemp2;
        if(!target)
        {
                target = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = *target;
        *target = *target + 1;
        flag_check_add(btemp, *target);
        if(target == &btemp2)
        {
                write_memory((H << 8) | L, btemp2);
        }
}

inline void opcode_dcr(BYTE *target)
{
        BYTE btemp;
        BYTE btemp2;
        if(!target)
        {
                target = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = *target;
        *target = *target - 1;
        flag_check_sub(btemp, *target);
        if(target == &btemp2)
        {
                write_memory((H << 8) | L, btemp2);
        }
}

inline void opcode_inx(BYTE* upper, BYTE* lower)
{
        WORD temp = ((*upper << 8) | *lower);
        temp++;
        *upper = temp >> 8;
        *lower = temp & 0xFF;
}

inline void opcode_dcx(BYTE* upper, BYTE* lower)
{
        WORD temp = ((*upper << 8) | *lower);
        temp--;
        *upper = temp >> 8;
        *lower = temp & 0xFF;
}

inline void opcode_adi(void)
{
        BYTE btemp;
        btemp = A;
        A += read_memory(PC);
        PC++;
        flag_check_add(btemp, A);
}

inline void opcode_aci(void)
{
        BYTE btemp;
        btemp = A;
        A = A + read_memory(PC) + *CARRY;
        PC++;
        flag_check_add(btemp, A);
}

inline void opcode_sui(void)
{
        BYTE btemp;
        btemp = A;
        A -= read_memory(PC);
        PC++;
        flag_check_sub(btemp, A);
}

inline void opcode_sbi(void)
{
        BYTE btemp;
        btemp = A;
        A = A - read_memory(PC) - *CARRY;
        PC++;
        flag_check_sub(btemp, A);
}

inline void opcode_ani(void)
{
        A &= read_memory(PC);
        PC++;
        flag_check_simple(A);
}

inline void opcode_xri(void)
{
        A ^= read_memory(PC);
        PC++;
        flag_check_simple(A);
}

inline void opcode_ori(void)
{
        A |= read_memory(PC);
        PC++;
        flag_check_simple(A);
}

inline void opcode_cpi(void)
{
        BYTE btemp = A - read_memory(PC);
        PC++;
        flag_check_sub(A, btemp);
}

inline void opcode_dad(BYTE *upper, BYTE *lower)
{
        WORD temp = (*upper << 8) | *lower;
        WORD temp2 = (H << 8) | L;
        WORD temp3 = temp2;
        temp2 += temp;
        H = temp2 >> 8;
        L = temp2 & 0xFF;
        if(temp2 < temp3)
        {
                *CARRY = 1;
        }
        else
        {
                *CARRY = 0;
        }
}

inline void opcode_add(BYTE *lower)
{
        BYTE btemp;
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = A;
        A += *lower;
        flag_check_add(btemp, A);
}

inline void opcode_adc(BYTE *lower)
{
        BYTE btemp;
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = A;
        A = A + *lower + *CARRY;
        flag_check_add(btemp, A);
}

inline void opcode_sub(BYTE *lower)
{
        BYTE btemp;
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = A;
        A = A - *lower;
        flag_check_sub(btemp, A);
}

inline void opcode_sbb(BYTE *lower)
{
        BYTE btemp;
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = A;
        A = A - *lower - *CARRY;
        flag_check_sub(btemp, A);
}

inline void opcode_ana(BYTE *lower)
{
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        A &= *lower;
        flag_check_simple(A);
}

inline void opcode_xra(BYTE *lower)
{
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        A ^= *lower;
        flag_check_simple(A);
}

inline void opcode_ora(BYTE *lower)
{
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        A  |= *lower;
        flag_check_simple(A);
}

inline void opcode_cmp(BYTE *lower)
{
        BYTE btemp;
        BYTE btemp2;
        if(!lower)
        {
                lower = &btemp2;
                btemp2 = read_memory((H << 8) | L);
        }
        btemp = A - *lower;
        flag_check_sub(A, btemp);
}

inline void opcode_rar(void)
{
        if((A & 0x01) == 0x01)
        {
                A = (A >> 1) | 0x80;
                *CARRY = 1;
        }
        else
        {
                A = A >> 1;
                *CARRY = 0;
        }
}

inline void opcode_rrc(void)
{
        int old_carry = *CARRY;
        *CARRY = A & 0x01;
        if(old_carry)
        {
                A = (A >> 1) | 0x80;
        }
        else
        {
                A = A >> 1;
        }
}

inline void opcode_ral(void)
{
        if((A & 0x80) == 0x80)
        {
                A = (A << 1) | 0x01;
                *CARRY = 1;
        }
        else
        {
                A = A >> 1;
                *CARRY = 0;
        }
}

inline void opcode_rlc(void)
{
        int old_carry = *CARRY;
        if((A & 0x80) == 0x80)
        {
                *CARRY = 1;
        }
        else
        {
                *CARRY = 0;
        }
        if(old_carry)
        {
                A = (A << 1) | 0x01;
        }
        else
        {
                A = A << 1;
        }
}

