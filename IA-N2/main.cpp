#include <iostream>
#include "Miller-Rabin.h"
#include <fstream>
#include <cmath>
#include <vector>
#include <bitset>
#include <cstdint>
#include <chrono>

// Extended Euclidean Algorithm for BigInt
void EXTENDED_EUCLID(bigint a, bigint b, bigint &d, bigint &x, bigint &y) {
    if (b == bigint(0)) {
        d = a;
        x = 1;
        y = 0;
    } else {
        bigint n_d, n_x, n_y;

        // Recursive call and adjust modulo for negative results
        EXTENDED_EUCLID(b, (a % b + b) % b, n_d, n_x, n_y);

        d = n_d;
        x = n_y;
        y = n_x - (a / b) * n_y;
    }
}

bigint findE(bigint phi) {
    bigint e, d, x, y;
    do {

        e = RANDOM(1, bigint(phi));
        if (e % bigint(2) == bigint(0)) e += 1;

        EXTENDED_EUCLID(e, phi, d, x, y);
    } while (d != bigint(1));
    return e;
}

// Modular Linear Equation Solver
bigint MODULAR_LINEAR_EQUATION_SOLVER(bigint a, bigint b, bigint n) {
    bigint d, x, y;
    EXTENDED_EUCLID(a, n, d, x, y);

    // Check if solution exists
    if (b % d == bigint(0)) {
        // Compute initial solution x0, ensuring it is non-negative
        bigint x0 = (x * (b / d)) % n;
        if (x0 < bigint(0)) {
            x0 = (x0 + n) % n;  // Ensure x0 is non-negative
        }

        return x0;
    } else {
        std::cout << "No solutions" << std::endl;
        return -1;
    }
}

void saveKeysToFile(bigint e, bigint d, bigint n) {
    // Shrani javni ključ v pubkey.txt
    std::ofstream pubkeyFile("pubkey.txt");
    if (pubkeyFile.is_open()) {
        pubkeyFile << e << " " << n << std::endl;
        pubkeyFile.close();
    } else {
        std::cerr << "Ne morem odpreti pubkey.txt za pisanje." << std::endl;
    }

    // Shrani zasebni ključ v privkey.txt
    std::ofstream privkeyFile("privkey.txt");
    if (privkeyFile.is_open()) {
        privkeyFile << d << " " << n << std::endl;
        privkeyFile.close();
    } else {
        std::cerr << "Ne morem odpreti privkey.txt za pisanje." << std::endl;
    }
}


void GenKeyPair(int bits) {

    bigint p = MILLER_RABIN(bits, 20);
    bigint q;
    do {
        q = MILLER_RABIN(bits, 20);
    } while (p == q);

    bigint n = p * q;
    bigint phi = (p - bigint(1)) * (q - bigint(1));

    bigint e = findE(phi);

    bigint d = MODULAR_LINEAR_EQUATION_SOLVER(e, 1, phi);
    if (d == bigint(-1)) {
        std::cout << "Error" << std::endl;
        return;
    }

    //Javni ključ P predstavlja par P=(e , n),
    //skrivni ključ S pa par S=(d ,n).

//    std::cout << "Public key: (" << e << ", " << n << ")" << std::endl;
//    std::cout << "Private key: (" << d << ", " << n << ")" << std::endl;

    saveKeysToFile(e, d, n);

    //test 1
//    bigint res = MODULAR_EXPONENTIATION(e*d, 1, phi);
//    std::cout << "Test: " << res << std::endl;

    //test 2
//    bigint res = MODULAR_EXPONENTIATION(bigint("309485009821158151731281920"), e, n);
//    res = MODULAR_EXPONENTIATION(res, d, n);
//    std::cout << "Test: " << res << std::endl;

}

void loadPublicKey(bigint &e, bigint &n) {
    std::ifstream pubkeyFile("pubkey.txt");
    if (pubkeyFile.is_open()) {
        pubkeyFile >> e >> n;
        pubkeyFile.close();
    } else {
        std::cerr << "Ne morem odpreti pubkey.txt za branje." << std::endl;
    }
}

