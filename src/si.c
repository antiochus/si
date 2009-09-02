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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <allegro.h>

#include "si_types.h"
#include "cpu.h"

BYTE SOUND = 1;
BYTE halt = 0;

BYTE* memory = NULL;

WORD shiftdata_amount = 0;
WORD shiftdata_1 = 0;
WORD shiftdata_2 = 0;

FILE* debug = NULL;
FILE* debug2 = NULL;

SAMPLE* sample_00 = NULL;
SAMPLE* sample_01 = NULL;
SAMPLE* sample_02 = NULL;
SAMPLE* sample_03 = NULL;
SAMPLE* sample_04 = NULL;
SAMPLE* sample_05 = NULL;
SAMPLE* sample_06 = NULL;
SAMPLE* sample_07 = NULL;
SAMPLE* sample_08 = NULL;
SAMPLE* sample_09 = NULL;

BITMAP* double_buffer;

void set_memlocs(void)
{
        CODE_BEGIN = 0x0000;
        CODE_END = 0x1FFF;
        WORKRAM_BEGIN = 0x2000;
        WORKRAM_END = 0x23FF;
        VIDRAM_BEGIN = 0x2400;
        VIDRAM_END = 0x3FFF;
        SPECIALRAM = 0;
        SPECIALROM = 0;
        SPECIALROM_SIZE = 0x0800;
}

void syntax(void)
{
        printf("Syntax: siemu [SPIELNAME]\n\n");
        printf("Die unterstuetzten Spiele sind:\n\n");
        printf("SPIELNAME\t\tORIGINALNAME\t\t\tROM-VERZEICHNIS\n");
        printf("invaders\t\tSpace Invaders\t\t\troms\\invaders\n");
        printf("invadpt2\t\tSpace Invaders Part II\t\troms\\invadpt2\n");
        printf("spaceatt\t\tSpace Attack 2\t\t\troms\\spaceatt\n");
        printf("earthinv\t\tSuper Earth Invasion\t\troms\\earthinv\n");
}

void message(void)
{
        printf("                          SPACE INVADERS (C) 1979 MIDWAY\n\n");
        printf("                                     ÛÛÛÛÛÛÛÛ\n");
        printf("                               ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ\n");
        printf("                               ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ\n");
        printf("                             ÛÛÛÛÛÛ    ÛÛÛÛ    ÛÛÛÛÛÛ\n");
        printf("                             ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ\n");
        printf("                                   ÛÛÛÛ    ÛÛÛÛ      \n");
        printf("                                 ÛÛÛÛ  ÛÛÛÛ  ÛÛÛÛ    \n");
        printf("                             ÛÛÛÛ                ÛÛÛÛ\n\n");
}

WORD load_rom(char* filename, short offset, short size)
{
        FILE* fp = NULL;
         
        fp = fopen(filename, "rb");

        if(!fp)
        {
                printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);
                return 0;
        }

        fread(memory + offset, size, 1, fp);

        fclose(fp);
        return 1;
}

WORD load_samples(char* filename, SAMPLE** sample)
{
        *sample = load_sample(filename);
        if(!*sample)
        {
                printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);
                return 0;
        }
        else
        {
                return 1;
        }
}

