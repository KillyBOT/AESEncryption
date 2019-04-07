#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMMUTABLE_POLYNOMIAL 0x11b
#define GENERATOR 0x03
#define C_BYTE 0x63

typedef unsigned char byte;

byte GFMult(byte, byte);
byte GFExp(byte, byte);
byte GFFastMult(byte, byte, byte*, byte*);
byte GFInv(byte, byte*);
byte* createGFExpTable(byte);
byte* createGFLogTable(byte);
byte* createGFInvTable(byte*, byte*);
void printGFTable(byte*);

int main(){
    byte test1 = 0xb6;
    byte test2 = 0x53;
    byte* expTable;
    byte* logTable;
    byte* invTable;

    printf("%x\t%x\n",GFMult(test1, test2), GFExp(0x03,0x02));

    printf("%d\n",sizeof(byte));

    expTable = createGFExpTable(GENERATOR);
    logTable = createGFLogTable(GENERATOR);
    invTable = createGFInvTable(expTable, logTable);
    //printGFTable(expTable);
    //printGFTable(logTable);
    //printGFTable(invTable);

    free(expTable);
    free(logTable);
    free(invTable);

    return 0;
}

byte GFMult(byte a, byte b){
    byte aa = a;
    byte bb = b;
    //R is remainder, temp is a temporary value
    byte r = 0, temp;

    while( aa != 0){
        if((aa & 1) != 0) r = r ^ bb;

        temp = bb & 0x80;
        bb = bb << 1;
        if(temp != 0){
            bb = bb ^ IMMUTABLE_POLYNOMIAL;
        }

        aa = aa >> 1;
    } 

    return r;
}

byte GFExp(byte base, byte power){
    if (power <= 0) return 0x01;

    byte toRet = base;

    for(byte b = 1; b < power; b++) toRet = GFMult(toRet, base);

    return toRet;
}

byte* createGFExpTable(byte generator){
    byte* table = malloc(256);
    
    for(byte b = 0; b < 0xff; b++){
        table[ b ] = GFExp(generator, b);
        //printf("%x\t%x\n", GFExp(generator, b),b);
    }

    return table;
}

byte* createGFLogTable(byte generator){
    byte* table = malloc(256);


    
    for(byte b = 0; b < 0xff; b++){
        table[ GFExp(generator, b) ] = b;
        //printf("%x\t%x\n", GFExp(generator, b),b);
    }

    return table;
}

void printGFTable(byte* table){
    printf("   ");
    for(byte b = 0; b < 16; b++) printf("%x  ",b);
    printf("\n");

    for(byte row = 0; row < 16; row++){
        printf("%x ", row);
        for(byte col = 0; col < 16; col++){
            if(table[(row << 4) | col] < 0x10) printf("0");
            printf("%x ", table[(row << 4) | col]);
        }
        printf("\n");
    }
}

byte GFFastMult(byte a, byte b, byte* ExpTable, byte* LogTable){
    if(a == 0 || b == 0) return b;
    int t = LogTable[a] + LogTable[b];
    if(t > 0xff) t = t & 0xff;
    return ExpTable[t];
}

byte* createGFInvTable(byte* expTable, byte* logTable){

    byte* table = malloc(256);

    for(byte b = 1; b < 0xff; b++){
        table[b] = expTable[0xff - logTable[b]];
    }

    //Make some adjustments to the table to make it work, probably not good practice

    table[0xff] = expTable[0xff - logTable[0xff]];
    table[0x01] = 0x01;
    table[0x00] = C_BYTE;

    return table;

}

byte GFInv(byte in, byte* invTable){
    return invTable[in];
}