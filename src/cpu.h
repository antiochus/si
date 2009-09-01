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

extern WORD _DEBUG;

extern WORD CODE_BEGIN;
extern WORD CODE_END;
extern WORD WORKRAM_BEGIN;
extern WORD WORKRAM_END;
extern WORD VIDRAM_BEGIN;
extern WORD VIDRAM_END;
extern WORD SPECIALRAM;
extern WORD SPECIALROM;
extern WORD SPECIALROM_SIZE;

extern WORD* CARRY;
extern WORD* FLAG_X1;
extern WORD* PARITY;
extern WORD* FLAG_X2;
extern WORD* AUX_CARRY;
extern WORD* FLAG_X3;
extern WORD* ZERO;
extern WORD* SIGN;
extern WORD INT;

extern WORD SP;
extern WORD PC;
extern BYTE PSW,A,B,C,D,E,H,L;
extern BYTE ERROR;

WORD cpu(WORD cycles);
void interrupt(void);
void NMI(void);
void reset(void);