void loadPrivateKey(bigint &d, bigint &n) {
    std::ifstream privkeyFile("privkey.txt");
    if (privkeyFile.is_open()) {
        privkeyFile >> d >> n;
        privkeyFile.close();
    } else {
        std::cerr << "Ne morem odpreti privkey.txt za branje." << std::endl;
    }
}

std::vector<bigint> writtenBlocks;
std::vector<bigint> readBlocks;


void encryptFile(const std::string &inputFile, const std::string &outputFile) {
    bigint e, n;
    loadPublicKey(e, n);

    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);

    if (in.is_open() && out.is_open()) {
        int bitsIN = stoi(big_log2(n).getString());
        int bitsOUT = bitsIN + 1;

        int BytesIN = (bitsIN / 8);
        int BytesOUT = (bitsOUT / 8);

        std::cout << bitsIN << std::endl;
        std::cout << BytesIN << std::endl;

        std::vector<uint8_t> buffer(BytesIN);
        while (true) {
            in.read(reinterpret_cast<char *>(buffer.data()), BytesIN);
            std::streamsize bytesRead = in.gcount();

            if (bytesRead == 0) {
                break;
            }

            if (bytesRead < BytesIN) {
                std::fill(buffer.begin() + bytesRead, buffer.end(), 0);
            }

            bigint block(0);
            for (int i = 0; i < BytesIN; i++) {
                block = block * bigint(256) + bigint(buffer[i]);
            }

//            std::cout << "Read block: " << block.getString() << std::endl;


            bigint encryptedBlock = MODULAR_EXPONENTIATION(block, e, n);

//            std::cout << "Written block: " << encryptedBlock.getString() << std::endl;
            writtenBlocks.push_back(encryptedBlock);

            writtenBlocks.push_back(block);

            bigint tmp(0);


//            for (int i = BytesOUT - 1; i >= 0; i--) {
//                bigint divisor = big_pow(bigint(256), bigint(i));
//                uint8_t byte = static_cast<uint8_t> (stoi(((encryptedBlock / divisor) % bigint(256)).getString()));
//                out.write(reinterpret_cast<char*>(&byte), 1);
//                tmp = tmp * bigint(256) + bigint(byte);
//
//            }

//            bigint currentBlock(encryptedBlock);
//            std::vector<uint8_t> extractedBytes;
//            for (int i = 0; i < BytesOUT; ++i) {
//                // Extract the least significant byte by using modulo 256
//                uint8_t byte = static_cast<uint8_t>(stoi((currentBlock % bigint(256)).getString()));
//
//                extractedBytes.push_back(byte);
//                // Divide the block by 256 to shift to the next byte
//                currentBlock /= 256;
//            }
//
//            for (int i = BytesOUT - 1; i >= 0; i--) {
//                out.write(reinterpret_cast<char *>(&extractedBytes[i]), 1);
//                tmp = tmp * bigint(256) + bigint(extractedBytes[i]);
//            }


//            if (tmp.getString() != encryptedBlock.getString()) {
//                std::cout << "Error" << std::endl;
//                std::cout << tmp.getString() << " " << encryptedBlock.getString() << std::endl;
//            }

            out << encryptedBlock << " ";
//            break;
        }

        in.close();
        out.close();
    } else {
        std::cerr << "Failed to open file for encryption." << std::endl;
    }
}


