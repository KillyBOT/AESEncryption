#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMMUTABLE_POLYNOMIAL 0x11b

typedef unsigned char byte;

byte GFMult(byte, byte);
byte GFExp(byte, byte);
byte* createGFExpTable(byte);

int main(){
    byte test1 = 0xb6;
    byte test2 = 0x53;

    printf("%X\t%X\n",GFMult(test1, test2));

    byte* table = createGFExpTable(0x03);

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

    for(byte b = 0; b < power; b++) toRet = GFMult(toRet, base);

    return toRet;
}