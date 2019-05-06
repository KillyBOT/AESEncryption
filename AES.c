#include "AES.h"

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

word* flipRows(word* rows){
    word* flippedRows = malloc(sizeof(word)*4);
    word toAdd;
    byte byteToAdd;
    for( int x = 3; x >= 0; x--){
        toAdd = 0;
        for(int y = 0; y < 4; y++){
            byteToAdd = getByte(rows[y], x);
            toAdd = toAdd << 8;
            toAdd = toAdd | byteToAdd;
            //printf("%x\n",toAdd);

        }
        flippedRows[x] = toAdd;
    }
    //printCurrentReadSquare(rows);
    //I got really lazy and I decided to just swap the values to fix an issue

    word temp = flippedRows[0];
    flippedRows[0] = flippedRows[3];
    flippedRows[3] = temp;

    temp = flippedRows[1];
    flippedRows[1] = flippedRows[2];
    flippedRows[2] = temp;
    //printCurrentReadSquare(flippedRows);

    return flippedRows;


}

word flipBytes(word toFlip){
	word flipped = 0;
	word remainder = toFlip;
	word temp;

	for(int x = 0; x < 3; x++){
		temp = remainder & 0xff000000;
		flipped = flipped | temp;
		flipped = flipped >> 8;
		remainder = remainder << 8;
	}

	temp = remainder & 0xff000000;
	flipped = flipped | temp;

	return flipped;
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

void printWord(word toPrint){
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = toPrint >> (8*i);
        if(temp < 16) printf("0");
        printf("%x",temp);
    }
}

