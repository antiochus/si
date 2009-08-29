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

#define BYTE unsigned char
#define WORD unsigned short int
#define DWORD unsigned long int

BYTE SOUND = 1;
BYTE halt = 0;

BYTE* memory = NULL;

WORD shiftdata_amount = 0;
WORD shiftdata_1 = 0;
WORD shiftdata_2 = 0;

FILE* debug = NULL;
FILE* debug2 = NULL;

SAMPLE* sample3_1 = NULL;
SAMPLE* sample3_2 = NULL;
SAMPLE* sample3_3 = NULL;
SAMPLE* sample3_4 = NULL;

SAMPLE* sample5_1 = NULL;
SAMPLE* sample5_2 = NULL;
SAMPLE* sample5_3 = NULL;
SAMPLE* sample5_4 = NULL;
SAMPLE* sample5_5 = NULL;

BITMAP* double_buffer;
