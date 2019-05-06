#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMMUTABLE_POLYNOMIAL 0x11b
#define GENERATOR 0x03
#define C_BYTE 0x63
#define D_BYTE 0x05
#define ROUNDS 11
#define BLOCK_SIZE 4

typedef unsigned char byte;
typedef unsigned int word;

byte GFMult(byte, byte);
byte GFExp(byte, byte);
byte GFFastMult(byte, byte, byte*, byte*);
byte GFInv(byte, byte*);
byte byteRotateLeft(byte, byte);
byte byteRotateRight(byte, byte);
word wordRotateLeft(word, byte);
word wordRotateRight(word, byte);
word* flipRows(word*);
word flipBytes(word);

byte* createGFExpTable(byte);
byte* createGFLogTable(byte);
byte* createGFInvTable(byte*, byte*);
byte* createGFInvTableBruteForce();

void printGFTable(byte*);
void printWord(word);
void printWordASCII(word);
void printByte(byte);
void printKey(word*);
void printCurrentRead(word*);
void printCurrentReadSquare(word*);
void printCurrentReadASCII(word*);
void writeGFInvTable(char*);
byte* readGFInvTable(char*);
void writeWord(FILE*, word);

byte subBytes(byte, byte*);
byte subBytesDecrypt(byte, byte*);
word subBytesWord(word, byte*);
word subBytesWordDecrypt(word, byte*);
void subBytesAll(word*, byte*);
void subBytesAllDecrypt(word*, byte*);
void shiftRows(word*);
void shiftRowsDecrypt(word*);
void mixColumns(word*);
void mixColumnsDecrypt(word*);

word* wordExpansion(word*, byte*);
word getRoundKey(word*, byte);
word g(word, byte, byte*);
byte roundConstant(byte);

byte getBit(byte, byte);
byte setBit(byte, byte, byte);
byte getByte(word, byte);
word setByte(word, byte, byte);
