#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMMUTABLE_POLYNOMIAL 0x11b
#define GENERATOR 0x03
#define C_BYTE 0x63
#define ROUNDS 44
#define BLOCK_SIZE 16

typedef unsigned char byte;
typedef unsigned int word;

byte GFMult(byte, byte);
byte GFExp(byte, byte);
byte GFFastMult(byte, byte, byte*, byte*);
byte GFInv(byte, byte*);
word SubBytes(word, byte*);
byte byteRotateLeft(byte, byte);
byte byteRotateRight(byte, byte);
word wordRotateLeft(word, byte);
word wordRotateRight(word, byte);

byte* createGFExpTable(byte);
byte* createGFLogTable(byte);
byte* createGFInvTable(byte*, byte*);
byte* createGFInvTableBruteForce();

void printGFTable(byte*);
void printWord(word);
void printByte(byte);
void printKey(word*);
void writeGFInvTable(char*);
byte* readGFInvTable(char*);

word* wordExpansion(word*, byte*);
word getRoundKey(word*, byte);
word g(word, byte, byte*);
byte roundConstant(byte);

byte getBit(byte, byte);
byte setBit(byte, byte, byte);

int main(){

    FILE* inputFile;
    FILE* outputFile;
    FILE* keyFile;
    byte toEncrypt[2048];

    byte* invTable;

    word* roundKeys;
    word* testKey = malloc(sizeof(word) * 4);
    word* key = malloc(sizeof(word) * 4);

    byte* currentRead = malloc(sizeof(byte) * 16);
    
    int keepRunning = 1;

    testKey[0] = 0x2b7e1516;
    testKey[1] = 0x28aed2a6;
    testKey[2] = 0xabf71588;
    testKey[3] = 0x9cf4f3c;

    printf("%x\n",testKey[3]);

    //This sets all the empty spaces to, well, spaces.
    key[0] = 0x30303030;
    key[1] = 0x30303030;
    key[2] = 0x30303030;
    key[3] = 0x30303030;

    inputFile = fopen("ToEncrypt.txt","r");
    outputFile = fopen("Encrypted.txt","w+");
    keyFile = fopen("key.txt", "r");

    //writeGFInvTable("AESInvTable.txt");
    writeGFInvTable("AESInvTable.txt");
    invTable = readGFInvTable("AESInvTable.txt");

    printByte(C_BYTE);    

    roundKeys = wordExpansion(testKey, invTable);
    printf("%x\n",setBit(0x16,1,1));

    //Write the key from the file
    fread(key, sizeof(byte), 16, keyFile);

    //Print key, for test reasons
    printKey(key);

    for(int x = 0; x < ROUNDS; x++){
        printWord(roundKeys[x]);
    }

    while(keepRunning == 1){
        if(fread(currentRead, sizeof(byte), BLOCK_SIZE, inputFile) != BLOCK_SIZE) keepRunning = 0;

        printf("%s\n",currentRead);
        for(byte round = 0; round < ROUNDS; round++){
            //printWord(roundKeys[round]);
            //First, we will be substituting each byte with its inverse in the GF(2^8) field
            for(byte b = 0; b < BLOCK_SIZE; b++){
                currentRead[b] = GFInv(currentRead[b], invTable);
            }
        }

        fwrite(currentRead, sizeof(byte), BLOCK_SIZE, outputFile);
    }
    
    fclose(inputFile);
    fclose(outputFile);
    free(invTable);
    free(roundKeys);
    free(testKey);

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

byte byteRotateLeft(byte toRotate, byte amount){
    byte temp;
    byte toRet = toRotate;
    for(byte b = 0; b < amount; b++){
        temp = toRet & 0x80;
        temp = temp >> 7;
        toRet = toRet << 1;
        toRet = toRet ^ temp;
    }

    return toRet;
}

byte byteRotateRight(byte toRotate, byte amount){
    byte temp;
    byte toRet = toRotate;
    for(byte b = 0; b < amount; b++){
        temp = toRet & 0x01;
        temp = temp << 7;
        toRet = toRet >> 1;
        toRet = toRet ^ temp;
    }

    return toRet;
}

word wordRotateLeft(word toRotate, byte amount){
    word temp;
    word toRet = toRotate;
    for(byte b = 0; b < amount; b++){
        temp = toRet & 0x80000000;
        temp = temp >> 31;
        toRet = toRet << 1;
        toRet = toRet ^ temp;
    }

    return toRet;
}


word wordRotateRight(word toRotate, byte amount){
    word temp;
    word toRet = toRotate;
    for(byte b = 0; b < amount; b++){
        temp = toRet & 0x01;
        temp = temp << 31;
        toRet = toRet >> 1;
        toRet = toRet ^ temp;
    }

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

    for(int b = 1; b < 0x100; b++){
        table[b] = expTable[0xff - logTable[b]];
    }

    //Make some adjustments to the table to make it work, probably not good practice

    table[0x01] = 0x01;
    table[0x00] = 0x00;

    return table;

}

byte GFInv(byte in, byte* invTable){
    return invTable[in];
}

word SubBytes(word inWord, byte* invTable){

    word newWord = 0;
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = inWord >> (8 * i);
        temp = GFInv(temp, invTable);
        temp = temp ^ byteRotateRight(temp, 4) ^ byteRotateRight(temp, 5) ^ byteRotateRight(temp, 6) ^ byteRotateRight(temp, 7) ^ C_BYTE;
        newWord = newWord << 8;
        newWord = newWord | temp;
    }

    return newWord;
}

