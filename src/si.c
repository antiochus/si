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

#define BYTE unsigned char
#define WORD unsigned short int
#define DWORD unsigned long int

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

extern WORD INT;

WORD cpu(WORD cycles);
void interrupt(void);
void NMI(void);
void reset(void);

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

WORD load_samples(char* filename, WORD sample_number)
{
        if(sample_number == 31)
        {
                sample3_1 = load_sample(filename);
                if(!sample3_1){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 32)
        {
                sample3_2 = load_sample(filename);
                if(!sample3_2){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 33)
        {
                sample3_3 = load_sample(filename);
                if(!sample3_3){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 34)
        {
                sample3_4 = load_sample(filename);
                if(!sample3_4){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 51)
        {
                sample5_1 = load_sample(filename);
                if(!sample5_1){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 52)
        {
                sample5_2 = load_sample(filename);
                if(!sample5_2){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 53)
        {
                sample5_3 = load_sample(filename);
                if(!sample5_3){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 54)
        {
                sample5_4 = load_sample(filename);
                if(!sample5_4){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        else if(sample_number == 55)
        {
                sample5_5 = load_sample(filename);
                if(!sample5_5){printf("Ein Fehler ist beim Einlesen der Datei %s aufgetreten\n",filename);return 0;}
        }
        return 1;
}

WORD load_roms(WORD game)
{
        memory = (BYTE*) malloc(65535);
        if(memory == NULL)
        {
                printf("Eine Fehler ist beim allokieren des virtuellen Speichers aufgetreten\n");
                printf("Ist nicht genuegend Speicher im RAM frei ?\n"); 
                return 0;
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
        if(!load_samples("samples/0.wav",31))return 0;
        if(!load_samples("samples/1.wav",32))return 0;
        if(!load_samples("samples/2.wav",33))return 0;
        if(!load_samples("samples/12.wav",34))return 0;
        if(!load_samples("samples/4.wav",51))return 0;
        if(!load_samples("samples/5.wav",52))return 0;
        if(!load_samples("samples/6.wav",53))return 0;
        if(!load_samples("samples/7.wav",54))return 0;
        if(!load_samples("samples/3.wav",55))return 0;
        return 1;
}

inline void invaders_sound(WORD channel, WORD op)
{
        if(SOUND)
        {
                if(channel == 3)
                {
                if(op & 0x01)play_sample(sample3_1, 255, 128, 1000, TRUE);
                if(op & 0x02)play_sample(sample3_2, 255, 128, 1000, FALSE);
                if(op & 0x04)play_sample(sample3_3, 255, 128, 1000, FALSE);
                if(op & 0x10)play_sample(sample3_4, 255, 128, 1000, FALSE);
                }
                else if(channel == 5)
                {
                if(op & 0x01)play_sample(sample5_1, 255, 128, 1000, FALSE);
                if(op & 0x02)play_sample(sample5_2, 255, 128, 1000, FALSE);
                if(op & 0x04)play_sample(sample5_3, 255, 128, 1000, FALSE);
                if(op & 0x10)play_sample(sample5_4, 255, 128, 1000, FALSE);
                if(op & 0x20)play_sample(sample5_5, 255, 128, 1000, FALSE);
                }
        }
}

inline void update_screen()
{
        blit(double_buffer, screen, 0, 0, 0, 0, 256, 256);
}

inline void snapshot()
{
        PALETTE pal;
        get_palette(pal);
        save_bmp("si.bmp", double_buffer, pal);
}

void update_buffer(WORD offset, BYTE data)
{
        int b,c,x,y;

        x = (offset - 0x2400) >> 5;
        y = (offset - 0x2400) & 0x1F;
        c = 15;
        if(y < 8) c = 2;
        if(y > 23 && y < 28) c = 4;
        for (b = 0; b < 8; b++)
        {
                //if(data & 0x01) putpixel(screen, x, 256 - ((y << 3) + b), c);
                //else putpixel(screen, x, 256 - ((y << 3) + b), 0); data = data >> 1;
                if(data & 0x01) putpixel(double_buffer, x, 256 - ((y << 3) + b), c);
                else putpixel(double_buffer, x, 256 - ((y << 3) + b), 0); data = data >> 1;
        }
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
        if(port == 3)invaders_sound(3,op);
        if(port == 4)
        {
                invaders_shift(4,op);
                if(_DEBUG)fprintf(debug,"%X wird an Port %X geschrieben\n", op, port);
        }
        else if(port == 5)invaders_sound(5,op);
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
                        return 5;
                }
        }

        set_gfx_mode(GFX_AUTODETECT, 256, 256, 0, 0);
        double_buffer = create_bitmap(256,256);
        clear_to_color(double_buffer,0);

        if(_DEBUG)debug = fopen("DEBUG.TXT","wt");
        if(_DEBUG == 2)debug2 = fopen("DEBUG2.TXT","wt");

        i8080_time = 0x26D6; // 9942 Cycles = 1/120 Second
        i8080_clock = clock(); // For initialisation only

        for(;;)
        {
                if(key[KEY_ESC])break;
                if(key[KEY_F10])snapshot();
                if(key[KEY_F3])reset();
                if(key[KEY_B])i8080_time ^= 0x136B; // Speed up wait Cycle 
#ifdef SLOW
                if(key[KEY_H])halt ^= 1;
                if(key[KEY_I])INT ^= 1;
                if(!halt && key[KEY_P]){ INT = 0; halt = 1; }
                if(!halt)
                {
#endif
                        i8080_clock = clock();
                        cpu_cycles = cpu(17067); // 33333 Cycles = 60 HZ(34133?)
                        display_update += cpu_cycles;

                        if(!cpu_cycles)return 6; 
#ifdef SLOW
                }
                if(key[KEY_S])
                {
                        cpu_cycles = cpu(1); // 33333 Cycles = 60 HZ(34133?)
                        display_update += cpu_cycles;

                        if(!cpu_cycles)return 6;
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

                while(clock() < (i8080_clock + i8080_time));
        }
        if(_DEBUG)fclose(debug);
        if(_DEBUG == 2)fclose(debug2);

        free(memory);

        if(SOUND)
        {
                destroy_sample(sample3_1);
                destroy_sample(sample3_2);
                destroy_sample(sample3_3);
                destroy_sample(sample3_4);
                destroy_sample(sample5_1);
                destroy_sample(sample5_2);
                destroy_sample(sample5_3);
                destroy_sample(sample5_4);
                destroy_sample(sample5_5);
        }

        allegro_exit();
        return 1;
}
 END_OF_MAIN()