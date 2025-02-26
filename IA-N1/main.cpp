#include <iostream>
#include <cmath>
#include <bitset>
#include <algorithm>

unsigned long long minNumber(int bits) {
    return (1 << (bits - 1));
}

unsigned long long maxNumber(int bits) {

    return ((1 << bits) - 1);

}

unsigned long long LCG(unsigned long long m, unsigned long long a, unsigned long long b, unsigned long long R0) {
    return (a * R0 + b) % m;
}

unsigned long long r0 = 1;

unsigned long long RANDOM(unsigned long long a, unsigned long long b) {
    r0 = LCG((unsigned long long) pow(2, 32), 69069, 0, r0);
    return a + r0 % (b - a + 1);
}

bool isPrime(unsigned long long p) {
    if (p < 2) return false;
    if (p % 2 == 0) return p == 2;

    unsigned long long j = 3;
    while (j <= std::sqrt(p)) {
        if (p % j == 0) return false;
        j += 2;
    }

    return true;
}

unsigned long long NAIVNA(unsigned long long n) {
    //unsigned long long p = RANDOM((unsigned long long) (std::pow(10, n - 1)),(unsigned long long) (std::pow(10, n) - 1));  // Generate a random n-digit number
    unsigned long long p = RANDOM(minNumber(n), maxNumber(n));  // Generate a random n-bit number

    // Make sure p is odd
    if (p % 2 == 0) {
        p += 1;
    }

    while (true) {
        unsigned long long j = 3;

        while (j <= std::sqrt(p)) {
            if (p % j == 0) {
                break;
            }
            j += 2;
        }

        if (j > std::sqrt(p)) {
            return p;
        }

        p += 2;
    }
}

unsigned long long MODULAR_EXPONENTIATION(unsigned long long a, unsigned long long b, unsigned long long n) {
    unsigned long long d = 1;

    std::string bits = std::bitset<32>(b).to_string();

    for (int i = bits.size()-1; i > 0; i--) {
        d = (d * d) % n;
        if (bits[i] == '1') {
            d = (d * a) % n;
        }
    }

//    int bit_length2 = floor(std::log2(b)) + 1;

//    for (int i = bit_length2 - 1; i >= 0; i--) {
//        d = (d * d) % n;
//        if ((b >> i) & 1) {
//            d = (d * a) % n;
//        }
//    }

    return d;
}

bool MILLER_RABIN_TEST(unsigned long long p, unsigned long long s) {
    if (p <= 3) return true;
    if ((p % 2) == 0) return false;

    unsigned long long d = p - 1;
    unsigned long long k = 0;
    while (d % 2 == 0) {
        d /= 2;
        k++;
    }

    for (unsigned long long j = 1; j <= s; j++) {
        unsigned long long a = RANDOM(2, p - 2);
        unsigned long long x = MODULAR_EXPONENTIATION(a, d, p);

        if (x == 1 || x == (p - 1)) {
            continue;
        }

        for (unsigned long long i = 0; i < k; i++) {
            //x = (x * x) % p;
            x = MODULAR_EXPONENTIATION(x, 2, p);
            if (x == 1) {
                return false;
            }
            if (x == p - 1)
                break;
        }

        if (x != (p - 1)) return false;
    }
    return true;
}

unsigned long long MILLER_RABIN(unsigned long long n, int s) {
    //unsigned long long p = RANDOM((unsigned long long) (std::pow(10, n - 1)),(unsigned long long) (std::pow(10, n) - 1));  // Generate a random n-digit number
    unsigned long long p = RANDOM(minNumber(n), maxNumber(n));  // Generate a random n-bit number

    // Make sure p is odd
    if (p % 2 == 0) {
        p += 1;
    }

    while (!MILLER_RABIN_TEST(p, s)) {
        p += 2;  // Move to the next odd number
    }

    return p;
}

#include <chrono>

int main() {
    /*
    for (unsigned long long i = 1; i <= 20; ++i) {
        unsigned long long n = MILLER_RABIN(31, i);
        std::cout << n << " ";
        std::cout << MILLER_RABIN_TEST(n, 20) << (isPrime(n) ? " IS PRIME " : " IS NOT PRIME ")
                  << std::endl;
    }
    */

    /*
    for (int i = 0; i < 20; ++i) {
        double sum = 0;
        for (int j = 0; j < 100; ++j) {
            auto start = std::chrono::high_resolution_clock::now();

            MILLER_RABIN(31, i + 1);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            sum += duration.count();
        }

        std::cout << "Time taken: " << sum / 100 << " seconds" << std::endl;

    }
    */

    /*
    for (int i = 4; i < 32; ++i) {
        double sum1 = 0;
        double sum2 = 0;
        for (int j = 0; j < 100; ++j) {
            auto start1 = std::chrono::high_resolution_clock::now();

            MILLER_RABIN(i, 10);

            auto end1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end1 - start1;
            sum1 += duration.count();

            auto start2 = std::chrono::high_resolution_clock::now();

            NAIVNA(i);

            auto end2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration2 = end2 - start2;
            sum2 += duration2.count();

        }

        std::cout << i << " bit number, Time taken Miller: " << sum1 / 100 << " seconds" << std::endl;
        std::cout << i << " bit number, Time taken Naive: " << sum2 / 100 << " seconds" << std::endl;
    }
    */

    //int option = 0;
    int option = -1;

    while (option != 0) {
        //print menu for user
        std::cout << "---------------------------" << std::endl;
        std::cout << "1. Generate prime number" << std::endl;
        std::cout << "2. Check if number is prime" << std::endl;
        std::cout << "---------------------------" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "---------------------------" << std::endl << std::endl;
        std::cout << "Choose option: ";
        //get user input
        std::cin >> option;

        switch (option) {
            case 1: {
                int n, s;
                std::cout << "Enter number of bits: ";
                std::cin >> n;
                std::cout << "Enter number for s: ";
                std::cin >> s;
                std::cout << "Generated prime number: " << std::endl;
                std::cout << "\tNaive method:" << NAIVNA(n) << std::endl;
                std::cout << "\tMiller-Rabinov: " << MILLER_RABIN(n, s) << std::endl;
                break;
            }
            case 2: {
                unsigned long long p;
                unsigned long long s;
                std::cout << "Enter number: ";
                std::cin >> p;
                std::cout << "Enter number for s: ";
                std::cin >> s;
                std::cout << "Naive: " << p << (isPrime(p) ? " IS PRIME " : " IS NOT PRIME ") << std::endl;
                std::cout << "Miller-Rabin: " << p << (MILLER_RABIN_TEST(p, s) ? " IS PRIME " : " IS NOT PRIME ")
                          << std::endl;
                break;
            }
            case 0: {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
            default: {
                std::cout << "Invalid option!" << std::endl;
                break;
            }
        }
    }

    return 0;
}
