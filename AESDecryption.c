#include "AES.h"
int main(int argc, char** argv){

    FILE* inputFile;
    FILE* outputFile;
    FILE* outputFileHex;
    FILE* keyFile;
    byte toEncrypt[2048];

    byte* invTable;

    word* roundKeys;
    word* testKey = malloc(sizeof(word) * 4);
    word* key = malloc(sizeof(word) * 4);

    word* currentRead = malloc(sizeof(byte) * 16);

    int keepRunning = 1;
    int sizeOfRead;
    int finalSizeOfWrite = sizeof(word) * BLOCK_SIZE;

    testKey[0] = 0x00010203;
    testKey[1] = 0x04050607;
    testKey[2] = 0x08090a0b;
    testKey[3] = 0x0c0d0e0f;

    //This sets all the empty spaces to, well, spaces.
    key[0] = 0x30303030;
    key[1] = 0x30303030;
    key[2] = 0x30303030;
    key[3] = 0x30303030;

    if(argc > 1) inputFile = fopen(argv[1],"r");
    else inputFile = fopen("Encrypted.txt","r");

    if(argc > 2)outputFile = fopen(argv[2],"w+");
    else outputFile = fopen("Decrypted.txt","w+");

    if(argc > 3) keyFile = fopen(argv[3],"r");
    else keyFile = fopen("key.txt", "r");

    if(argc > 4)outputFileHex = fopen(argv[4],"w+");
    else outputFileHex = fopen("DecryptedHex.txt","w+");

    //writeGFInvTable("AESInvTable.txt");
    writeGFInvTable("AESInvTable.txt");
    invTable = readGFInvTable("AESInvTable.txt");

    //Write the key from the file
    fread(key, sizeof(word), 4, keyFile);

    for(int x = 0; x < BLOCK_SIZE; x++) key[x] = flipBytes(key[x]);

    //Print key, for test reasons
    printf("Key: ");
    printKey(key);

    roundKeys = wordExpansion(key, invTable);

    /*for(int x = 0; x < 0x100; x++){
        printf("%d ",subBytesDecrypt(x, invTable));
    }*/

    while(keepRunning == 1){

        sizeOfRead = fread(currentRead, sizeof(word), BLOCK_SIZE, inputFile);
        //printCurrentRead(currentRead);
        //printf("%ld\n", ftell(inputFile));

        for(int x = 0; x < BLOCK_SIZE; x++) currentRead[x] = flipBytes(currentRead[x]);

        for(int x = 0; x < BLOCK_SIZE; x++){
            currentRead[x] = currentRead[x] ^ roundKeys[40 + x];
        }

        /*for(int x = 0; x < ROUNDS * 4; x++){
            printWord(roundKeys[x]);
        }*/

        for(int round = ROUNDS - 2; round >= 0; round--){
            //First, we inverse shift the rows

            currentRead = flipRows(currentRead);

            shiftRowsDecrypt(currentRead);

            currentRead = flipRows(currentRead);

            //Second, We inverse substitute the bytes in GF(2^8)

            subBytesAllDecrypt(currentRead,invTable);

            //Third, we XOR the currentRead with the current round key

            currentRead[0] ^= roundKeys[(round * 4)];
            currentRead[1] ^= roundKeys[(round * 4) + 1];
            currentRead[2] ^= roundKeys[(round * 4) + 2];
            currentRead[3] ^= roundKeys[(round * 4) + 3];

            //Finally, we mix the columns

            currentRead = flipRows(currentRead);
            if(round > 0)mixColumnsDecrypt(currentRead);
            currentRead = flipRows(currentRead);

        }

        //If you're asking about why I'm flipping the bits, it's because little and big endian is reeeeeally annoying
        //printCurrentRead(currentRead);
        currentRead[BLOCK_SIZE - 1] = flipBytes(currentRead[BLOCK_SIZE - 1]);

        if(fgetc(inputFile) == EOF) {
          finalSizeOfWrite = currentRead[BLOCK_SIZE - 1];
          //printf("End of file reached\n");
          keepRunning = 0;

        } else fseek(inputFile, -1, SEEK_CUR);
        currentRead[BLOCK_SIZE - 1] = flipBytes(currentRead[BLOCK_SIZE - 1]);

        for(int x = 0; x < BLOCK_SIZE; x++){
          writeWord(outputFileHex, currentRead[x]);
          currentRead[x] = flipBytes(currentRead[x]);
        }

        for(int x = 0; x < finalSizeOfWrite; x++){
          //printf("%x\n", currentRead[x]);
          fwrite((char*)currentRead + x, sizeof(char), 1, outputFile);
        }
        //printCurrentRead(currentRead);
    }


    fclose(inputFile);
    fclose(outputFile);
    free(invTable);
    free(roundKeys);
    free(currentRead);
    free(key);

    return 0;
}
