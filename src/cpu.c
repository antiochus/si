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

//WORD wdebug = 0;
//WORD BC = 0;
//uclock_t test_time = 0;

inline BYTE read_memory(WORD offset)
{
        if(SPECIALROM == 0)
        {
                if(offset <= VIDRAM_END)return memory[offset];
                else
                {
                        printf("Leseversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                        if(DEBUG)fprintf(debug,"Leseversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                        exit(1);
                        return 0;
                }
        }
        else
        {
                if(offset <= (VIDRAM_END + SPECIALROM_SIZE))return memory[offset];
                else
                {
                        printf("Leseversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                        if(DEBUG)fprintf(debug,"Leseversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                        exit(1);
                        return 0;
                }
        }
}

inline void write_memory(WORD offset,BYTE data)
{                              
        if(offset >= WORKRAM_BEGIN && offset <= WORKRAM_END)memory[offset] = data;
        else if(offset >= VIDRAM_BEGIN && offset <= VIDRAM_END)
        {
                memory[offset] = data;
                update_buffer(offset, data);
        }
        else
        {
                printf("Schreibversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
                if(DEBUG)fprintf(debug,"Schreibversuch an Offset %4X\tPC = %4X\n",offset,(PC - 1));
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

WORD cpu(WORD cycles)
{
        int cycle_count = cycles;

        do
        {
                //BC++;
                if(!SPECIALROM)
                {
                        if(PC > CODE_END)
                        {
                                return 0;
                                if(DEBUG)fprintf(debug,"PC hat den Gueltigkeitsbereich verlassen\tPC = %4X\n",PC);
                        }
                }
                else
                {
                        if(PC > (VIDRAM_END + SPECIALROM_SIZE))
                        {
                                return 0;
                                if(DEBUG)fprintf(debug,"PC hat den Gueltigkeitsbereich verlassen\tPC = %4X\n",PC);
                        }
                }

                //if(DEBUG == 2 && wdebug)
                if(DEBUG == 2)
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
                switch(memory[PC++])
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
                        btemp = B;
                        B++;
                        if((btemp & 0x0F) > (B & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((B & 0x80))*SIGN = 1;
                        *ZERO = !(B);
                        temp3 = 0;
                        if((B & 0x80))temp3++;
                        if((B & 0x40))temp3++;
                        if((B & 0x20))temp3++;
                        if((B & 0x10))temp3++;
                        if((B & 0x08))temp3++;
                        if((B & 0x04))temp3++;
                        if((B & 0x02))temp3++;
                        if((B & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x05: /* DCR B */
                        btemp = B;
                        B--;
                        if((btemp & 0x0F) < (B & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((B & 0x80))*SIGN = 1;
                        *ZERO = !(B);
                        temp3 = 0;
                        if((B & 0x80))temp3++;
                        if((B & 0x40))temp3++;
                        if((B & 0x20))temp3++;
                        if((B & 0x10))temp3++;
                        if((B & 0x08))temp3++;
                        if((B & 0x04))temp3++;
                        if((B & 0x02))temp3++;
                        if((B & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp = C;
                        C++;
                        if((temp & 0x0F) > (C & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((C & 0x80))*SIGN = 1;
                        *ZERO = !(C);
                        temp3 = 0;
                        if((C & 0x80))temp3++;
                        if((C & 0x40))temp3++;
                        if((C & 0x20))temp3++;
                        if((C & 0x10))temp3++;
                        if((C & 0x08))temp3++;
                        if((C & 0x04))temp3++;
                        if((C & 0x02))temp3++;
                        if((C & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x0D: /* DCR C */
                        temp = C;
                        C--;
                        if((temp & 0x0F) < (C & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((C & 0x80))*SIGN = 1;
                        *ZERO = !(C);
                        temp3 = 0;
                        if((C & 0x80))temp3++;
                        if((C & 0x40))temp3++;
                        if((C & 0x20))temp3++;
                        if((C & 0x10))temp3++;
                        if((C & 0x08))temp3++;
                        if((C & 0x04))temp3++;
                        if((C & 0x02))temp3++;
                        if((C & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        btemp = D;
                        D++;
                        if((btemp & 0x0F) > (D & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((D & 0x80))*SIGN = 1;
                        *ZERO = !(D);
                        temp3 = 0;
                        if((D & 0x80))temp3++;
                        if((D & 0x40))temp3++;
                        if((D & 0x20))temp3++;
                        if((D & 0x10))temp3++;
                        if((D & 0x08))temp3++;
                        if((D & 0x04))temp3++;
                        if((D & 0x02))temp3++;
                        if((D & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x15: /* DCR D */
                        btemp = D;
                        D--;
                        if((btemp & 0x0F) < (D & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((D & 0x80))*SIGN = 1;
                        *ZERO = !(D);
                        temp3 = 0;
                        if((D & 0x80))temp3++;
                        if((D & 0x40))temp3++;
                        if((D & 0x20))temp3++;
                        if((D & 0x10))temp3++;
                        if((D & 0x08))temp3++;
                        if((D & 0x04))temp3++;
                        if((D & 0x02))temp3++;
                        if((D & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp = E;
                        E++;
                        if((temp & 0x0F) > (E & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((E & 0x80))*SIGN = 1;
                        *ZERO = !(E);
                        temp3 = 0;
                        if((E & 0x80))temp3++;
                        if((E & 0x40))temp3++;
                        if((E & 0x20))temp3++;
                        if((E & 0x10))temp3++;
                        if((E & 0x08))temp3++;
                        if((E & 0x04))temp3++;
                        if((E & 0x02))temp3++;
                        if((E & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x1D: /* DCR E */
                        temp = E;
                        E--;
                        if((temp & 0x0F) < (E & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((E & 0x80))*SIGN = 1;
                        *ZERO = !(E);
                        temp3 = 0;
                        if((E & 0x80))temp3++;
                        if((E & 0x40))temp3++;
                        if((E & 0x20))temp3++;
                        if((E & 0x10))temp3++;
                        if((E & 0x08))temp3++;
                        if((E & 0x04))temp3++;
                        if((E & 0x02))temp3++;
                        if((E & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp = H;
                        H++;
                        if((temp & 0x0F) > (H & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((H & 0x80))*SIGN = 1;
                        *ZERO = !(H);
                        temp3 = 0;
                        if((H & 0x80))temp3++;
                        if((H & 0x40))temp3++;
                        if((H & 0x20))temp3++;
                        if((H & 0x10))temp3++;
                        if((H & 0x08))temp3++;
                        if((H & 0x04))temp3++;
                        if((H & 0x02))temp3++;
                        if((H & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x25: /* DCR H */
                        temp = H;
                        H--;
                        if((temp & 0x0F) < (H & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((H & 0x80))*SIGN = 1;
                        *ZERO = !(H);
                        temp3 = 0;
                        if((H & 0x80))temp3++;
                        if((H & 0x40))temp3++;
                        if((H & 0x20))temp3++;
                        if((H & 0x10))temp3++;
                        if((H & 0x08))temp3++;
                        if((H & 0x04))temp3++;
                        if((H & 0x02))temp3++;
                        if((H & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp = L;
                        L++;
                        if((temp & 0x0F) > (L & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((L & 0x80))*SIGN = 1;
                        *ZERO = !(L);
                        temp3 = 0;
                        if((L & 0x80))temp3++;
                        if((L & 0x40))temp3++;
                        if((L & 0x20))temp3++;
                        if((L & 0x10))temp3++;
                        if((L & 0x08))temp3++;
                        if((L & 0x04))temp3++;
                        if((L & 0x02))temp3++;
                        if((L & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x2D: /* DCR L */
                        temp = L;
                        L--;
                        if((temp & 0x0F) < (L & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((L & 0x80))*SIGN = 1;
                        *ZERO = !(L);
                        temp3 = 0;
                        if((L & 0x80))temp3++;
                        if((L & 0x40))temp3++;
                        if((L & 0x20))temp3++;
                        if((L & 0x10))temp3++;
                        if((L & 0x08))temp3++;
                        if((L & 0x04))temp3++;
                        if((L & 0x02))temp3++;
                        if((L & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        wtemp = read_memory(((H << 8) | L));
                        wtemp2 = temp;
                        wtemp++;
                        write_memory(((H << 8) | L),wtemp);
                        if((wtemp2 & 0x0F) > (wtemp & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((wtemp & 0x8000))*SIGN = 1;
                        *ZERO = !(wtemp);
                        temp3 = 0;
                        if((wtemp & 0x8000))temp3++;
                        if((wtemp & 0x4000))temp3++;
                        if((wtemp & 0x2000))temp3++;
                        if((wtemp & 0x1000))temp3++;
                        if((wtemp & 0x0800))temp3++;
                        if((wtemp & 0x0400))temp3++;
                        if((wtemp & 0x0200))temp3++;
                        if((wtemp & 0x0100))temp3++;
                        if((wtemp & 0x0080))temp3++;
                        if((wtemp & 0x0040))temp3++;
                        if((wtemp & 0x0020))temp3++;
                        if((wtemp & 0x0010))temp3++;
                        if((wtemp & 0x0008))temp3++;
                        if((wtemp & 0x0004))temp3++;
                        if((wtemp & 0x0002))temp3++;
                        if((wtemp & 0x0001))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 10;
                        break;

                case 0x35: /* DCR M */
                        wtemp = read_memory(((H << 8) | L));
                        wtemp2 = temp;
                        wtemp--;
                        write_memory(((H << 8) | L),wtemp);
                        if((wtemp2 & 0x0F) < (wtemp & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((wtemp & 0x8000))*SIGN = 1;
                        *ZERO = !(wtemp);
                        temp3 = 0;
                        if((wtemp & 0x8000))temp3++;
                        if((wtemp & 0x4000))temp3++;
                        if((wtemp & 0x2000))temp3++;
                        if((wtemp & 0x1000))temp3++;
                        if((wtemp & 0x0800))temp3++;
                        if((wtemp & 0x0400))temp3++;
                        if((wtemp & 0x0200))temp3++;
                        if((wtemp & 0x0100))temp3++;
                        if((wtemp & 0x0080))temp3++;
                        if((wtemp & 0x0040))temp3++;
                        if((wtemp & 0x0020))temp3++;
                        if((wtemp & 0x0010))temp3++;
                        if((wtemp & 0x0008))temp3++;
                        if((wtemp & 0x0004))temp3++;
                        if((wtemp & 0x0002))temp3++;
                        if((wtemp & 0x0001))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp = A;
                        A++;
                        if((temp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 5;
                        break;

                case 0x3D: /* DCR A */
                        temp = A;
                        A--;
                        if((temp & 0x0F) < (A & 0x0F))*AUX_CARRY = 1;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 7;
                        break;

                case 0xA0: /* ANA B */
                        A &= B;
                        *AUX_CARRY = 0;
                        *CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xA6: /* ANA M */
                        A &= read_memory((H << 8) | L);
                        *AUX_CARRY = 0;
                        *CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 7;
                        break;

                case 0xA7: /* ANA A */
                        A &= A;
                        *AUX_CARRY = 0;
                        *CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xA8: /* XRA B */
                        A ^= B;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xAE: /* XRA M */
                        A ^= read_memory((H << 8) | L);
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 7;
                        break;

                case 0xAF: /* XRA A */
                        A ^= A;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xB0: /* ORA B */
                        A |= B;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xB4: /* ORA H */
                        A |= H;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xB5: /* ORA L */
                        A |= L;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 4;
                        break;

                case 0xB6: /* ORA M */
                        A |= read_memory((H << 8) | L);
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
                        cycle_count -= 7;
                        break;

                case 0xB7: /* ORA A */
                        A |= A;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        if((btemp & 0x0F) > (A & 0x0F))*AUX_CARRY = 1;
                        else *AUX_CARRY = 0;
                        if(A > btemp)*CARRY = 1;
                        else *CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        A ^= temp;
                        *CARRY = 0;
                        *AUX_CARRY = 0;
                        *SIGN = 0;
                        if((A & 0x80))*SIGN = 1;
                        *ZERO = !(A);
                        temp3 = 0;
                        if((A & 0x80))temp3++;
                        if((A & 0x40))temp3++;
                        if((A & 0x20))temp3++;
                        if((A & 0x10))temp3++;
                        if((A & 0x08))temp3++;
                        if((A & 0x04))temp3++;
                        if((A & 0x02))temp3++;
                        if((A & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        temp3 = 0;
                        if((btemp & 0x80))temp3++;
                        if((btemp & 0x40))temp3++;
                        if((btemp & 0x20))temp3++;
                        if((btemp & 0x10))temp3++;
                        if((btemp & 0x08))temp3++;
                        if((btemp & 0x04))temp3++;
                        if((btemp & 0x02))temp3++;
                        if((btemp & 0x01))temp3++;
                        *PARITY = !(temp3 % 2);
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
                        if(DEBUG)
                        {
                        fprintf(debug,"\nFehler bei Offset %4X\n\n",(PC - 1));
                        fprintf(debug,"Opcode : %2X\n\n",memory[PC - 1]);
                        fprintf(debug,"A = %2X\tB = %2X\tC = %2X\tSP = %4X\n",A,B,C,SP);
                        fprintf(debug,"D = %2X\tE = %2X\tH = %2X\tL  = %4X\n\n",D,E,H,L);
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
                        //fprintf(debug,"Stack = %2X %2X %2X %2X %2X %2X %2X %2X %2X\n\n",(temp >> 8),(temp & 0xFF),(temp2 >> 8),(temp2 & 0xFF),(temp3 >> 8),(temp3 & 0xFF),btemp,btemp2,btemp3);
                        fprintf(debug,"PARITY = %2X\tCARRY = %2X\tAUX_CARRY = %2X\n",*PARITY,*CARRY,*AUX_CARRY);
                        fprintf(debug,"ZERO   = %2X\tSIGN  = %2X\tPSW       = %2X\n",*ZERO,*SIGN,PSW);
                        fprintf(debug,"X1     = %2X\tX2    = %2X\tX3        = %2X\n\n",*FLAG_X1,*FLAG_X2,*FLAG_X3);
                        //fprintf(debug,"cycle_count = %d\n",cycle_count);
                        }
                        return 0;
                }

        } while(cycle_count > 0);

        return (cycles - cycle_count);
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
        test_zero = 0;
        INT = 1;
        set_flags();
}
