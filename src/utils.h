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

// ------------------- FUNCIONES AUXILIARES ----------------------------
/**
 * @brief Verifica si dos rectángulos delimitadores se solapan
 * @param a Primer rectángulo delimitador
 * @param b Segundo rectángulo delimitador
 * @return true si los rectángulos se solapan o tocan, false en caso contrario
 */
bool intersects(MBR a, MBR b);

/**
 * @brief Combina dos rectángulos delimitadores en uno que los contiene a ambos
 * @param a Primer rectángulo delimitador
 * @param b Segundo rectángulo delimitador
 * @return MBR que es la unión envolvente de a y b
 */
MBR combine(MBR a, MBR b);

/**
 * @brief Calcula la coordenada X del centro de un rectángulo delimitador
 * @param m Rectángulo delimitador
 * @return Coordenada X del centro
 */
float centerX(MBR m);

/**
 * @brief Calcula la coordenada Y del centro de un rectángulo delimitador
 * @param m Rectángulo delimitador
 * @return Coordenada Y del centro
 */
float centerY(MBR m);

/**
 * @brief Computa el MBR que contiene a un grupo de entries
 * @param group Vector de entries cuyos MBRs se deben combinar
 * @return MBR envolvente que contiene a todos los MBRs del grupo
 */
MBR computeMBR(std::vector<Entry>& group);

#endif