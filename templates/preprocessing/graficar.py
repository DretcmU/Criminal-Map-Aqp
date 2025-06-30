import matplotlib.pyplot as plt
import numpy as np

# === FUNCIONES HILBERT 2D ===

def hilbert_index_to_xy(h, bits):
    x = y = 0
    n = 1 << bits
    s = 1
    t = h

    for i in range(bits):
        rx = 1 & (t >> 1)
        ry = 1 & (t ^ rx)
        x, y = rot(s, x, y, rx, ry)
        x += s * rx
        y += s * ry
        t >>= 2
        s <<= 1

    return x, y

def rot(n, x, y, rx, ry):
    if ry == 0:
        if rx == 1:
            x = n - 1 - x
            y = n - 1 - y
        x, y = y, x
    return x, y

# === LECTURA DEL TXT ===

def leer_txt(nombre_archivo):
    datos = []
    with open(nombre_archivo, 'r') as f:
        for linea in f:
            partes = linea.strip().split()
            if len(partes) != 2:
                continue
            key = int(partes[0])
            cluster = int(partes[1])
            datos.append((key, cluster))
    return datos

# === GRAFICADO CORREGIDO ===

def graficar_hilbert_2d(datos, bits=16, max_mostrar=100000):
    print("Decodificando Hilbert indices a coordenadas (x, y)...")
    puntos = []
    clusters = []

    for key, cluster in datos[:max_mostrar]:
        x, y = hilbert_index_to_xy(key, bits)
        puntos.append((x, y))
        clusters.append(cluster)

    puntos = np.array(puntos)
    x = puntos[:, 0]
    y = puntos[:, 1]

    # === FILTRAR EXTREMOS POR PERCENTIL ===
    xmin, xmax = np.percentile(x, [1, 99])
    ymin, ymax = np.percentile(y, [1, 99])
    mascara = (x >= xmin) & (x <= xmax) & (y >= ymin) & (y <= ymax)

    x_filtrado = x[mascara]
    y_filtrado = y[mascara]
    clusters_filtrados = np.array(clusters)[mascara]

    clusters_unicos = sorted(set(clusters_filtrados))
    cmap = plt.get_cmap("tab20", len(clusters_unicos))
    cluster_to_color = {c: cmap(i) for i, c in enumerate(clusters_unicos)}
    colores = [cluster_to_color[c] for c in clusters_filtrados]

    plt.figure(figsize=(8, 8))
    plt.scatter(x_filtrado, y_filtrado, c=colores, s=2)
    plt.title("Recorrido Hilbert 2D por Clúster (Filtrado)")
    plt.axis("equal")
    plt.xticks([])
    plt.yticks([])
    plt.grid(False)
    plt.tight_layout()
    plt.show()

# === MAIN ===

if __name__ == "__main__":
    archivo = "BinDatos.bin.txt"
    datos = leer_txt(archivo)
    graficar_hilbert_2d(datos,max_mostrar=100000)