void printWord(word toPrint){
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = toPrint >> (8*i);
        if(temp < 16) printf("0");
        printf("%x",temp);
    }
    printf("\n");
}

void printByte(byte toPrint){
    byte temp = toPrint;
    for(int x = 0; x < 8; x++){
        //printf("%x\n",temp & 0x01);
        if((temp & 0x80) == 0x80) printf("1");
        else printf("0");
        temp = temp << 1;
    }
    printf("\n");
}

void printKey(word* keyToPrint){
    for(int x = 0; x < 4; x++){
        printf("%x",keyToPrint[x]);
    }
    printf("\n");
}

void writeGFInvTable(char* filename){
    FILE* fp;

    fp = fopen(filename, "w+b");

    byte* expTable = createGFExpTable(GENERATOR);
    byte* logTable = createGFLogTable(GENERATOR);
    byte* invTable = createGFInvTable(expTable, logTable);

    fwrite(invTable, sizeof(byte), 256, fp);

    fclose(fp);

    free(expTable);
    free(logTable);
    free(invTable);
}

byte* readGFInvTable(char* filename){
    FILE* fp;
    byte* table = malloc(256);

    fp = fopen(filename, "rb");
    fread(table, 1, 256, fp);

    fclose(fp);
    return table;
}

word* wordExpansion(word* initialKey, byte* invTable){
    word* roundWords = malloc(sizeof(word) * ROUNDS);

    roundWords[0] = initialKey[0];
    roundWords[1] = initialKey[1];
    roundWords[2] = initialKey[2];
    roundWords[3] = initialKey[3];

    byte currentPlace;
    for(byte round = 1; round < ROUNDS / sizeof(word); round++){
        currentPlace = round * 4;
        roundWords[currentPlace] = roundWords[currentPlace-4] ^ g(roundWords[currentPlace-1],round,invTable);
        roundWords[currentPlace+1] = roundWords[currentPlace] ^ roundWords[currentPlace-3];
        roundWords[currentPlace+2] = roundWords[currentPlace+1] ^ roundWords[currentPlace-2];
        roundWords[currentPlace+3] = roundWords[currentPlace+2] ^ roundWords[currentPlace-1];
    }

    return roundWords;
}

word g(word endWord, byte round, byte* invTable){

    word toRet = endWord;

    toRet = wordRotateLeft(toRet, 8);
    //printf("%x ",toRet);

    toRet = SubBytes(toRet, invTable);
    //printf("%x ",toRet);

    word currentRoundConstant = roundConstant(round) << 24;
    toRet = toRet ^ currentRoundConstant;
    //printf("%x\n",toRet);

    return toRet;
}

byte roundConstant(byte round){
    if(round <= 1) return 0x01;
    else return GFMult(0x02, roundConstant(round - 1));
}

byte getBit(byte inByte, byte pos){
    byte ret = inByte;
    ret = ret >> (7 - pos);
    ret = ret & 0x01;
    return ret;
}

byte setBit(byte inByte, byte pos, byte newVal){
    byte newBit = newVal;
    newBit = newBit << (7 - pos);
    if(newVal == 0) return inByte & newBit;
    else return inByte | newBit;
}