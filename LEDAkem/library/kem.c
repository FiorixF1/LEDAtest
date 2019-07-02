/**
 *
 * <kem.c>
 *
 * @version 2.0 (March 2019)
 *
 * Reference ISO-C11 Implementation of the LEDAcrypt KEM cipher using GCC built-ins.
 *
 * In alphabetical order:
 *
 * @author Marco Baldi <m.baldi@univpm.it>
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Franco Chiaraluce <f.chiaraluce@univpm.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 * @author Paolo Santini <p.santini@pm.univpm.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#include "niederreiter_keygen.h"
#include "niederreiter_encrypt.h"
#include "niederreiter_decrypt.h"
#include "rng.h"
#include "sha3.h"
#include <string.h>

#if CPU_WORD_BITS == 32

#if PADDING_LENGTH == 0
void convert_array_32_to_64(unsigned char *error_vector, size_t length) {
    uint32_t high, low, *ptr = error_vector;
    
    for (int i = 0; i < (length >> 2); i += 2) {
        high = ptr[i];
        low = ptr[i+1];
        
        ptr[i] = low;
        ptr[i+1] = high;
    }
}
void convert_array_64_to_32(unsigned char *error_vector, size_t length) {
    convert_array_32_to_64(error_vector, length);
}
#elif PADDING_LENGTH == 1
void convert_array_32_to_64(unsigned char *error_vector, size_t length) {
    // how many polynomials are contained in the array (for example 1, N0-1, N0...)
    int blocks = length/(NUM_DIGITS_GF2X_ELEMENT_REFERENCE*DIGIT_SIZE_B);
    // how many 32-bit words are contained in the array
    int words = length >> 2;
    
    uint32_t *ptr = error_vector;
    
    for (int curr_block = blocks; curr_block > 0; --curr_block) {
        int i = words - blocks - (blocks-curr_block)*NUM_DIGITS_GF2X_ELEMENT - 1;
        for (int x = 0; x < NUM_DIGITS_GF2X_ELEMENT-1; x += 2) {
            ptr[i+curr_block-1] = ptr[i];
            ptr[i+curr_block] = ptr[i-1];
            i -= 2;
        }
        ptr[i+curr_block-1] = ptr[i];
    }
    
    for (int curr_block = 0; curr_block < blocks; ++curr_block) {
        ptr[curr_block*NUM_DIGITS_GF2X_ELEMENT_REFERENCE+1] = 0;
    }
}
void convert_array_64_to_32(unsigned char *error_vector, size_t length) {
    // how many polynomials are contained in the array (for example 1, N0-1, N0...)
    int blocks = length/(NUM_DIGITS_GF2X_ELEMENT_REFERENCE*DIGIT_SIZE_B);
    // how many 32-bit words are contained in the array
    int words = length >> 2;
    
    uint32_t *ptr = error_vector;

    for (int curr_block = 1; curr_block <= blocks; ++curr_block) {
        int i = (curr_block-1)*NUM_DIGITS_GF2X_ELEMENT;
        ptr[i] = ptr[i+curr_block-1];
        ++i;
        
        for (int x = 0; x < NUM_DIGITS_GF2X_ELEMENT-1; x += 2) {
            ptr[i] = ptr[i+curr_block+1];
            ptr[i+1] = ptr[i+curr_block];
            i += 2;
        }
    }
}
#else
#error Everything is messed up
#endif

#endif  // CPU_WORD_BITS

/* Generates a keypair - pk is the public key and sk is the secret key. */
int crypto_kem_keypair( unsigned char *pk,
                        unsigned char *sk )
{

   AES_XOF_struct niederreiter_keys_expander;
   randombytes( ((privateKeyNiederreiter_t *)sk)->prng_seed,
                TRNG_BYTE_LENGTH);
   seedexpander_from_trng(&niederreiter_keys_expander,
                          ((privateKeyNiederreiter_t *)sk)->prng_seed);
   key_gen_niederreiter((publicKeyNiederreiter_t *) pk,
                        (privateKeyNiederreiter_t *) sk,
                        &niederreiter_keys_expander);
   return 0;
}

/* Encrypt - pk is the public key, ct is a key encapsulation message
  (ciphertext), ss is the shared secret.*/
int crypto_kem_enc( unsigned char *ct,
                    unsigned char *ss,
                    const unsigned char *pk )
{

   AES_XOF_struct niederreiter_encap_key_expander;
   unsigned char encapsulated_key_seed[TRNG_BYTE_LENGTH];
   randombytes(encapsulated_key_seed,TRNG_BYTE_LENGTH);
   seedexpander_from_trng(&niederreiter_encap_key_expander,encapsulated_key_seed);

   DIGIT error_vector[N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE)];
   rand_circulant_blocks_sequence(error_vector,
                                  NUM_ERRORS_T,
                                  &niederreiter_encap_key_expander);

   #if CPU_WORD_BITS == 32
   convert_array_32_to_64(error_vector, N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE*DIGIT_SIZE_B));
   #endif
   HASH_FUNCTION((const unsigned char *) error_vector,    // input
                 (N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE)*DIGIT_SIZE_B), // input Length
                 ss);
   #if CPU_WORD_BITS == 32
   convert_array_64_to_32(error_vector, N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE*DIGIT_SIZE_B));
   #endif
   
   encrypt_niederreiter((DIGIT *) ct,(publicKeyNiederreiter_t *) pk, error_vector);
   return 0;
}


/* Decrypt - ct is a key encapsulation message (ciphertext), sk is the private
   key, ss is the shared secret */

int crypto_kem_dec( unsigned char *ss,
                    const unsigned char *ct,
                    const unsigned char *sk )
{
   DIGIT decoded_error_vector[N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE)];

   int decode_ok = decrypt_niederreiter(decoded_error_vector,
                                        (privateKeyNiederreiter_t *)sk,
                                        (DIGIT *)ct);
   
   #if CPU_WORD_BITS == 32
   convert_array_32_to_64(decoded_error_vector, N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE*DIGIT_SIZE_B));
   #endif
   
   HASH_FUNCTION((const unsigned char *) decoded_error_vector,
                    (N0*(NUM_DIGITS_GF2X_ELEMENT_REFERENCE)*DIGIT_SIZE_B),
                    ss);
   if (decode_ok == 1) {
      return 0;
   }
   return 1;
}
