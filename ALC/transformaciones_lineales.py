import os
from time import sleep

import numpy as np
import matplotlib.pyplot as plt
import math
import cv2
import timeit
import math
import gdown
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from numba import jit, njit
from PIL import Image

supermario_path = 'https://drive.google.com/uc?id=14SQwOXcNzCseSvHWgpLDwv3-C16JTZpB'
mario_path = 'https://drive.google.com/uc?id=16WybrWHRrxXXIlv9fK7u4kdMYNHWY7H1'

def main_tl():
    print("Transformaciones Lineales!")

    if not os.path.exists("super_mario.jpg"):
        print("Descargando imagencitas de mario")
        gdown.download(supermario_path, 'super_mario.jpg', quiet=False)
        gdown.download(mario_path, 'mario.png', quiet=False)

    imagen_grises_random()
    benchmark_funcs_escalado()


def imagen_grises_random():

    print("Generando una imagen random en grises")

    img = np.random.randint(0,255,(64,64))

    plt.imshow(img, cmap='gray')
    plt.show()

def leer_imagen():
    img = cv2.cvtColor(cv2.imread('super_mario.jpg', cv2.IMREAD_UNCHANGED), cv2.COLOR_BGR2RGB)
    plt.imshow(img, cmap='gray')
    plt.show()

@njit
def escalado_sofisticado(image, factor):
    w, h = image.shape[0], image.shape[1]
    new_w, new_h = int(w*factor), int(h*factor)

    size = (new_w, new_h, image.shape[2])

    output = np.zeros(size, dtype=np.uint8)

    scale_matrix = np.array([[1/factor,0],[0,1/factor]], dtype=np.float32)

    for i in range(0, new_w):
        for j in range(0, new_h):
            old_x, old_y = (scale_matrix@np.array([i, j], dtype=np.float32)).flatten().astype(np.int32)
            output[i, j] = image[old_x, old_y]

    return output

@jit(nopython=True)
def escalado_clasico(image, factor):
    w, h = image.shape[0], image.shape[1]
    new_w, new_h = int(w*factor), int(h*factor)

    size = (new_w, new_h, image.shape[2])

    output = np.zeros(size, dtype=np.uint8)

    for i in range(0, new_w):
        for j in range(0, new_h):
            old_x, old_y = int(i / factor), int(j / factor)
            output[i, j] = image[old_x, old_y]

    return output

def benchmark_funcs_escalado():
    print("Escalando una imagen")

    image = cv2.cvtColor(cv2.imread("super_mario.jpg", cv2.IMREAD_UNCHANGED), cv2.COLOR_BGR2RGB)

    factor = 0.1

    benchmark_escalado_sofisticado = timeit.timeit(
        stmt=lambda: escalado_sofisticado(image, factor),
        number=5)

    print(f"Tiempo promedio de escalado sofisticado: {benchmark_escalado_sofisticado/5:.5f}s")

    benchmark_escalado_clasico = timeit.timeit(
        stmt=lambda: escalado_clasico(image, factor),
        number=5
    )

    print(f"Tiempo promedio de escalado clasico: {benchmark_escalado_clasico/5:.5f}s")

    scaled_image = escalado_clasico(image, 0.1)
    plt.imshow(scaled_image)
    plt.show()

def proyectarPts(T, wz):
    assert (T.shape == (2, 2))  # chequeo de matriz 2x2
    assert (T.shape[1] == wz.shape[0])  # multiplicacion matricial valida
    xy = None
    ############### Insert code here!! ######################3

    ############### Insert code here!! ######################3
    return xy


def pointsGrid(corners):
    # crear 10 lineas horizontales
    [w1, z1] = np.meshgrid(np.linspace(corners[0, 0], corners[1, 0], 46),
                           np.linspace(corners[0, 1], corners[1, 1], 10))

    [w2, z2] = np.meshgrid(np.linspace(corners[0, 0], corners[1, 0], 10),
                           np.linspace(corners[0, 1], corners[1, 1], 46))

    w = np.concatenate((w1.reshape(1, -1), w2.reshape(1, -1)), 1)
    z = np.concatenate((z1.reshape(1, -1), z2.reshape(1, -1)), 1)
    wz = np.concatenate((w, z))

    return wz


def vistform(T, wz, titulo=''):
    # transformar los puntos de entrada usando T
    xy = proyectarPts(T, wz)
    if xy is None:
        print('No fue implementada correctamente la proyeccion de coordenadas')
        return
    # calcular los limites para ambos plots
    minlim = np.min(np.concatenate((wz, xy), 1), axis=1)
    maxlim = np.max(np.concatenate((wz, xy), 1), axis=1)

    bump = [np.max(((maxlim[0] - minlim[0]) * 0.05, 0.1)),
            np.max(((maxlim[1] - minlim[1]) * 0.05, 0.1))]
    limits = [[minlim[0] - bump[0], maxlim[0] + bump[0]],
              [minlim[1] - bump[1], maxlim[1] + bump[1]]]

    fig, (ax1, ax2) = plt.subplots(1, 2)
    fig.suptitle(titulo)
    grid_plot(ax1, wz, limits, 'w', 'z')
    grid_plot(ax2, xy, limits, 'x', 'y')


def grid_plot(ax, ab, limits, a_label, b_label):
    ax.plot(ab[0, :], ab[1, :], '.')
    ax.set(aspect='equal',
           xlim=limits[0], ylim=limits[1],
           xlabel=a_label, ylabel=b_label)


def main():
    print('Ejecutar el programa')
    # generar el tipo de transformacion dando valores a la matriz T
    T = pd.read_csv('T.csv', header=None).values
    corners = np.array([[0, 0], [100, 100]])
    # corners = np.array([[-100,-100],[100,100]]) array con valores positivos y negativos
    wz = pointsGrid(corners)
    vistform(T, wz, 'Deformar coordenadas')


if __name__ == "__main__":
    main()