WORD load_roms(WORD game)
{
        int i;
        memory = (BYTE*) malloc(65535);
        if(memory == NULL)
        {
                printf("Eine Fehler ist beim allokieren des virtuellen Speichers aufgetreten\n");
                printf("Ist nicht genuegend Speicher im RAM frei ?\n"); 
                return 0;
        }
        for(i = 0; i < 65536; i++)
        {
                memory[i] = 0xFF;
        }

        switch(game)
        {
                case 1:
                {
                        if(!load_rom("roms/invaders/invaders.h", 0x0000, 2048))return 0;
                        if(!load_rom("roms/invaders/invaders.g", 0x0800, 2048))return 0;
                        if(!load_rom("roms/invaders/invaders.f", 0x1000, 2048))return 0;
                        if(!load_rom("roms/invaders/invaders.e", 0x1800, 2048))return 0;
                        break;
                }
                case 2:
                {
                        if(!load_rom("roms\\spaceatt\\spaceatt.h", 0x0000, 2048))return 0;
                        if(!load_rom("roms\\spaceatt\\spaceatt.g", 0x0800, 2048))return 0;
                        if(!load_rom("roms\\spaceatt\\spaceatt.f", 0x1000, 2048))return 0;
                        if(!load_rom("roms\\spaceatt\\spaceatt.e", 0x1800, 2048))return 0;
                        break;
                }
                case 3:
                {
                        if(!load_rom("roms\\earthinv\\earthinv.h", 0x0000, 2048))return 0;
                        if(!load_rom("roms\\earthinv\\earthinv.g", 0x0800, 2048))return 0;
                        if(!load_rom("roms\\earthinv\\earthinv.f", 0x1000, 2048))return 0;
                        if(!load_rom("roms\\earthinv\\earthinv.e", 0x1800, 2048))return 0;
                        break;
                }
                case 4:
                {
                        if(!load_rom("roms\\invadpt2\\pv.01", 0x0000, 2048))return 0;
                        if(!load_rom("roms\\invadpt2\\pv.02", 0x0800, 2048))return 0;
                        if(!load_rom("roms\\invadpt2\\pv.03", 0x1000, 2048))return 0;
                        if(!load_rom("roms\\invadpt2\\pv.04", 0x1800, 2048))return 0;
                        if(!load_rom("roms\\invadpt2\\pv.05", 0x4000, 2048))return 0;
                        break;
                }
                default:
                {
                        printf("Der interne Fehler 10 ist aufgetreten\n");
                        return 0;
                }
        }
        return 1;
}

WORD load_sounds(WORD game)
{                                                       
        if(!load_samples("samples/0.wav", &sample_00))return 0; // Mystery ship
        if(!load_samples("samples/1.wav", &sample_01))return 0; // Shot
        if(!load_samples("samples/2.wav", &sample_02))return 0; // Base hit
        if(!load_samples("samples/3.wav", &sample_03))return 0; // Invader hit
        if(!load_samples("samples/4.wav", &sample_04))return 0; // Walk 1
        if(!load_samples("samples/5.wav", &sample_05))return 0; // Walk 2
        if(!load_samples("samples/6.wav", &sample_06))return 0; // Walk 3
        if(!load_samples("samples/7.wav", &sample_07))return 0; // Walk 4
        if(!load_samples("samples/8.wav", &sample_08))return 0; // Mystery ship hit
        if(!load_samples("samples/9.wav", &sample_09))return 0; // Extra life ?
        return 1;
}

int last_sound_03 = 0x00;
int last_sound_05 = 0x00;

inline void try_sound(WORD op, BYTE mask, SAMPLE* sample, BYTE loop)
{
        if((!(last_sound_03 & mask)) && (op & mask))
        {
                  play_sample(sample, 255, 128, 1000, loop);
        }
}

inline void invaders_sound(WORD channel, WORD op)
{
        if(SOUND)
        {
                if(channel == 3)
                {
                        try_sound(op, 0x01, sample_00, 1); // Mystery ship
                        if(!(op & 0x01))stop_sample(sample_00);
                        try_sound(op, 0x02, sample_01, 0); // Shot
                        try_sound(op, 0x04, sample_02, 0); // Base Hit
                        try_sound(op, 0x08, sample_03, 0); // Invader Hit
                        try_sound(op, 0x10, sample_09, 0); // Extra life
                        if(last_sound_03 != op)
                        {
                                //printf("%2X", op);
                                //fflush(stdout);
                                last_sound_03 = op;               
                        }
                        
                }
                else 
                if(channel == 5)
                {
                        if(op & 0x01)play_sample(sample_07, 255, 128, 1000, FALSE); // Walk 4
                        if(op & 0x02)play_sample(sample_04, 255, 128, 1000, FALSE); // Walk 1
                        if(op & 0x04)play_sample(sample_05, 255, 128, 1000, FALSE); // Walk 2
                        if(op & 0x08)play_sample(sample_06, 255, 128, 1000, FALSE); // Walk 3
                        if((!(last_sound_05 & 0x10)) && (op & 0x10))play_sample(sample_08, 255, 128, 1000, FALSE); // Mystery Ship hit
                        if(last_sound_05 != op)
                        {
                                //printf("%2X", op);
                                //fflush(stdout);
                                last_sound_05 = op;               
                        }
                }
        }
}

