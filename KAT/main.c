#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"

// print array of bytes in hex
void printBytes(unsigned char *data, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%02X", data[i]);
    }
}

int main(int argc, char **argv) {  
  char *SEED = "\nseed = ";
  char *PK = "\npk = ";
  char *SK = "\nsk = ";
  char *CT = "\nct = ";
  char *SS = "\nss = ";
  char *ERR_DEC = "\nError in dec";
  int ans;
  
  unsigned char msg[CRYPTO_CIPHERTEXTBYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char ss[CRYPTO_BYTES];
  
  // seed
  unsigned char entropy_string[97];
  strcpy(entropy_string, argv[1]);
    
  unsigned char entropy[48];
  unsigned char *tmp = entropy_string;
  for (size_t i = 0; i < 48; i++) {
    char high = entropy_string[i*2];
    char low = entropy_string[i*2 + 1];
    entropy[i] = 0;
    
    if (high >= '0' && high <= '9') {
        entropy[i] = (high - '0') << 4;
    } else if (high >= 'A' && high <= 'F') {
        entropy[i] = (high - 'A' + 10) << 4;
    } else if (high >= 'a' && high <= 'f') {
        entropy[i] = (high - 'a' + 10) << 4;
    }
    
    if (low >= '0' && low <= '9') {
        entropy[i] |= low - '0';
    } else if (low >= 'A' && low <= 'F') {
        entropy[i] |= low - 'A' + 10;
    } else if (low >= 'a' && low <= 'f') {
        entropy[i] |= low - 'a' + 10;
    }
  }
  randombytes_init(entropy, NULL, 1);
  printf("%s", SEED);
  printBytes(entropy, 48);
   
  // pk and sk
  ans = crypto_kem_keypair(pk, sk);
  
  printf("%s", PK);
  convert_array_32_to_64(pk, CRYPTO_PUBLICKEYBYTES);
  printBytes(pk, CRYPTO_PUBLICKEYBYTES);
  convert_array_64_to_32(pk, CRYPTO_PUBLICKEYBYTES);
  
  printf("%s", SK);
  printBytes(sk, CRYPTO_SECRETKEYBYTES);
  
  // ct and ss
  ans = crypto_kem_enc(msg, ss, pk);
  
  printf("%s", CT);
  convert_array_32_to_64(msg, CRYPTO_CIPHERTEXTBYTES);
  printBytes(msg, CRYPTO_CIPHERTEXTBYTES);
  convert_array_64_to_32(msg, CRYPTO_CIPHERTEXTBYTES);
  
  printf("%s", SS);
  printBytes(ss, CRYPTO_BYTES);
    
  // decryption testing
  ans = crypto_kem_dec(ss, msg, sk);
  if (ans != 0) printf("%s", ERR_DEC);
    
  return 0;
}

