#ifndef UTILS_H
#define UTILS_H

#include "node.h"

/**
 * @file utils.h
 * @brief Declaraciones de funciones auxiliares para operaciones con MBRs
 * 
 * Proporciona utilidades para:
 * - Pruebas de intersección/solapamiento de rectángulos
 * - Combinación de múltiples MBRs
 * - Cálculo de centros de rectángulos
 */

/**
 * @brief Verifica si dos rectángulos delimitadores se solapan
 * @param a Primer rectángulo delimitador
 * @param b Segundo rectángulo delimitador
 * @return true si los rectángulos se solapan o tocan, false en caso contrario
 * 
 * Utiliza la prueba de separación en ejes: dos rectángulos NO se solapan
 * si están completamente separados en al menos uno de los ejes X o Y.
 * 
 * Caso de uso: En consultas de rango para descartar rápidamente subárboles
 * que no pueden contener resultados.
 */
bool intersects(MBR a, MBR b);

/**
 * @brief Combina dos rectángulos delimitadores en uno que los contiene a ambos
 * @param a Primer rectángulo delimitador
 * @param b Segundo rectángulo delimitador
 * @return MBR que es la unión envolvente de a y b
 * 
 * Retorna el rectángulo más pequeño que contiene completamente a ambos
 * rectángulos. Usado para actualizar MBRs durante la inserción.
 * 
 * Fórmula:
 * - x1 = min(a.x1, b.x1)
 * - x2 = max(a.x2, b.x2)
 * - y1 = min(a.y1, b.y1)
 * - y2 = max(a.y2, b.y2)
 */
MBR combine(MBR a, MBR b);

/**
 * @brief Calcula la coordenada X del centro de un rectángulo delimitador
 * @param m Rectángulo delimitador
 * @return Coordenada X del centro: (x1 + x2) / 2
 * 
 * Usado en algoritmos de construcción (Nearest-X, STR) para ordenar
 * o particionar puntos según su posición central.
 */
float centerX(MBR m);

/**
 * @brief Calcula la coordenada Y del centro de un rectángulo delimitador
 * @param m Rectángulo delimitador
 * @return Coordenada Y del centro: (y1 + y2) / 2
 * 
 * Usado en algoritmos de construcción (Nearest-X, STR) para ordenar
 * o particionar puntos según su posición central.
 */
float centerY(MBR m);

/**
 * @brief Computa el MBR que contiene a un grupo de entries
 * @param group Vector de entries cuyos MBRs se deben combinar
 * @return MBR envolvente que contiene a todos los MBRs del grupo
 * 
 * Itera sobre todas las entries combinando sus MBRs progresivamente.
 * Precondición: group no debe estar vacío.
 * 
 * Uso: Cuando se agrupa un conjunto de entries en un nodo, se calcula
 * su MBR envolvente para crear la entrada padre en el nivel superior.
 */
MBR computeMBR(std::vector<Entry>& group);

#endif