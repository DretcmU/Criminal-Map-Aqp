import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from sklearn.neighbors import NearestNeighbors
from sklearn.cluster import DBSCAN
from hilbertcurve.hilbertcurve import HilbertCurve

# -------------------- CONFIGURACIÓN --------------------
csv_path = 'processed_data_subset_500k.csv'
columnas_usar = ["trip_distance", "fare_amount", "total_amount"]
num_bits = 10  # Rango [0, 1023]
minPts = 5
# ------------------------------------------------------

# 1. Cargar datos
df = pd.read_csv(csv_path)
df = df[columnas_usar].dropna()

# 2. Eliminar outliers extremos
for col in columnas_usar:
    q1 = df[col].quantile(0.01)
    q99 = df[col].quantile(0.99)
    df = df[(df[col] >= q1) & (df[col] <= q99)]

# 3. Escalar a [0, 2^bits - 1] para la curva de Hilbert
scaler = MinMaxScaler(feature_range=(0, 2**num_bits - 1))
scaled = scaler.fit_transform(df).astype(int)

# 4. Calcular índice Hilbert
dimension = scaled.shape[1]
hilbert = HilbertCurve(num_bits, dimension)
indices_hilbert = np.array([
    hilbert.distance_from_point(p.tolist()) for p in scaled
]).reshape(-1, 1)

# 5. k-distancias
neigh = NearestNeighbors(n_neighbors=minPts)
neigh.fit(indices_hilbert)
distancias, _ = neigh.kneighbors(indices_hilbert)
dist_k = np.sort(distancias[:, -1])  # distancia al k-ésimo vecino

# 6. Estimar mejor ε
dy = np.gradient(dist_k)
ddy = np.gradient(dy)
curvatura = np.abs(ddy)
mejor_indice = np.argmax(curvatura)
mejor_eps = dist_k[mejor_indice]

# 7. Mostrar gráfico del codo
plt.figure(figsize=(10, 5))
plt.plot(dist_k, label=f'Distancia al {minPts}-ésimo vecino')
plt.axvline(mejor_indice, color='red', linestyle='--', label=f'ε ≈ {mejor_eps:.4f}')
plt.title('Gráfico del codo para estimar ε')
plt.xlabel('Puntos ordenados')
plt.ylabel('Distancia')
plt.legend()
plt.grid(True)
plt.show()

print(f"✅ Mejor valor estimado de ε: {mejor_eps:.6f}")

# 8. Aplicar DBSCAN
db = DBSCAN(eps=mejor_eps, min_samples=minPts)
labels = db.fit_predict(indices_hilbert)

# 9. Visualización de clusters (1D)
plt.figure(figsize=(10, 4))
plt.scatter(indices_hilbert, np.zeros_like(indices_hilbert), c=labels, cmap='tab10', s=2)
plt.title('Clusters usando índice Hilbert')
plt.yticks([])
plt.xlabel('Índice Hilbert')
plt.grid(True)
plt.show()

# 10. Resumen
n_clusters = len(set(labels)) - (1 if -1 in labels else 0)
n_noise = list(labels).count(-1)
print(f"🧠 Clusters encontrados: {n_clusters}")
print(f"🔸 Puntos como ruido: {n_noise} de {len(labels)}")