void decryptFile(const std::string &inputFile, const std::string &outputFile) {
    bigint d, n;
    loadPrivateKey(d, n);

    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);

    if (in.is_open() && out.is_open()) {
        int bitsOUT = stoi(big_log2(n).getString());
        int bitsIN = bitsOUT + 1;

        int BytesIN = (bitsIN / 8);
        int BytesOUT = (bitsOUT / 8);

        std::vector<uint8_t> buffer(BytesIN);
        while (true) {
//            if (in.eof()) {
//                break;
//            }
//
//            in.read(reinterpret_cast<char *>(buffer.data()), BytesIN);
//            std::streamsize bytesRead = in.gcount();
//
//            if (bytesRead == 0) {
//                break;
//            }
//
//            if (bytesRead < BytesIN) {
//                std::cout << "Padding with zeros" << std::endl;
//                std::fill(buffer.begin() + bytesRead, buffer.end(), 0);
//            }
//
//            bigint block(0);
//            for (int i = 0; i < BytesIN; i++) {
//                block = block * bigint(256) + bigint(buffer[i]);
//            }

            if (in.eof()) {
                break;
            }

            bigint block;
            in >> block;

            if (block.getString() == "0") {
                break;
            }

            std::cout << "Read block: " << block.getString() << std::endl;
            readBlocks.push_back(block);

            bigint decryptedBlock = MODULAR_EXPONENTIATION(block, d, n);

            std::cout << "Written block: " << decryptedBlock.getString() << std::endl;
            readBlocks.push_back(decryptedBlock);

            for (int i = BytesOUT - 1; i >= 0; i--) {
                uint8_t byte = stoi(((decryptedBlock / big_pow(bigint(256), bigint(i))) % bigint(256)).getString());
                out.write(reinterpret_cast<char *>(&byte), 1);
            }
//            out << decryptedBlock<<" ";
//            break;
        }

        in.close();
        out.close();
    } else {
        std::cerr << "Failed to open file for decryption." << std::endl;
    }
}

int main() {
    int bits;
    std::cout << "Vpisi stevilo bitov:  ";
    std::cin >> bits;

//    bigint num = MILLER_RABIN(62, 20);
//    bigint log = big_log2(num);
//    std::cout << log << " bit number: " << num << (MILLER_RABIN_TEST(num, 20) ? " Prime" : " Not Prime") << std::endl;


//    int bits = 61;
    GenKeyPair(bits);

//
    encryptFile("input.txt", "msg.bin");
    std::cout << std::endl << std::endl;
    decryptFile("msg.bin", "dec.txt");
//
//    for (int i = 0; i < writtenBlocks.size(); ++i) {
//        if (writtenBlocks[i].getString() != readBlocks[i].getString()) {
//            std::cout << "Error on: " << i << std::endl;
//            std::cout << writtenBlocks[i].getString() << " " << readBlocks[i].getString() << std::endl;
//        }
//    }


    // Test for key generation time
//    for (int i = 3; i < 16; ++i) {
//        long long totalTime = 0;
//
//        for (int j = 0; j < 100; ++j) {
//            auto start = std::chrono::high_resolution_clock::now();
//            GenKeyPair(i);
//            auto end = std::chrono::high_resolution_clock::now();
//            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//        }
//
//        long long averageTime = totalTime / 100;
//        std::cout << "Average time to generate key with " << i << " bits: " << averageTime << "ms" << std::endl;
//    }

    // Test for encryption and decryption time
//    for (int i = 33; i < 49; ++i) {
//        long long totalTime_enc = 0;
//        long long totalTime_dec = 0;
//
//        bigint n_bits;
//        for (int j = 0; j < 10; ++j) {
//            GenKeyPair(i);
//
//            bigint n, n2, e, d;
//            loadPublicKey(e, n);
//            loadPrivateKey(d, n2);
//
//            n_bits = big_log2(n);
//
//            bigint number = RANDOM(0, n);
//            bigint res;
//
//            if (n.getString() != n2.getString()) throw std::runtime_error("Keys do not match");
//
//            auto start = std::chrono::high_resolution_clock::now();
//            res = MODULAR_EXPONENTIATION(number, e, n);
//            auto end = std::chrono::high_resolution_clock::now();
//            totalTime_enc += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//
//            start = std::chrono::high_resolution_clock::now();
//            res = MODULAR_EXPONENTIATION(res, d, n);
//            end = std::chrono::high_resolution_clock::now();
//            totalTime_dec += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//
//            if (res.getString() != number.getString()) throw std::runtime_error("Encryption/Decryption failed");
//        }
//
//        long long averageTime_enc = totalTime_enc / 10;
//        long long averageTime_dec = totalTime_dec / 10;
//        std::cout << "Average time to encrypt: " << averageTime_enc << "ms and decrypt: " << averageTime_dec
//                  << " ms for n with " << n_bits << " bits" << std::endl;
//    }

    return 0;
}
