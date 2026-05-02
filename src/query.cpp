#include "node.h"
#include "utils.h"
#include <fstream>
#include <vector>

// Contador global de lecturas a disco realizadas por las consultas.
int disk_reads = 0;

/**
 * @brief Lee desde disco el nodo almacenado en la posición indicada
 *
 * El archivo se interpreta como un arreglo contiguo de `Node`, por lo que
 * el desplazamiento se calcula con `index * sizeof(Node)`.
 *
 * @param file Archivo binario que contiene los nodos serializados
 * @param index Índice lógico del nodo dentro del archivo
 * @return Nodo leído desde la posición solicitada
 */
Node readNode(std::ifstream &file, int index) {
    Node n;
    // Posiciona el cursor en el nodo solicitado dentro del archivo.
    file.seekg(index * sizeof(Node));
    // Carga el bloque completo que representa un nodo del R-tree.
    file.read((char*)&n, sizeof(Node));
    // Registra la lectura de disco para el análisis de costo de consultas.
    disk_reads++;
    return n;
}


/**
 * @brief Recorre recursivamente el R-tree para encontrar MBR que intersectan
 *
 * Parte desde el nodo `idx` y acumula en `result` todos los MBR hoja que
 * intersectan con la región de consulta `R`.
 *
 * @param file Archivo binario con los nodos del R-tree
 * @param idx Índice del nodo actual dentro del árbol
 * @param R Región de consulta contra la que se filtran las entradas
 * @param result Vector de salida donde se almacenan los MBR encontrados
 */
void rangeQuery(std::ifstream &file, int idx, MBR R, std::vector<MBR>& result) {
    // Recupera el nodo actual desde disco para inspeccionar sus entradas.
    Node node = readNode(file, idx);

    for (int i = 0; i < node.k; i++) {
        // Solo se sigue la rama si el MBR del hijo puede intersectar la consulta.
        if (intersects(node.entries[i].mbr, R)) {
            // child == -1 indica una entrada hoja: se guarda el rectángulo hallado.
            if (node.entries[i].child == -1){
                result.push_back(node.entries[i].mbr);
            }
            // En nodos internos, se continúa el recorrido hacia el hijo correspondiente.
            else
                rangeQuery(file, node.entries[i].child, R, result);
        }
    }
}