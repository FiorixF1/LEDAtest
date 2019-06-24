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

// print array of bytes in a KAT-compliant way
// more complicated than expected: leading zeros are placed differently in PK and CT based on values of CATEGORY and N0
#if N0 == 2
void printPk(unsigned char *data, int length) {
    char zeros[9] = "00000000";
    char buffer[16];
    
    for (int i = 0; i < 4; ++i) {
        printf("%02X", data[i]);
    }
    printf("%s", zeros);
    
    int swap = 0;
    int i = 8;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}

void printCt(unsigned char *data, int length) {
    printPk(data, length);
}
#elif (N0 == 3) && ((CATEGORY == 1) || (CATEGORY == 2) || (CATEGORY == 3))
void printPk(unsigned char *data, int length) {
    char zeros[9] = "00000000";
    char buffer[16];
    
    for (int i = 0; i < 4; ++i) {
        printf("%02X", data[i]);
    }
    printf("%s", zeros);
    
    int swap = 0;
    int i = 8;
    while (i < length/2) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
    
    for (int j = 0; j < 4; ++j) {
        printf("%02X", data[i-4+j]);
    }
    printf("%s", zeros);
    
    swap = 0;
    i += 4;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}

void printCt(unsigned char *data, int length) {
    char zeros[9] = "00000000";
    char buffer[16];
    
    for (int i = 0; i < 4; ++i) {
        printf("%02X", data[i]);
    }
    printf("%s", zeros);
    
    int swap = 0;
    int i = 8;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}
#elif ((N0 == 3) && ((CATEGORY == 4) || (CATEGORY == 5))) || ((N0 == 4) && ((CATEGORY == 1) || (CATEGORY == 4) || (CATEGORY == 5)))
void printPk(unsigned char *data, int length) {
    int swap = 0;
    int i = 4;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}

void printCt(unsigned char *data, int length) {
    printPk(data, length);
}
#elif (N0 == 4) && ((CATEGORY == 2) || (CATEGORY == 3))
void printPk(unsigned char *data, int length) {
    char zeros[9] = "00000000";
    char buffer[16];
    
    for (int i = 0; i < 4; ++i) {
        printf("%02X", data[i]);
    }
    printf("%s", zeros);
    
    int swap = 0;
    int i = 8;
    while (i <= length/3) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
    
    for (int j = 0; j < 4; ++j) {
        printf("%02X", data[i-4+j]);
    }
    printf("%s", zeros);
    
    swap = 0;
    i += 4;
    while (i < 2*length/3) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
    
    for (int j = 0; j < 4; ++j) {
        printf("%02X", data[i-4+j]);
    }
    printf("%s", zeros);
    
    swap = 0;
    i += 4;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}

void printCt(unsigned char *data, int length) {
    char zeros[9] = "00000000";
    char buffer[16];
    
    for (int i = 0; i < 4; ++i) {
        printf("%02X", data[i]);
    }
    printf("%s", zeros);
    
    int swap = 0;
    int i = 8;
    while (i < length) {
        for (int j = 0; j < 4; ++j) {
            printf("%02X", data[i+j]);
        }
        
        if (swap) {
            i += 12;
            swap = 0;
        } else {
            i -= 4;
            swap = 1;
        }
    }
}
#else
#error Unsupported values for CATEGORY and N0
#endif

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
  printPk(pk, CRYPTO_PUBLICKEYBYTES);
  printf("%s", SK);
  printBytes(sk, CRYPTO_SECRETKEYBYTES);
  
  // ct and ss
  ans = crypto_kem_enc(msg, ss, pk);
  printf("%s", CT);
  printCt(msg, CRYPTO_CIPHERTEXTBYTES);
  printf("%s", SS);
  printBytes(ss, CRYPTO_BYTES);
    
  // decryption testing
  ans = crypto_kem_dec(ss, msg, sk);
  if (ans != 0) printf("%s", ERR_DEC);
    
  return 0;
}

