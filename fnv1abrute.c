// compile: gcc -Wall -Werror -Ofast fnv1abrute.c -o fnv1abrute

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define FNV1_PRIME	((u_int64_t) 0x100000001b3)
#define FNV1_START	((u_int64_t) 0xcbf29ce484222325)
#define FNV1_XORKEY	((u_int64_t) 0x0000000000000000) // FIXME: change xor key
#define MAXLEN 6

static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz2346._-";
#define FIRSTCHAR (alphabet[0])

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(0[a]))

u_int64_t hashes[] = { // FIXME: insert xored hashes here
	1111111111111111111UL,
	2222222222222222222UL
};

u_int64_t fnv1a_hash(unsigned char *buffer, size_t size) {
	size_t i;
	u_int64_t hash = FNV1_START;

	for (i = 0; i < size; ++i) {
		hash ^= buffer[i];
		hash *= FNV1_PRIME;
	}

	return hash;
}

int main(int argc, char *argv[]) {
	int i, k;
	char s[MAXLEN+1], c;
	u_int64_t hash[MAXLEN+1];
	unsigned char bytemap[256];

	// unxor target hashes
	for (k = 0; k < ARRAY_SIZE(hashes); k ++) hashes[k] ^= FNV1_XORKEY;

	for (i = 0; i < strlen(alphabet); ++i) bytemap[(u_char) alphabet[i]] = i;

	s[MAXLEN] = 0;
	hash[0] = FNV1_START;

	for (i = 0; i < MAXLEN; ++i) {
		s[i] = FIRSTCHAR;
		hash[i+1] = fnv1a_hash((u_char *) s, i+1);
	}

	// explore hash tree
	for (i = 1; i;) {
		// hash character, output result
		for (i = MAXLEN; i; --i) {
			for (k = 0; k < ARRAY_SIZE(hashes); k ++) {
				if (hash[i] == hashes[k]) {
					c = s[i];
					s[i] = 0;
					printf("%lu  %s\n", hashes[k] ^ FNV1_XORKEY, s);
					s[i] = c;
					break;
				}
			}

			if (bytemap[(u_char) s[i-1]] < strlen(alphabet) - 1) {
				s[i-1] = alphabet[bytemap[(u_char) s[i-1]] + 1];

				hash[i] = hash[i-1];
				hash[i] ^= s[i-1];
				hash[i] *= FNV1_PRIME;

				// update all subsequent hashes
				for (k = i+1; k < MAXLEN+1; ++k) {
					hash[k] = hash[k-1];
					hash[k] ^= s[k-1];
					hash[k] *= FNV1_PRIME;
				}

				break;
			} else {
				s[i-1] = FIRSTCHAR;
			}
		}
	}

	return EXIT_SUCCESS;
}
