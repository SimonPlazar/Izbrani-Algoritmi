//
// Created by simon on 17. 10. 2024.
//

#include <cmath>
#include "Miller-Rabin.h"

bigint minNumber(int bits) {
//    return (1 << (bits - 1));
    bigint num(2);
    bigint bits2(bits-1);
    return big_pow(num, bits2);
}

bigint maxNumber(int bits) {
//    return ((1 << bits) - 1);
    bigint num(2);
    bigint bits2(bits);
    return big_pow(num, bits2) - bigint(1);
}

bigint LCG(bigint m, bigint a, bigint b, bigint R0) {
    return (a * R0 + b) % m;
}

bigint r0(1);
const bigint zero(0);
const bigint one(1);
const bigint two(2);

bigint RANDOM(bigint a, bigint b) {
    bigint num(2);
    bigint num2(31);
    r0 = LCG(big_pow(num, num2), bigint(69069), zero, r0);
    return a + r0 % (b - a + one);
}

bool isPrime(bigint p) {
    if (p < two) return false;
    if (p % two == zero) return p == two;

    bigint j(3);
    while (j <= big_sqrt(p)) {
        if (p % j == zero) return false;
        j += 2;
    }

    return true;
}

bigint NAIVNA(int n) {
    //bigint p = RANDOM((bigint) (std::pow(10, n - 1)),(bigint) (std::pow(10, n) - 1));  // Generate a random n-digit number
    bigint p = RANDOM(minNumber(n), maxNumber(n));  // Generate a random n-bit number

    // Make sure p is odd
    if (p % two == zero) {
        p += 1;
    }

    while (true) {
        bigint j(3);

        while (j <= big_sqrt(p)) {
            if (p % j == zero) {
                break;
            }
            j += 2;
        }

        if (j > big_sqrt(p)) {
            return p;
        }

        p += 2;
    }
}

bigint MODULAR_EXPONENTIATION(bigint a, bigint b, bigint n) {
    bigint d(1);

    while (b > zero) {
        if (b % two == one) {
            d = (d * a) % n;
        }
        a = (a * a) % n;
        b /= two;
    }

    return d;
}

bool MILLER_RABIN_TEST(bigint p, bigint s) {
    if (p <= bigint(3)) return true;
    if ((p % two) == zero) return false;

    bigint d = p - one;
    bigint k(0);
    while (d % two == zero) {
        d /= 2;
        k++;
    }

    for (bigint j = 1; j <= s; j++) {
        bigint a = RANDOM(2, p - two);
        bigint x = MODULAR_EXPONENTIATION(a, d, p);
//        bigint x = big_pow(a, d) % p;

        if (x == one || x == (p - one)) {
            continue;
        }

        for (bigint i = 0; i < (k - one); i++) {
            x = MODULAR_EXPONENTIATION(x, 2, p);
//            x = big_pow(x, const_cast<bigint &>(two)) % p;
            if (x == one) {
                return false;
            }
            if (x == p - one)
                break;
        }

        if (x != (p - one)) return false;
    }
    return true;
}

bigint MILLER_RABIN(int n, int s) {
    bigint p = RANDOM(minNumber(n), maxNumber(n) + bigint(1));  // Generate a random n-bit number

    // Make sure p is odd
    if (p % two == zero) {
        p += 1;
    }

    while (!MILLER_RABIN_TEST(p, s)) {
        p += 2;  // Move to the next odd number
    }

    return p;
}