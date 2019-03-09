#include <stdint.h>

void calc_sha_256(uint8_t hash[32], const void *input, size_t len);
void hash_to_string(char string[65], const uint8_t hash[32]);
