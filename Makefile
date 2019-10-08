all: AESEncryption.o AESDecryption.o AES.o
	gcc -o AESEncryption AESEncryption.o AES.o
	gcc -o AESDecryption AESDecryption.o AES.o

encryption: AESEncryption.o
	gcc -o AESEncryption AESEncryption.o AES.o

decryption: AESDecryption.o
	gcc -o AESDecryption AESDecryption.o AES.o

AESEncryption.o: AESEncryption.c AES.c AES.h
	gcc -c AESEncryption.c AES.c

AESDecryption.o: AESDecryption.c AES.c AES.h
	gcc -c AESDecryption.c AES.c

run_encrypt:
	./AESEncryption

run_decrypt:
	./AESDecryption

clean:
	rm *.o
