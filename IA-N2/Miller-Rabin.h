//
// Created by simon on 22. 10. 2024.
//

#ifndef IA_N2_MILLER_RABIN_H
#define IA_N2_MILLER_RABIN_H

#include "bigint.h"

bigint minNumber(int n);
bigint maxNumber(int n);
bigint LCG(bigint m, bigint a, bigint b, bigint R0);
bigint RANDOM(bigint lower, bigint upper);
bool isPrime(bigint n);
bool NAIVNA(bigint n);
bigint MODULAR_EXPONENTIATION(bigint base, bigint exp, bigint mod);
bool MILLER_RABIN_TEST(bigint d, bigint n);
bigint MILLER_RABIN(int n, int k);

#endif //IA_N2_MILLER_RABIN_H
