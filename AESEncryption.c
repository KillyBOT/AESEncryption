#include "AES.h"
int main(){

    FILE* inputFile;
    FILE* outputFile;
    FILE* outputFileHex;
    FILE* keyFile;
    byte toEncrypt[2048];

    byte* invTable;

    word* roundKeys;
    //word* testKey = malloc(sizeof(word) * 4);
    word* key = malloc(sizeof(word) * 4);

    word* currentRead = malloc(sizeof(byte) * 16);
    
    int keepRunning = 1;
    int readSize;

    //testKey[0] = 0x2b7e1516;
    //testKey[1] = 0x28aed2a6;
    //testKey[2] = 0xabf71588;
    //testKey[3] = 0x9cf4f3c;

    //This sets all the empty spaces to, well, spaces.
    key[0] = 0x30303030;
    key[1] = 0x30303030;
    key[2] = 0x30303030;
    key[3] = 0x30303030;

    inputFile = fopen("ToEncrypt.txt","r");
    outputFile = fopen("Encrypted.txt","w+");
    outputFileHex = fopen("EncryptedHex.txt","w+");
    keyFile = fopen("key.txt", "r");

    //writeGFInvTable("AESInvTable.txt");
    writeGFInvTable("AESInvTable.txt");
    invTable = readGFInvTable("AESInvTable.txt");

    //Write the key from the file
    fread(key, sizeof(word), BLOCK_SIZE, keyFile);
    for(int x = 0; x < BLOCK_SIZE; x++) key[x] = flipBytes(key[x]);


    //Print key, for test reasons
    printKey(key);

    roundKeys = wordExpansion(key, invTable);

    while(keepRunning == 1){

        readSize = fread(currentRead, sizeof(word), BLOCK_SIZE, inputFile);
        
        if(readSize != BLOCK_SIZE) {
            keepRunning = 0;
            for(int x = readSize; x < BLOCK_SIZE; x++){
                currentRead[x] = 0x20202020;
            }
        }
        //Flip bytes, becuase fread is being annoying
        for(int x = 0; x < BLOCK_SIZE; x++) currentRead[x] = flipBytes(currentRead[x]);
        //printCurrentRead(currentRead);
        
        //printCurrentRead(currentRead);

        //word* currentRead = malloc(sizeof(word) * 4);
        //currentRead[0] = 0x3243f6a8;
        //currentRead[1] = 0x885a308d;
        //currentRead[2] = 0x313198a2;
        //currentRead[3] = 0xe0370734;

        for(int x = 0; x < BLOCK_SIZE; x++){
            currentRead[x] = currentRead[x] ^ roundKeys[x];
        }

        for(byte round = 1; round < ROUNDS; round++){
            //printCurrentReadSquare(roundKeys + (4 * round));
            //printCurrentReadSquare(currentRead);

            //First, we will be substituting each byte with its inverse in the GF(2^8) field

            subBytesAll(currentRead, invTable);

            //printCurrentReadSquare(currentRead);

            //Second, shift the rows over by certain amounts

            currentRead = flipRows(currentRead);

            shiftRows(currentRead);

            //printCurrentReadSquare(currentRead);

            //Third, we will mix the columns

            if(round < 10)mixColumns(currentRead);
            //printCurrentReadSquare(currentRead);

            currentRead = flipRows(currentRead);

            //Finally, we XOR the currentRead with the round keys

            currentRead[0] ^= roundKeys[(round * BLOCK_SIZE)];
            currentRead[1] ^= roundKeys[(round * BLOCK_SIZE) + 1];
            currentRead[2] ^= roundKeys[(round * BLOCK_SIZE) + 2];
            currentRead[3] ^= roundKeys[(round * BLOCK_SIZE) + 3];

            //printCurrentRead(currentRead);



        }
        printCurrentRead(currentRead);
        for(int x = 0; x < BLOCK_SIZE; x++) currentRead[x] = flipBytes(currentRead[x]);
        fwrite(currentRead, sizeof(word), BLOCK_SIZE, outputFile);

        for(int x = 0; x < BLOCK_SIZE; x++) writeWord(outputFileHex,currentRead[x]);
    
    }
    
    fclose(inputFile);
    fclose(outputFile);
    free(invTable);
    free(roundKeys);
    free(currentRead);
    free(key);

    return 0;
}