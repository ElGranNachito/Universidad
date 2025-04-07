import math

import numpy as np

e = np.finfo(np.float32).eps

def run():
    print(f"Epsilon: {e}")
    ej_a()
    ej_b()
    ej_c()
    ef_d()
    ej_e()
    ej_f()
    ej_g()
    ej_h()
    ej_i()
    ej_j()
    ej_k()
    ej_l()

#Tomamos p = 1e34, q = 1, calcular p + q - p
def ej_a():
    print("Ejercicio A")

    p = 1e34
    q = 1

    #Da cero porque 1 es muy pequeño comparado a 1e34 y queda fuera de la precision
    print(f"p + q - p = {p + q - p}")

def ej_b():
    print("Ejercicio B")

    p = 100
    q = 1e-15

    print(f"(p + q) + q = {(p + q) + q}")
    print(f"((p + q) + q) + q = {((p + q) + q) + q}")
    print(f"p + 2q = {p + 2 * q}")
    print(f"p + 3q = {p + 3 * q}")

def ej_c():
    print("Ejercicio C")

    print(f"0.1 + 0.2 == 0.3? {0.1 + 0.2 == 0.3}")

def ef_d():
    print("Ejercicio D")

    print(f"0.1 + 0.3 == 0.4? {0.1 + 0.3 == 0.4}")

def ej_e():
    print("Ejercicio E")

    print(f"1e-323 = {1e-323}")

def ej_f():
    print("Ejercicio F")

    print(f"1e-324 = {1e-324}")

def ej_g():
    print("Ejercicio G")

    print(f"e/2 = {e/2}")

def ej_h():
    print("Ejercicio H")

    print(f"(1 + e/2) + e/2 = {(1+e/2) + e/2}")

def ej_i():
    print("Ejercicio I")

    print(f"1 + (e/2 + e/2) = {1 + (e/2 + e/2)}")

def ej_j():
    print("Ejercicio J")

    print(f"((1 + e/2) + e/2)-1 = {((1+e/2) + e/2)-1}")

def ej_k():
    print("Ejercicio K")

    print(f"(1 + (e/2 + e/2))-1 = {(1 + (e/2 + e/2))-1}")

def ej_l():
    print("Ejercicio L")

    for i in range(1,26):
        print(f"Sen (10^{i}pi) = {math.sin(10**i*math.pi)}")