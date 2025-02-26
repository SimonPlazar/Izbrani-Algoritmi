import numpy as np


def read_data(file_path):
    with open(file_path, 'r') as file:
        n, m = map(int, file.readline().split())
        A = np.zeros((2 * m, 2 * m))

        file.readline()  # Preskoči prazno vrstico

        # koeficiente v omejitvah
        for i in range(2 * m):
            A[i] = list(map(float, file.readline().split()))

        file.readline()  # Preskoči prazno vrstico

        # desne strani neenačb
        b = list(map(float, file.readline().split()))

        file.readline()  # Preskoči prazno vrstico

        # koeficienti kriterijske funkcije
        c = list(map(float, file.readline().split()))

    return A, b, c, n, m


def initialize_simplex(b, n, m):
    if min(b) < 0:
        raise ValueError("Zacetna resitev je nedopustna")

    N = list(range(n))  # indeksi neosnovnih spremenljivk
    B = list(range(n, n + m))  # indeksi osnovnih spremenljivk
    v = 0  # začetna vrednost kriterijske funkcije
    return N, B, v


def PIVOT(A, b, c, N, B, v, l, e):
    n_A = np.copy(A)
    n_b = np.copy(b)
    n_c = np.copy(c)

    # posodobi pivot vrstico
    n_b[e] = b[l] / A[l, e]

    for j in N:
        if j != e:
            n_A[e, j] = A[l, j] / A[l, e]
    n_A[e, l] = 1 / A[l, e]

    # posodobi ostale vrstice
    for i in B:
        if i != l:
            n_b[i] = b[i] - A[i, e] * n_b[e]
            for j in N:
                if j != e:
                    n_A[i, j] = A[i, j] - A[i, e] * n_A[e, j]
            n_A[i, l] = -A[i, e] * n_A[e, l]

    # posodobi kriterijsko funkcijo
    v += c[e] * n_b[e]
    for j in N:
        if j != e:
            n_c[j] = c[j] - c[e] * n_A[e, j]
    n_c[l] = -c[e] * n_A[e, l]

    N.remove(e)
    N.append(l)
    B.remove(l)
    B.append(e)

    return n_A, n_b, n_c, N, B, v


def SIMPLEX(A, b, c, n, m):
    N, B, v = initialize_simplex(b, n, m)

    while True:
        e = next((j for j in N if c[j] > 0), None)
        if e is None:
            break

        l = None
        min_delta = float('inf')
        for j in B:
            if A[j, e] > 0:
                delta_j = b[j] / A[j, e]
                if delta_j < min_delta:
                    min_delta = delta_j
                    l = j

        if l is None:
            return None, None  # Neomejen problem

        A, b, c, N, B, v = PIVOT(A, b, c, N, B, v, l, e)

    x = np.zeros(len(c))
    for i in B:
        x[i] = b[i]

    return x, v


if __name__ == '__main__':
    file_path = "lprogram.txt"

    # pot do datoteke
    in_str = input("Vnesi pot do datoteke: ")
    if in_str != "":
        file_path = in_str

    A, b, c, n, m = read_data(file_path)
    x, z = SIMPLEX(A, b, c, n, m)
    if x is not None:
        for i in range(len(x)):
            print(f"x{i + 1}: {round(x[i])}")

        print(f"\nz: {round(z)}")
    else:
        print("Problem je neomejen")
