#include <iostream>
#include <utility>
#include <vector>
#include <random>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <boost/multiprecision/cpp_int.hpp>
#include <filesystem>
#include <sstream>
#include <chrono>

using namespace boost::multiprecision;

void Program();

struct Share {
    int x;
    uint32_t fx;
};

struct Share_BigInt {
    int x;
    cpp_int fx;
};

uint32_t random(uint32_t min, uint32_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(gen);
}

std::vector<Share_BigInt> RAZBIJ_BigInt(cpp_int S, int k, int n) {
    std::vector<cpp_int> coefficients(k);
    coefficients[0] = std::move(S);

    // Generiraj naključne koeficiente za polinom
    for (int i = 1; i < k; ++i) {
        coefficients[i] = random(1, UINT32_MAX);
    }

    std::vector<Share_BigInt> shares;
    for (int x = 1; x <= n; ++x) {
        cpp_int fx = 0;
        for (int i = 0; i < k; ++i) {
            fx += coefficients[i] * static_cast<cpp_int>(pow(x, i));
        }
        shares.push_back({x, fx});
//        std::cout << "Dodan kot delez: <" << x << ", " << fx << ">" << std::endl;
    }

    return shares;
}


// Funkcija za razbijanje skrivnosti S na k deležev s pragom k
std::vector<Share> RAZBIJ(uint32_t S, int k, int n) {
    std::vector<uint32_t> coefficients(k);
    coefficients[0] = S;


    // Generiraj naključne koeficiente za polinom
    for (int i = 1; i < k; ++i) {
//        coefficients[i] = random(1, UINT32_MAX);
        coefficients[i] = random(1, UINT8_MAX);
    }


    std::vector<Share> shares;
    // Izračunaj vsak delež f(x) in ga shrani v vektor
    for (int x = 1; x <= n; ++x) {
        uint32_t fx = 0;
        for (int i = 0; i < k; ++i) {
            fx += coefficients[i] * static_cast<uint32_t>(std::pow(x, i));
        }
        shares.push_back({x, fx});
//        std::cout << "<" << x << ", " << fx << "> dodan kot delež." << std::endl;
    }

    return shares;
}

cpp_int REKONSTRUIRAJ_ROBUST_BigInt(const std::vector<Share_BigInt> &D, int k) {
    cpp_int S = 0;
    cpp_int downGlobal = 1;
    for (int j = 0; j < k; ++j) {
        cpp_int inside = 1;
        for (int i = 0; i < k; ++i) {
            if (i != j) {
                inside *= (D[i].x - D[j].x);
            }
        }
        downGlobal *= D[j].fx * inside;
    }

    for (int j = 0; j < k; ++j) {
        cpp_int up = 1;
        cpp_int downLocal = 1;

        for (int i = 0; i < k; ++i) {
            if (i != j) {
                up *= D[i].x;
                downLocal *= (D[j].x - D[i].x);
            }
        }

        S += D[j].fx * up * (downGlobal / downLocal);
    }

    S = S / downGlobal;

    return abs(S);
}

uint32_t REKONSTRUIRAJ_ROBUST(const std::vector<Share> &D, int k) {
    int64_t S = 0;
    int64_t downGlobal = 1;
    for (int j = 0; j < k; ++j) {
        int64_t inside = 1;
        for (int i = 0; i < k; ++i) {
            if (i != j) {
//                downGlobal *= D[j].fx * (D[i].x - D[j].x);
                inside *= (D[i].x - D[j].x);
            }
        }
        downGlobal *= D[j].fx * inside;
    }

    for (int i = 0; i < k; ++i) {
        int64_t up = 1;
        int64_t downLocal = 1;

        for (int j = 0; j < k; ++j) {
            if (j != i) {
                up *= D[j].x;
                downLocal *= (D[i].x - D[j].x);
            }
        }

        S += D[i].fx * up * (downGlobal / downLocal);
    }

    S = S / downGlobal;

    return static_cast<uint32_t>(S);
}