void printWordASCII(word toPrint){
	for(int b = 3; b >= 0; b++){
		printf("%c",getByte(toPrint,b));
	}
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

void printCurrentRead(word* currentRead){
    for(int x = 0; x < 4; x++){
        printWord(currentRead[x]);
    }
    printf("\n");
}

void printCurrentReadSquare(word* currentRead){
    for(int x = 0; x < 4; x++){
        printWord(currentRead[x]);
        printf("\n");
    }
    printf("\n");
}

void printCurrentReadASCII(word* currentRead){
	for(int x = 0; x < 4; x++){
		for( int b = 3; b >= 0; b--){
			printf("%c",getByte(currentRead[x],b));
		}
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

void writeWord(FILE* fp, word toPrint){
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = toPrint >> (8*i);
        if(temp < 16) fprintf(fp,"0");
        fprintf(fp,"%x",temp);
    }
}

byte subBytes(byte in, byte* invTable){
    byte toRet = in;
    toRet = GFInv(toRet, invTable);
    toRet = toRet ^ byteRotateRight(toRet, 4) ^ byteRotateRight(toRet, 5) ^ byteRotateRight(toRet, 6) ^ byteRotateRight(toRet, 7) ^ C_BYTE;
    return toRet;
}

word subBytesWord(word inWord, byte* invTable){

    word newWord = 0;
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = inWord >> (8 * i);
        temp = subBytes(temp, invTable);
        newWord = newWord << 8;
        newWord = newWord | temp;
    }

    return newWord;
}

void subBytesAll(word* currentRead, byte* invTable){

    for(int x = 0; x < 4; x++){
        currentRead[x] = subBytesWord(currentRead[x], invTable);
    }
}


byte subBytesDecrypt(byte in, byte* invTable){
    byte toRet = in;
    toRet = byteRotateRight(toRet, 2) ^ byteRotateRight(toRet, 5) ^ byteRotateRight(toRet, 7) ^ D_BYTE;
    toRet = GFInv(toRet, invTable);

    return toRet;
}

word subBytesWordDecrypt(word inWord, byte* invTable){

    word newWord = 0;
    byte temp;
    for(int i = 3; i >= 0; i--){
        temp = inWord >> (8 * i);
        temp = subBytesDecrypt(temp, invTable);
        newWord = newWord << 8;
        newWord = newWord | temp;
    }

    return newWord;
}

void subBytesAllDecrypt(word* currentRead, byte* invTable){

    for(int x = 0; x < 4; x++){
        currentRead[x] = subBytesWordDecrypt(currentRead[x], invTable);
    }
}


void shiftRows(word* rows){

    rows[1] = wordRotateLeft(rows[1],8);
    rows[2] = wordRotateLeft(rows[2],16);
    rows[3] = wordRotateLeft(rows[3],24);
}

void shiftRowsDecrypt(word* rows){
	rows[1] = wordRotateRight(rows[1],8);
    rows[2] = wordRotateRight(rows[2],16);
    rows[3] = wordRotateRight(rows[3],24);
}

void mixColumns(word* rows){
    word oldRows0, oldRows1, oldRows2, oldRows3;
    for(int col = 0; col < 4; col++){
        oldRows0 = rows[0];
        oldRows1 = rows[1];
        oldRows2 = rows[2];
        oldRows3 = rows[3];
        rows[0] = setByte( rows[0], col, GFMult(2, getByte(oldRows0,col)) ^ GFMult(3, getByte(oldRows1,col)) ^ getByte(oldRows2, col) ^ getByte(oldRows3, col));
        rows[1] = setByte( rows[1], col, getByte(oldRows0, col) ^ GFMult(2, getByte(oldRows1,col)) ^ GFMult(3, getByte(oldRows2, col)) ^ getByte(oldRows3, col));
        rows[2] = setByte( rows[2], col, getByte(oldRows0, col) ^ getByte(oldRows1, col) ^ GFMult(2, getByte(oldRows2, col)) ^ GFMult(3,getByte(oldRows3, col)));
        rows[3] = setByte( rows[3], col, GFMult(3,getByte(oldRows0, col)) ^ getByte(oldRows1, col) ^ getByte(oldRows2, col) ^ GFMult(2,getByte(oldRows3, col)));
    }  
}

void mixColumnsDecrypt(word* rows){
    word oldRows0, oldRows1, oldRows2, oldRows3;
    for(int col = 0; col < 4; col++){
        oldRows0 = rows[0];
        oldRows1 = rows[1];
        oldRows2 = rows[2];
        oldRows3 = rows[3];
        rows[0] = setByte( rows[0], col, GFMult(0x0e, getByte(oldRows0,col)) ^ GFMult(0x0b, getByte(oldRows1,col)) ^ GFMult(0x0d, getByte(oldRows2, col)) ^ GFMult(0x09,getByte(oldRows3, col)));
        rows[1] = setByte( rows[1], col, GFMult(0x09, getByte(oldRows0,col)) ^ GFMult(0x0e, getByte(oldRows1,col)) ^ GFMult(0x0b, getByte(oldRows2, col)) ^ GFMult(0x0d,getByte(oldRows3, col)));
        rows[2] = setByte( rows[2], col, GFMult(0x0d, getByte(oldRows0,col)) ^ GFMult(0x09, getByte(oldRows1,col)) ^ GFMult(0x0e, getByte(oldRows2, col)) ^ GFMult(0x0b,getByte(oldRows3, col)));
        rows[3] = setByte( rows[3], col, GFMult(0x0b, getByte(oldRows0,col)) ^ GFMult(0x0d, getByte(oldRows1,col)) ^ GFMult(0x09, getByte(oldRows2, col)) ^ GFMult(0x0e,getByte(oldRows3, col)));
    }  
}


word* wordExpansion(word* initialKey, byte* invTable){
    word* roundWords = malloc(sizeof(word) * ROUNDS * 4);

    roundWords[0] = initialKey[0];
    roundWords[1] = initialKey[1];
    roundWords[2] = initialKey[2];
    roundWords[3] = initialKey[3];

    byte currentPlace;
    for(byte round = 1; round < ROUNDS; round++){
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

    toRet = subBytesWord(toRet, invTable);
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

byte getByte(word in, byte byteToGet){
    byte temp = wordRotateRight(in, byteToGet * 8);
    return temp;
}

word setByte(word in, byte bytePos, byte newByte){
    word toRet = in;

    word temp = 0xffffff00;
    temp = wordRotateLeft(temp, bytePos * 8);

    toRet = toRet & temp;

    temp = 0x000000ff & newByte;
    temp = temp << (bytePos * 8);
    toRet = toRet | temp;

    return toRet;

}