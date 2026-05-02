#include "node.h"
#include "utils.h"
#include <vector>
#include <algorithm>
#include <cmath>

extern std::vector<Node> tree;
/**
 * @brief Construccion STR (Sort-Tile-Recursive) para bulk-loading de R-tree.
 *
 * Resumen en pasos:
 * 1) Si el numero de entradas es <= B, devolver esas entradas (caso base).
 * 2) Ordenar todas las entradas por el centro en X (crear slices en X).
 * 3) Calcular S = ceil(sqrt(n / B)), el numero aproximado de slices.
 * 4) Para cada slice (intervalo de hasta S*B entradas):
 *    a) Ordenar las entradas de la slice por centro en Y.
 *    b) Particionar en bloques consecutivos de tamaño B.
 *    c) Por cada bloque crear un Node, calcular su MBR y añadir una Entry padre.
 * 5) Repetir recursivamente con la lista de entries padre hasta que quepan en un nodo.
 *
 * Esta funcion retorna el conjunto de entries del nivel superior construido; el
 * llamador debe crear el nodo raiz con esas entries.
 *
 * @param entries Entradas del nivel actual (hojas o internas).
 * @return Entradas del nivel superior para que el llamador arme la raiz.
 */
std::vector<Entry> buildSTR(std::vector<Entry> entries) {

    // Paso 1: caso base, si el numero de entries es pequeño, se retorna directamente.
    if (entries.size() <= B) {
        return entries;
    }


    // Paso 2: orden global por X para crear las slices
    std::sort(entries.begin(), entries.end(),
        [](Entry a, Entry b) {
            return centerX(a.mbr) < centerX(b.mbr);
        }
    );


    // Paso 3: calcular numero de slices S
    int n = entries.size();
    int S = std::ceil(std::sqrt(n / (double)B));

    std::vector<Entry> parents;


    // Paso 4: iterar por cada slice de hasta S*B entradas
    for (int i = 0; i < n; i += S * B) {

        int end = std::min(i + S * B, n);

        // Paso 4a: Dentro de la slice, ordenar por Y para formar tiles verticales
        std::sort(entries.begin() + i, entries.begin() + end,
            [](Entry a, Entry b) {
                return centerY(a.mbr) < centerY(b.mbr);
            });

        // Paso 4b: Particionar en bloques consecutivos de tamaño B
        for (int j = i; j < end; j += B) {

            Node node;
            node.k = 0;

            std::vector<Entry> group;

            // Llenar el nodo con hasta B entries
            for (int k = j; k < std::min(j + B, end); k++) {
                node.entries[node.k++] = entries[k];
                group.push_back(entries[k]);
            }

            // Guardar el nodo en la estructura global y crear entry padre
            int idx = tree.size();
            tree.push_back(node);

            // Paso 4c: Crear entry padre que resume el bloque y apunta al nodo recien insertado
            Entry e;
            e.mbr = computeMBR(group);
            e.child = idx;

            parents.push_back(e);
        }
    }

    
    // Paso 5: recursar hasta que el nivel superior quepa en un nodo
    return buildSTR(parents);
}