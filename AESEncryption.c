#include "AES.h"
int main(int argc, char** argv){

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

    if(argc > 1) inputFile = fopen(argv[1],"r");
    else inputFile = fopen("ToEncrypt.txt","r");

    if(argc > 2)outputFile = fopen(argv[2],"w+");
    else outputFile = fopen("Encrypted.txt","w+");

    if(argc > 3) keyFile = fopen(argv[3],"r");
    else keyFile = fopen("key.txt", "r");

    if(argc > 4)outputFileHex = fopen(argv[4],"w+");
    else outputFileHex = fopen("EncryptedHex.txt","w+");

    //writeGFInvTable("AESInvTable.txt");
    writeGFInvTable("AESInvTable.txt");
    invTable = readGFInvTable("AESInvTable.txt");

    //Write the key from the file
    fread(key, sizeof(word), BLOCK_SIZE, keyFile);
    for(int x = 0; x < BLOCK_SIZE; x++) key[x] = flipBytes(key[x]);


    //Print key, for test reasons
    printf("Key: ");
    printKey(key);

    roundKeys = wordExpansion(key, invTable);

    while(keepRunning == 1){
        readSize = 0;

        for(int x = 0; x < BLOCK_SIZE; x++){
          currentRead[x] = 0;
          if(keepRunning == 0){
            if(x == BLOCK_SIZE - 1) *(currentRead + x) = readSize;
            else *(currentRead + x) = 0;
          } else {
            readSize += fread(currentRead + x, sizeof(char), sizeof(word), inputFile);
            if(readSize == 0 || readSize % sizeof(word) != 0) keepRunning = 0;
          }
          printCurrentRead(currentRead);
        }
        
        //Flip bytes, becuase fread is being annoying
        for(int x = 0; x < BLOCK_SIZE; x++) currentRead[x] = flipBytes(currentRead[x]);

        for(int x = 0; x < BLOCK_SIZE; x++){
            currentRead[x] = currentRead[x] ^ roundKeys[x];
        }

        for(byte round = 1; round < ROUNDS; round++){

            //First, we will be substituting each byte with its inverse in the GF(2^8) field

            subBytesAll(currentRead, invTable);

            //Second, shift the rows over by certain amounts

            currentRead = flipRows(currentRead);

            shiftRows(currentRead);

            //Third, we will mix the columns

            if(round < 10)mixColumns(currentRead);
            //printCurrentReadSquare(currentRead);

            currentRead = flipRows(currentRead);

            //Finally, we XOR the currentRead with the round keys

            currentRead[0] ^= roundKeys[(round * BLOCK_SIZE)];
            currentRead[1] ^= roundKeys[(round * BLOCK_SIZE) + 1];
            currentRead[2] ^= roundKeys[(round * BLOCK_SIZE) + 2];
            currentRead[3] ^= roundKeys[(round * BLOCK_SIZE) + 3];

        }
        //printCurrentRead(currentRead);

        for(int x = 0; x < BLOCK_SIZE; x++){
          currentRead[x] = flipBytes(currentRead[x]);
          printf("%x\n", *(currentRead + x));
          fwrite(currentRead + x, sizeof(word), 1, outputFile);
          writeWord(outputFileHex,currentRead[x]);
        }

    }

    fclose(inputFile);
    fclose(outputFile);
    free(invTable);
    free(roundKeys);
    free(currentRead);
    free(key);

    return 0;
}