inline void snapshot()
{
        PALETTE pal;
        get_palette(pal);
        save_bmp("si.bmp", double_buffer, pal);
}

void update_buffer(WORD offset, BYTE data)
{
        int b,c,x,y,ty,tc;

        offset = offset - VIDRAM_BEGIN;
        x = offset >> 5;
        y = offset & 0x1F;
        if(y < 8)
        {
                /* 
                    The bottom 7 rows are colored in green. This was done using a green
                    color layer that was glued on top of the CRT
                */
                c = 2;
        }
        else
        if(y > 23 && y < 28)
        {
                /*
                    The same goes for the top rows from 24 to 27 where the red "mystery" ship flys
                */
                c = 4;
        }
        else
        {
                /*
                    Everything else is not obscured by a color layer so appears in white
                */
                c = 15;
        }
        y = y << 3;
        /*
            1 byte of data packs 8 vertical color codes = 8 pixel colors in black & white
            1 color per bit (0 = black, 1 = white)
        */
        for (b = 0; b < 8; b++)
        {
                /*
                    Space Invaders coordinates go from bottom to top,
                    but Allegro coordinates go from top to bottom so flipping the image is necessary
                */
                ty = 248 - (y + b);
                /*
                    Bit on? Use color, else use black
                */ 
                tc = (data & 0x01) ? c : 0;
                putpixel(double_buffer, x, ty, tc);
                /*
                    Next bit
                */
                data = data >> 1;
        }
}

inline void update_screen()
{
        /*int i;
        for(i = VIDRAM_BEGIN; i <= VIDRAM_END; i++)
        {
                update_buffer(i, memory[i]);
        }*/
        blit(double_buffer, screen, 0, 0, 0, 0, 224, 248);
}

BYTE invaders_shift(BYTE port, BYTE op)
{
        if(port == 2)
        {
                shiftdata_amount = op & 0x07;
                return 0;
        }
        if(port == 3)
        {
                return ((((shiftdata_1 << 8) | shiftdata_2) << shiftdata_amount) >> 8) & 0xFF;
        }
        if(port == 4)
        {
                shiftdata_2 = shiftdata_1;
                shiftdata_1 = op;
                return 0;
        }
        return 0;
}
inline WORD port_read(WORD port)
{
        WORD ret = 0;
        if(port == 0)ret = 0xF4;
        if(port == 1)
        {
                ret = 0x81;
                if(key[KEY_5])ret -= 0x01;
                if(key[KEY_2])ret += 0x02;
                if(key[KEY_1])ret += 0x04;
                if(key[KEY_LCONTROL])ret += 0x10;
                if(key[KEY_LEFT])ret += 0x20;
                if(key[KEY_RIGHT])ret += 0x40;
        }
        if(port == 2)
        {
                ret = 0x00;
                if(key[KEY_T])ret += 0x04;
                if(key[KEY_LCONTROL])ret += 0x10;
                if(key[KEY_LEFT])ret += 0x20;
                if(key[KEY_RIGHT])ret += 0x40;
        }
        if(port == 3)ret = invaders_shift(3,0);
        return ret;
}

void invaders_out(BYTE port,BYTE op)
{
        if(port == 2)
        {
                invaders_shift(2,op);
                if(_DEBUG)fprintf(debug,"%X wird an Port %X geschrieben\n", op, port);
        }
        else
        if(port == 3)
        {
                invaders_sound(3,op);
        }
        else
        if(port == 4)
        {
                invaders_shift(4,op);
                if(_DEBUG)fprintf(debug,"%X wird an Port %X geschrieben\n", op, port);
        }
        else
        if(port == 5)
        {
                invaders_sound(5,op);
        }
        else
        if(port == 6)
        {
                //
        }
        else
        {
        }
}

WORD invaders_in(BYTE port)
{
        if(_DEBUG && port != 1 && port != 2)fprintf(debug,"Port %X wird gelesen\n",port);
        return port_read(port);
}