uint32_t REKONSTRUIRAJ(const std::vector<Share> &D, int k) {
    int S = 0;

    for (int i = 0; i < k; ++i) {
        float inner = 1;
        int tmp = D[i].fx;
        for (int j = 0; j < k; ++j) {
            if (j != i)
                inner *= ((float) D[j].x / (D[j].x - D[i].x));
//                tmp *= ((float) D[i].x / (D[i].x - D[j].x));
        }
        S += tmp * inner;
//        S += tmp;
    }

    return S;
}

void getData1() {
    std::pair<int, int> N_K[] = {
            {10, 3},
            {10, 5},
            {20, 3},
            {20, 5},
            {20, 10},
    };

    for (int i = 8; i <= 24; ++i) {
        for (auto pair: N_K) {
            int razbit_bigint = 0;
            int razbit_byte = 0;
            int rekonstruiraj_bigint = 0;
            int rekonstruiraj_byte = 0;

            int N = pair.first;
            int K = pair.second;

            for (int k = 0; k < 10; ++k) {
                cpp_int bigint = 0;
                std::vector<unsigned char> bytes;
                for (int j = 0; j < i; ++j) {
                    unsigned char byte = random(0, UINT8_MAX);
                    bytes.push_back(byte);
                    bigint = bigint * 256 + byte;
                }

                auto start = std::chrono::high_resolution_clock::now();
                auto shares_bigint = RAZBIJ_BigInt(bigint, K, N);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                razbit_bigint += duration.count();

                start = std::chrono::high_resolution_clock::now();
                auto reconstructed = REKONSTRUIRAJ_ROBUST_BigInt(shares_bigint, K);
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                if (bigint != reconstructed) {
                    std::cerr << "Napaka pri rekonstrukciji!" << std::endl;
                    std::cerr << "N: " << N << ", K: " << K << std::endl;
                    std::cerr << "Original: " << bigint << std::endl;
                    std::cerr << "Reconstructed: " << reconstructed << std::endl;
                    return;
                }

                rekonstruiraj_bigint += duration.count();


                std::vector<std::vector<Share_BigInt>> shares_byte;
                start = std::chrono::high_resolution_clock::now();
                for (unsigned char byte: bytes) {
                    shares_byte.push_back(RAZBIJ_BigInt(byte, K, N));
                }
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                razbit_byte += duration.count();

                std::vector<unsigned char> reconstructed_bytes;
                start = std::chrono::high_resolution_clock::now();
                for (int j = 0; j < i; ++j) {
                    reconstructed_bytes.push_back(
                            static_cast<unsigned char>(REKONSTRUIRAJ_ROBUST_BigInt(shares_byte[j], K)));
                }
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                for (int j = 0; j < bytes.size(); ++j) {
                    if (bytes[j] != reconstructed_bytes[j]) {
                        std::cerr << "Napaka pri rekonstrukciji bajtov!" << std::endl;
                        std::cerr << "N: " << N << ", K: " << K << std::endl;
                        std::cerr << "Original: " << bigint << std::endl;
                        std::cerr << "Reconstructed: " << reconstructed << std::endl;

                        return;
                    }
                }

                rekonstruiraj_byte += duration.count();
            }

            razbit_bigint /= 10;
            rekonstruiraj_bigint /= 10;
            razbit_byte /= 10;
            rekonstruiraj_byte /= 10;

            std::cout << "RAZBIJANJE BIGINT: " << i << " bajtov, N = " << N << ", K = " << K << ", cas: "
                      << razbit_bigint
                      << " mikrosekund" << std::endl;
            std::cout << "REKONSTRUKCIJA BIGINT: " << i << " bajtov, N = " << N << ", K = " << K << ", cas: "
                      << rekonstruiraj_bigint << " mikrosekund" << std::endl;
            std::cout << "RAZBIJANJE BYTE: " << i << " bajtov, N = " << N << ", K = " << K << ", cas: "
                      << razbit_byte
                      << " mikrosekund" << std::endl;
            std::cout << "REKONSTRUKCIJA BYTE: " << i << " bajtov, N = " << N << ", K = " << K << ", cas: "
                      << rekonstruiraj_byte << " mikrosekund" << std::endl;
        }
    }

}

