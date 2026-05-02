# R-tree Bulk Loading - CC4102

## Descripción

Este proyecto implementa la construcción y consulta de R-trees utilizando dos algoritmos de bulk-loading:

- **Nearest-X**
- **Sort-Tile-Recursive (STR)**

El objetivo es comparar su rendimiento en:

- Tiempo de construcción
- Número de accesos a disco (I/O)
- Cantidad de puntos retornados en consultas

---

## Compilación

Compilar el proyecto con:

```bash
g++ src/*.cpp -o rtree -O2 -std=c++17
```

---


## Ejecución completa

1. Ejecutar experimentos

```cpp
./rtree
```

Esto:
- Construye R-trees para distintos tamaños
- Ejecuta consultas
- Genera archivos en /results

2. Generar gráficos

```python
cd scripts
python3 plot.py
```

Esto generará:
- build_random.png
- build_europe.png
- io_random.png
- io_europe.png
- points_random.png
- points_europe.png

--- 

## Explicación del Código

A continuación se describe la función de cada archivo del proyecto y las decisiones de implementación relevantes.

---

### node.h

Define las estructuras principales del R-tree:

- **MBR (Minimum Bounding Rectangle)**: representa un rectángulo mediante sus límites `(x1, x2, y1, y2)`.
- **Entry**: par clave-valor donde:
  - la clave es un MBR
  - el valor es un índice al nodo hijo (o -1 si es hoja)
- **Node**:
  - `k`: cantidad de hijos actuales
  - `entries`: arreglo de tamaño fijo B (204)
  - `pad`: relleno para alcanzar 4096 bytes (tamaño de bloque)

Esto asegura que **cada nodo corresponde exactamente a un bloque de disco**.

---

### utils.cpp

Contiene funciones auxiliares:

- `intersects(a, b)`: verifica si dos MBRs se intersectan
- `combine(a, b)`: retorna el MBR mínimo que contiene ambos rectángulos
- `centerX / centerY`: calcula el centro de un MBR (usado para ordenar)

Estas funciones son utilizadas en construcción y consultas.

---

### io.cpp

Responsable de la interacción con archivos binarios:

- `readPoints`: lee los puntos desde el dataset binario
  - cada punto se convierte en un MBR degenerado (punto)
- `writeTree`: escribe el árbol completo en disco
  - cada nodo se escribe secuencialmente

Esto permite simular almacenamiento en memoria externa.

---

### bulk_nearest_x.cpp

Implementa el algoritmo **Nearest-X**:

1. Ordena los elementos según el centro en X
2. Agrupa en bloques de tamaño B
3. Cada grupo forma un nodo
4. Se calcula el MBR del grupo
5. Se generan entradas para el siguiente nivel
6. Se repite recursivamente

Este método prioriza proximidad en el eje X.

---

### bulk_str.cpp

Implementa el algoritmo **STR (Sort-Tile-Recursive)**:

1. Ordena por X
2. Divide en S grupos (S ≈ sqrt(n/B))
3. Para cada grupo:
   - ordena por Y
   - subdivide en bloques de tamaño B
4. Cada subgrupo forma un nodo
5. Se repite recursivamente

Este método logra una mejor agrupación espacial en 2D.

---

### query.cpp

Implementa consultas por rango:

- `readNode`: lee un nodo desde disco usando su índice
  - incrementa el contador de accesos a disco (I/O)
- `rangeQuery`:
  - recorre recursivamente el árbol
  - si un MBR intersecta la consulta, se explora
  - si es hoja, se agregan los puntos al resultado

Las consultas se realizan **directamente desde el archivo binario**, cumpliendo la restricción de memoria externa.

---

### main.cpp

Controla toda la ejecución del programa:

#### Construcción:
- Lee datasets
- Ejecuta ambos métodos (Nearest-X y STR)
- Mide tiempos de ejecución
- Genera árboles en memoria
- Serializa a disco

#### Consultas:
- Genera 100 queries aleatorias por tamaño s
- Ejecuta consultas sobre el árbol en disco
- Mide:
  - accesos a disco
  - puntos encontrados

#### Resultados:
- Calcula promedios
- Calcula desviación estándar
- Guarda resultados en archivos CSV

---

### Manejo de la raíz (detalle importante)

La raíz se agrega al final del vector y luego se intercambia con la posición 0.  
Posteriormente, se actualizan todos los punteros a hijos para mantener consistencia.

Esto garantiza que:

- la raíz esté en índice 0
- los índices de los nodos sean correctos

---

### Estructura general del flujo

1. Leer datos
2. Construir árbol en RAM
3. Escribir árbol en disco
4. Ejecutar consultas desde disco
5. Registrar métricas
6. Generar resultados

---

### Consideraciones clave

- Cada nodo equivale a un bloque de disco (4096 bytes)
- Las consultas se hacen leyendo nodos desde archivo
- Cada lectura cuenta como un acceso a disco (I/O)
- Se evita mantener el árbol completo en memoria durante consultas