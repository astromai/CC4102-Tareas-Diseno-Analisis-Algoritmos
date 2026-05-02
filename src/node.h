#ifndef NODE_H
#define NODE_H

#include <vector>

/**
 * @file node.h
 * @brief Definición de estructuras de datos para la construcción de R-trees
 * 
 * Este archivo contiene las estructuras fundamentales para implementar un R-tree:
 * - MBR: Rectángulo delimitador mínimo en 2D
 * - Entry: Par (MBR, puntero hijo) que forma una entrada del árbol
 * - Node: Nodo del R-tree que contiene hasta B entries
 */

// ============================================================================
// CONSTANTE GLOBAL
// ============================================================================


/**
 * @constant B
 * @brief Número máximo de entries (pares MBR-hijo) por nodo
 * 
 * Este valor (204) está calibrado para que cada nodo ocupe exactamente 4096 bytes
 * (tamaño estándar de bloque de disco), optimizando el rendimiento de Io/S.
 *
 */
const int B = 204;


// ============================================================================
// ESTRUCTURA: MBR (Minimum Bounding Rectangle)
// ============================================================================

/**
 * @struct MBR
 * @brief Rectángulo delimitador mínimo en espacio 2D
 * 
 * Representa el rectángulo más pequeño que contiene un conjunto de puntos
 * o todos los puntos en un subárbol del R-tree.
 */
struct MBR {
    float x1;  // Coordenada X mínima del rectángulo
    float x2;  // Coordenada X máxima del rectángulo
    float y1;  // Coordenada Y mínima del rectángulo
    float y2;  // Coordenada Y máxima del rectángulo
};


// ============================================================================
// ESTRUCTURA: Entry
// ============================================================================

/**
 * @struct Entry
 * @brief Entrada en un nodo del R-tree (par MBR-puntero hijo)
 * 
 * Cada nodo del R-tree contiene hasta B entries. Cada entry mapea un
 * rectángulo delimitador a un hijo (nodo siguiente en el árbol).
 * 
 * En nodos hojas: El MBR representa el punto de datos (x1=x2, y1=y2)
 * En nodos internos: El MBR es la unión de todos los MBRs de sus hijos
 */
struct Entry {
    MBR mbr;    // Rectángulo delimitador mínimo del subárbol/punto
    int child;  // Índice en el vector global tree[] que apunta al nodo hijo (o -1/identificador especial para datos en hojas)
};


// ============================================================================
// ESTRUCTURA: Node
// ============================================================================

/**
 * @struct Node
 * @brief Nodo del R-tree
 * 
 * Representa un nodo en el árbol. 
 * 
 * Invariantes del R-tree:
 * - Nodos internos: 2 ≤ k ≤ B (excepto raíz, que puede tener k=1)
 * - Nodos hoja: 1 ≤ k ≤ B
 * - Los MBRs de las entries cubren todo el subárbol
 * - No hay solapamiento necesariamente entre entries hermanas
 */
struct Node {
    int k;              // Número actual de entries activas en este nodo (0 ≤ k ≤ B). Solo las primeras k entries contienen datos válidos
    Entry entries[B];   // Array de B entries potenciales
    char pad[12];       // Relleno de alineación para ocupar exactamente 4096 bytes
};

#endif