all: AESEncryption.o AESDecryption.o AES.o
	gcc -o AESEncryption AESEncryption.o AES.o
	gcc -o AESDecryption AESDecryption.o AES.o

encryption: AESEncryption.o
	gcc -o AESEncryption AESEncryption.o AES.o

decryption: AESDecryption.o
	gcc -o AESDecryption AESDecryption.o AES.o

AESEncryption.o: AESEncryption.c AES.o AES.h
	gcc -c AESEncryption.c

AESDecryption.o: AESDecryption.c AES.o AES.h
	gcc -c AESDecryption.c

AES.o: AES.c AES.h
	gcc -c AES.c

run_encrypt:
	./AESEncryption

run_decrypt:
	./AESDecryption

clean:
	rm *.o
