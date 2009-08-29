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
