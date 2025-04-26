import numpy as np

def main():
    print(f"Vamos a hacer cosas raras con matrices")
    hacer_muchas_lu()

def hacer_muchas_lu():

    L = np.array([[1,0,0,0,0,0], [5,1,0,0,0,0], [8,5,1,0,0,0], [-2,3,2,1,0,0], [4,-6,-2,1,1,0], [2,-4,-3,-2,4,1]])
    U = np.array([[8,5,3,1,5,20], [0,1,5,23,5,4], [0,0,1,5,2,3], [0,0,0,2,35,20], [0,0,0,0,20,20], [0,0,0,0,0,101]])

    L1, U1 = calculaLU(L @ U)

    assert np.allclose(L, L1)
    assert np.allclose(U, U1)

    print("Todo en orden")

def calculaLU(matriz):
    dim = len(matriz)

    L = np.identity(dim)

    U = np.array([row[:] for row in matriz])

    for i in range(dim):
        for j in range(i+1, dim):

            if(U[i][i] == 0):
                raise ZeroDivisionError("No se puede calcular LU porque la diagonal es nula")

            factor = U[j][i] / U[i][i]

            L[j][i] = factor

            for k in range(dim):
                U[j][k] -= factor * U[i][k]

    return L, U

def invertir_lu(L, U):
    dim = len(L)

    l_inv = invertir_triangular_inferior(L)
    u_inv = invertir_triangular_superior(U)

    return u_inv @ l_inv

def invertir(m):
    l,u = calculaLU(m)
    return invertir_lu(l, u)

def invertir_triangular_superior(u):
    u_t = np.transpose(u)
    u_inv = np.transpose(invertir_triangular_inferior(u_t))
    return u_inv

def invertir_triangular_inferior(l):
    n = len(l)
    inv = np.identity(n)
    for i in range(n):
        for j in range(i + 1):
            sum = 0
            for k in range(j, i):
                sum += l[i][k] * inv[k][j]

            #No asumimos que la l sea unitaria asi que dividimos por el elemento de la diagonal para normalizar
            inv[i][j] = (1 if i == j else 0 - sum) / l[i][i]

    return inv