void getData2() {
    for (int i = 8; i < 16; ++i) {
        for (int n = 6; n < 15; ++n) {
            for (int k = 3; k < n * 3/4; ++k) {
                uint32_t data = random(0, UINT8_MAX);
                for (int j = 0; j < i; ++j) {
                    data = data * 2 + random(0, 1);
                }

                auto shares = RAZBIJ(data, k, n);
                auto reconstructed = REKONSTRUIRAJ(std::vector<Share>(shares.begin(), shares.begin() + k), k);

                if (data != reconstructed) {
                    std::cerr << "Napaka pri rekonstrukciji!" << std::endl;
                    std::cerr << i+8 << " bits for data," << "N: " << n << ", K: " << k << std::endl;
                    std::cerr << "Original: " << data << std::endl;
                    std::cerr << "Reconstructed: " << reconstructed << std::endl;
//                    return;
                }
            }
        }
    }
}

void Test() {

    uint32_t S = 1234; // Primer skrivnosti
//    cpp_int S = 1234; // Primer skrivnosti

    int k = 3; // Prag
    int n = 6; // Število deležev

    // 1. Korak: Razbij skrivnost na deleže
    auto shares = RAZBIJ(S, k, n);
//    auto shares = RAZBIJ_BigInt(S, k, n);

//    std::vector<Share> shares = {
//            Share{1, 1494},
//            Share{2, 1942},
//            Share{3, 2578},
//            Share{4, 3402},
//            Share{5, 4414},
//            Share{6, 5614}
//    };

    // 2. Korak: Izberi k deležev
    std::vector<Share> selectedShares;
//    std::vector<Share_BigInt> selectedShares;
    selectedShares.push_back(shares[1]);
    selectedShares.push_back(shares[3]);
    selectedShares.push_back(shares[4]);
//    selectedShares.push_back(shares[0]);
//    selectedShares.push_back(shares[1]);
//    selectedShares.push_back(shares[2]);

    // 3. Korak: Rekonstruiraj skrivnost iz izbranih deležev
    uint32_t reconstructedSecret = REKONSTRUIRAJ(selectedShares, k);
//    uint32_t reconstructedSecret = REKONSTRUIRAJ_ROBUST(selectedShares, k);
//    auto reconstructedSecret = REKONSTRUIRAJ_ROBUST_BigInt(selectedShares, k);

    std::cout << "Secrets match: " << (S == reconstructedSecret) << std::endl;
    std::cout << "Original secret S: " << S << std::endl;
    std::cout << "Reconstructed secret S: " << reconstructedSecret << std::endl;

}


int main() {

    Program();
//    getData1();
//    getData2();
//    Test();

    return 0;

}

void Razbi_BigInt(int k, int n, const std::string &vhodnaDatoteka, const std::string &predpona_izhodnih) {
    //predstavitev celotne datoteke z BigInt in predstavitev
    // enega deleža s števili x in y (fx)
    std::ifstream in(vhodnaDatoteka, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
        return;
    }

    std::string data;
    std::getline(in, data, '\0');
    in.close();

    cpp_int block = 0;
    for (char byte: data) {
        block = block * 256 + static_cast<unsigned char>(byte);
    }

    std::cout << "Prebrana skrivnost: " << block << std::endl;

    auto shares = RAZBIJ_BigInt(block, k, n);

    // shrani deleže v datoteke
    for (int i = 0; i < n; ++i) {
        std::ofstream out(predpona_izhodnih + std::to_string(i) + ".txt");
        if (out.is_open()) {
            out << shares[i].x << " " << shares[i].fx << std::endl;
            out.close();
        } else {
            std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
            return;
        }
    }
}

