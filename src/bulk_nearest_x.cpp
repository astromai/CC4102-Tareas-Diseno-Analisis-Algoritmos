#include "node.h"
#include "utils.h"
#include <vector>
#include <algorithm>

extern std::vector<Node> tree;

/**
 * @file bulk_nearest_x.cpp
 * @brief Construccion bulk-loading de niveles de R-tree ordenando por eje X.
 *
 * @brief Construye recursivamente niveles superiores del arbol agrupando por cercania en X.
 *
 * Algoritmo:
 * 1) Caso base: si entries.size() <= B, se retorna entries.
 * 2) Ordenar entradas por centerX(mbr).
 * 3) Particionar secuencialmente en bloques de tamano B.
 * 4) Crear un Node por bloque y guardarlo en tree.
 * 5) Crear una entrada padre por bloque con:
 *    - mbr = MBR combinado del bloque
 *    - child = indice del Node en tree
 * 6) Si parents.size() <= B, retornar parents.
 * 7) En caso contrario, repetir recursivamente con parents.
 *
 * @param entries Entradas del nivel actual (hojas o internas).
 * @return Entradas del nivel superior para que el llamador arme la raiz final.
 */
std::vector<Entry> buildNearestX(std::vector<Entry> entries) {

    if (entries.size() <= B) {
        return entries;
    }

    // Orden espacial por eje X para juntar objetos cercanos horizontalmente.
    std::sort(entries.begin(), entries.end(),
        [](Entry a, Entry b) {
            return centerX(a.mbr) < centerX(b.mbr);
        });

    std::vector<Entry> parents;

    // Cada bloque de hasta B entradas se convierte en un nodo hijo.
    for (int i = 0; i < (int)entries.size(); i += B) {

        Node node;
        node.k = 0;

        std::vector<Entry> group;

        for (int j = i; j < std::min(i + B, (int)entries.size()); j++) {
            node.entries[node.k++] = entries[j];
            group.push_back(entries[j]);
        }

        // Guardar nodo y registrar su indice para enlazarlo desde el padre.
        int idx = tree.size();
        tree.push_back(node);

        Entry e;
        // La entrada padre resume el bloque y apunta al nodo recien insertado.
        e.mbr = computeMBR(group);
        e.child = idx;

        parents.push_back(e);
    }

    // Si el nivel superior ya cabe en un nodo, se retorna para que el llamador arme la raiz.
    if (parents.size() <= B) {
        return parents;
    }

    // Si aun no cabe en un nodo, se construye el nivel siguiente.
    return buildNearestX(parents);
}