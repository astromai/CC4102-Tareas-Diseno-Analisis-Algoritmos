#include "node.h"
#include "utils.h"
#include <algorithm>

/**
 * @brief Verifica si dos rectángulos delimitadores se solapan
 * 
 * Dos rectángulos se solapan si y solo si se solapan en AMBOS ejes X e Y.
 * Equivalentemente, NO se solapan si están separados en al menos un eje.
 * 
 * @param a Primer rectángulo
 * @param b Segundo rectángulo
 * @return true si se solapan (incluyendo bordes), false si están separados
 */
bool intersects(MBR a, MBR b) {
    return !(a.x2 < b.x1 || a.x1 > b.x2 || a.y2 < b.y1 || a.y1 > b.y2);
}


/**
 * @brief Combina dos rectángulos en su envolvente común
 * 
 * Calcula el rectángulo más pequeño que contiene completamente a ambos
 * rectángulos de entrada. Los valores extremos se toman por eje.
 * 
 * @param a Primer rectángulo
 * @param b Segundo rectángulo
 * @return MBR envolvente de a y b
 */
MBR combine(MBR a, MBR b) {
    return {
        std::min(a.x1, b.x1),
        std::max(a.x2, b.x2),
        std::min(a.y1, b.y1),
        std::max(a.y2, b.y2)
    };
}


/**
 * @brief Computa el MBR envolvente de un grupo de entries
 * 
 * Itera secuencialmente sobre todas las entries, combinando sus MBRs
 * mediante aplicación repetida de combine().
 * 
 * @param group Vector de entries (precondición: no vacío)
 * @return MBR que contiene la unión de todos los MBRs del grupo
 */
MBR computeMBR(std::vector<Entry>& group) {
    MBR m = group[0].mbr;
    for (auto &e : group)
        m = combine(m, e.mbr);
    return m;
}

/**
 * @brief Calcula la coordenada X del centro de un rectángulo
 * 
 * Retorna el punto medio en el eje X: (x1 + x2) / 2
 * 
 * @param m Rectángulo delimitador
 * @return Coordenada X del centro
 */
float centerX(MBR m) {
    return (m.x1 + m.x2) / 2;
}

/**
 * @brief Calcula la coordenada Y del centro de un rectángulo
 * 
 * Retorna el punto medio en el eje Y: (y1 + y2) / 2
 * 
 * @param m Rectángulo delimitador
 * @return Coordenada Y del centro
 */
float centerY(MBR m) {
    return (m.y1 + m.y2) / 2;
}