int main(int argc, char* argv[])
{
        WORD game = 0;
        WORD loop = 0;
        WORD display_update = 0;
        WORD cpu_cycles = 0;
        clock_t i8080_time;
        clock_t i8080_clock;
        struct timespec ts;

        set_memlocs();

        if(argc < 2)
        {
                syntax();
                return 2;
        }
        if(!strcmp((char*)strlwr(argv[1]),"invaders"))
        {                 
                game = 1;
        }
        if(!strcmp((char*)strlwr(argv[1]),"spaceatt"))
        {                 
                game = 2;
        }
        if(!strcmp((char*)strlwr(argv[1]),"earthinv"))
        {                 
                game = 3;
        }
        if(!strcmp((char*)strlwr(argv[1]),"invadpt2"))
        {                 
                game = 4;
                SPECIALROM = 1;
        }
        if(argc > 2)
        {
          if(!strcmp((char*)strlwr(argv[2]),"nosound"))
          {                 
                  SOUND = 0;
          }
          if(!strcmp((char*)strlwr(argv[2]),"debug"))
          {                 
                  _DEBUG = 1;
          }
          if(!strcmp((char*)strlwr(argv[2]),"debug2"))
          {                 
                  _DEBUG = 2;
          }
        }
        if(!game)
        {
                syntax();
                return 3;
        }

        if(!load_roms(game))return 4;
        if(SOUND)
        {
                if(!load_sounds(game))return 4;
        }
        //message();

        allegro_init();
        install_keyboard();
        if(SOUND)
        {
                if(install_sound(DIGI_AUTODETECT, MIDI_NONE ,"dfg") != 0)
                {
                        printf("Fehler beim Initialisieren des Sound-Systems");
                        SOUND = 0;
                }
        }

        set_gfx_mode(GFX_AUTODETECT_WINDOWED, 224, 248, 0, 0);
        double_buffer = create_bitmap(224, 248);
        clear_to_color(double_buffer, 0);

        if(_DEBUG)debug = fopen("DEBUG.TXT","wt");
        if(_DEBUG == 2)debug2 = fopen("DEBUG2.TXT","wt");

        i8080_time = 0x26D6; // 9942 Cycles = 1/120 Second
        i8080_clock = clock(); // For initialisation only

        for(;;)
        {
                if(key[KEY_ESC])break;
                if(key[KEY_F10])snapshot();
                if(key[KEY_F3])reset();
                if(key[KEY_B])
                {
                        i8080_time = 0x1000; // Speed up wait Cycle 136B
                }
#ifdef SLOW
                if(key[KEY_H])halt ^= 1;
                if(key[KEY_I])INT ^= 1;
                if(!halt && key[KEY_P]){ INT = 0; halt = 1; }
                if(!halt)
                {
#endif
                        i8080_clock = clock();
                        cpu_cycles = cpu(17067); // 2500); // 33333 Cycles = 60 HZ(34133?)
                        display_update += cpu_cycles;

                        if(cpu_cycles == -1000)return 6; 
#ifdef SLOW
                }
                if(key[KEY_S])
                {
                        cpu_cycles = cpu(1); // 33333 Cycles = 60 HZ(34133?)
                        display_update += cpu_cycles;

                        if(cpu_cycles == -1000)return 6;
                }
#endif
                if(display_update >= 34133)
                {
                        display_update = 0;
                        update_screen();
                }
                if(INT)
                {
                        halt = 0;
                        if(loop == 2)loop = 0;
                        else loop++;

                        if(loop == 1)interrupt();
                        if(loop == 2)NMI();
                }

                ts.tv_sec = 0;
                ts.tv_nsec = (i8080_time - (clock() -i8080_clock)) * 1000;
                nanosleep(&ts, NULL);
        }
        if(_DEBUG)fclose(debug);
        if(_DEBUG == 2)fclose(debug2);

        free(memory);

        if(SOUND)
        {
                destroy_sample(sample_00);
                destroy_sample(sample_01);
                destroy_sample(sample_02);
                destroy_sample(sample_03);
                destroy_sample(sample_04);
                destroy_sample(sample_05);
                destroy_sample(sample_06);
                destroy_sample(sample_07);
                destroy_sample(sample_08);
                destroy_sample(sample_09);
        }

        allegro_exit();
        return 1;
}
END_OF_MAIN()