void Rekonstrukcija_BigInt(const std::string &predpona_vhodnih, const std::string &izhodnaDatoteka) {
    std::vector<Share_BigInt> shares;
    int k = 0;

    try {
        // Poiščemo zadnji delimiter '/' in ločimo pot in predpono
        size_t zadnja_pot = predpona_vhodnih.rfind('/');
        std::string pot = predpona_vhodnih.substr(0, zadnja_pot);
        std::string predpona = predpona_vhodnih.substr(zadnja_pot + 1);

        std::cout << "Iskanje v direktoriju: " << pot << " z predpono: " << predpona << std::endl;

        // Preverimo vse datoteke v direktoriju
        for (const auto &entry: std::filesystem::directory_iterator(pot)) {
            if (std::filesystem::is_regular_file(entry.status())) {
                // Preverimo, ali ime datoteke začne s predpono
                if (entry.path().filename().string().rfind(predpona, 0) == 0) {
                    std::cout << "Odprta datoteka: " << entry.path().string() << std::endl;

                    std::ifstream datoteka(entry.path());
                    if (datoteka.is_open()) {
                        int x;
                        cpp_int fx;
                        datoteka >> x >> fx;
                        shares.push_back({x, fx});
                        datoteka.close();
                        k++;
                        std::cout << "Prebran delez: <" << x << ", " << fx << ">" << std::endl;

                        datoteka.close();
                    } else {
                        std::cerr << "Napaka pri odpiranju datoteke: " << entry.path().string() << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Napaka pri iskanju datotek: " << e.what() << std::endl;
    }

    cpp_int reconstructedSecret = REKONSTRUIRAJ_ROBUST_BigInt(shares, k);

    std::cout << "Rekonstruirana skrivnost: " << reconstructedSecret << std::endl;

    std::ofstream out(izhodnaDatoteka, std::ios::binary);

    if (out.is_open()) {

        std::string result;

        while (reconstructedSecret != 0) {
            unsigned char znak = static_cast<unsigned char>(reconstructedSecret % 256);
            result.insert(result.begin(), znak);
            reconstructedSecret /= 256;
        }

        out << result;

        out.close();
    } else {
        std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
        return;
    }
}

void Razbi_Byte(int k, int n, const std::string &vhodnaDatoteka, const std::string &predpona_izhodnih) {
    std::ifstream in(vhodnaDatoteka, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
        return;
    }

    std::vector<unsigned char> bytes;

    while (!in.eof()) {
        unsigned char byte;
        in.read(reinterpret_cast<char *>(&byte), sizeof(byte));

        if (in.gcount() == 0) {
            break;
        }

        bytes.push_back(byte);
    }
    in.close();

    for (int i = 0; i < bytes.size(); ++i) {
        std::cout << "Byte " << i << ": " << (char) bytes[i] << std::endl;
    }

    std::vector<std::vector<Share_BigInt>> shares;

    for (unsigned char byte: bytes) {
        auto sharesByte = RAZBIJ_BigInt(byte, k, n);
        shares.push_back(sharesByte);
    }

    std::cout << "Deleži za 1. byte: " << std::endl;
    for (auto &share: shares[0]) {
        std::cout << "<" << share.x << " " << share.fx << ">" << std::endl;
    }

    // shrani deleže v datoteke
    for (int i = 0; i < n; ++i) {
        std::ofstream out(predpona_izhodnih + std::to_string(i) + ".txt");
        if (out.is_open()) {
            out << shares[0][i].x;

            for (auto &share: shares) {
                out << " " << share[i].fx;
            }

            out.close();
        } else {
            std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
            return;
        }
    }
}

void Rekonstrukcija_Bytes(const std::string &predpona_vhodnih, const std::string &izhodnaDatoteka) {
    std::vector<std::vector<Share_BigInt>> shares;
    int k = 0;

    try {
        // Poiščemo zadnji delimiter '/' in ločimo pot in predpono
        size_t zadnja_pot = predpona_vhodnih.rfind('/');
        std::string pot = predpona_vhodnih.substr(0, zadnja_pot);
        std::string predpona = predpona_vhodnih.substr(zadnja_pot + 1);

        std::cout << "Iskanje v direktoriju: " << pot << " z predpono: " << predpona << std::endl;

        // Preverimo vse datoteke v direktoriju
        for (const auto &entry: std::filesystem::directory_iterator(pot)) {
            if (std::filesystem::is_regular_file(entry.status())) {
                // Preverimo, ali ime datoteke začne s predpono
                if (entry.path().filename().string().rfind(predpona, 0) == 0) {
//                    std::cout << "Odprta datoteka: " << entry.path().string() << std::endl;

                    std::ifstream datoteka(entry.path());
                    if (datoteka.is_open()) {

                        std::string line;
                        std::getline(datoteka, line);

                        std::istringstream iss(line);
                        int x;
                        cpp_int fx;

                        iss >> x;

                        int count = 0;

                        while (iss >> fx) {  // Preberi vse y vrednosti (fx)
                            if (k == 0) shares.emplace_back();
                            shares[count].push_back({x, fx});
                            count++;
                        }

                        k++;
                        datoteka.close();
                    } else {
                        std::cerr << "Napaka pri odpiranju datoteke: " << entry.path().string() << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Napaka pri iskanju datotek: " << e.what() << std::endl;
    }

//    std::cout<< "Delezi za 1. byte: " << std::endl;
//    for(auto &share: shares[0]) {
//        std::cout << "<" << share.x << " " << share.fx << ">" << std::endl;
//    }

    std::ofstream out(izhodnaDatoteka, std::ios::binary);
    std::string result;

    if (out.is_open()) {

        for (auto &share: shares) {
            auto reconstructedByte = REKONSTRUIRAJ_ROBUST_BigInt(share, k);
//            result.insert(result.end(), static_cast<unsigned char>(reconstructedByte));
            out.put(static_cast<unsigned char>(reconstructedByte));
        }
//        out << result;

        out.close();
    } else {
        std::cerr << "Napaka pri odpiranju datoteke!" << std::endl;
        return;
    }
}


void Program() {
    while (true) {
        int izbira;
        std::cout << "Podaj opcijo:" << std::endl;
        std::cout << "1. Razbijanje skrivnosti" << std::endl;
        std::cout << "2. Rekonstrukcija skrivnosti" << std::endl;
        std::cout << "----------------------------" << std::endl;
        std::cout << "0. Konec" << std::endl << std::endl;
        std::cout << "Izbira: ";

        std::cin >> izbira;

        int n, k;
        std::string vhodnaDatoteka, predpona_izhodnih;
        int nacin;

        std::string predpona_vhodnih, izhodnaDatoteka;

        switch (izbira) {
            case 1:
                //Uporabnik naj poda: n, k, vhodno datoteko in predpono imena datotek z deleži.
                std::cout << "Vnesite n: ";
                std::cin >> n;
                std::cout << "Vnesite k: ";
                std::cin >> k;
                std::cout << "Vnesite ime vhodne datoteke: ";
                std::cin >> vhodnaDatoteka;
                std::cout << "Vnesite predpono imena datotek z delezi: ";
                std::cin >> predpona_izhodnih;
                std::cout << "Izberite nacin (1. BigInt, 2. Byte): ";
                std::cin >> nacin;

                switch (nacin) {
                    case 1:
                        Razbi_BigInt(k, n, vhodnaDatoteka, predpona_izhodnih);
                        break;
                    case 2:
                        Razbi_Byte(k, n, vhodnaDatoteka, predpona_izhodnih);
                        break;
                    default:
                        std::cout << "Napacna izbira!" << std::endl;
                }

                break;
            case 2:
                //Uporabnik naj v tem primeru poda imena datotek z deleži, in ime izhodne datoteke
                std::cout << "Vnesite predpono imena datotek z delezi: ";
                std::cin >> predpona_vhodnih;
                std::cout << "Vnesite ime izhodne datoteke: ";
                std::cin >> izhodnaDatoteka;

                std::cout << "Izberite nacin (1. BigInt, 2. Byte): " << std::endl;
                std::cin >> nacin;

                switch (nacin) {
                    case 1:
                        Rekonstrukcija_BigInt(predpona_vhodnih, izhodnaDatoteka);
                        break;
                    case 2:
                        Rekonstrukcija_Bytes(predpona_vhodnih, izhodnaDatoteka);
                        break;
                    default:
                        std::cout << "Napacna izbira!" << std::endl;
                }

                break;
            case 0:
                return;
            default:
                std::cout << "Napačna izbira!" << std::endl;
                break;
        }
    }
}