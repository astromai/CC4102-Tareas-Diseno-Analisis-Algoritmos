#include "node.h"
#include <fstream>
#include <vector>
#include <iostream>

/**
 * @brief Lee hasta N puntos desde un archivo binario
 * 
 * El archivo de entrada debe contener pares consecutivos de coordenadas
 * flotantes en formato binario: primero x y luego y para cada punto.
 * Cada par leído se convierte en una Entry hoja cuyo MBR representa el
 * punto exacto (x1 = x2, y1 = y2) y cuyo hijo queda marcado como -1.
 * 
 * Si el archivo no puede abrirse, se retorna un vector vacío.
 * 
 * @param filename Ruta del archivo binario de entrada
 * @param N Número máximo de puntos a leer
 * @return Vector de entries construidas a partir de los puntos leídos
 */
std::vector<Entry> readPoints(std::string filename, int N) {
    // Abrir archivo en modo binario para lectura de floats crudos
    std::ifstream in(filename, std::ios::binary);

    // Vector donde se almacenarán las entries construidas a partir de los puntos
    std::vector<Entry> points;

    // Verificar que el archivo se abrió correctamente
    if (!in.is_open()) {
        std::cout << "ERROR: no se pudo abrir " << filename << std::endl;
        return points;
    }

    // Intentar leer hasta N puntos. Cada punto está formado por dos floats: (x, y).
    for (int i = 0; i < N; i++) {

        float x, y;

        // Leer coordenada X; si falla (EOF o error) salir del bucle
        if (!in.read((char*)&x, sizeof(float))) break;
        // Leer coordenada Y; si falla (EOF o error) salir del bucle
        if (!in.read((char*)&y, sizeof(float))) break;

        // Construir una Entry hoja donde el MBR representa el punto exacto
        Entry e;
        e.mbr = {x, x, y, y}; // x1=x2=x, y1=y2=y
        e.child = -1;         // child = -1 indica entrada de hoja / dato

        // Añadir la entry al vector de resultados
        points.push_back(e);
    }

    // Informar cuántos puntos se leyeron realmente (puede ser < N si hubo EOF)
    std::cout << "    puntos reales leídos: " << points.size() << std::endl;

    return points;
}


/**
 * @brief Escribe un árbol completo en un archivo binario
 * 
 * Serializa cada Node del vector tree escribiendo su representación en memoria
 * de forma cruda. El archivo resultante queda organizado como una secuencia de
 * nodos, en el mismo orden en que aparecen en el vector.
 * 
 * Si el archivo no puede abrirse, la función reporta el error y termina sin
 * escribir datos.
 * 
 * @param filename Ruta del archivo binario de salida
 * @param tree Vector de nodos del R-tree a serializar
 */
void writeTree(std::string filename, std::vector<Node>& tree) {
    // Abrir archivo en modo binario para volcar la representación en memoria de los nodos
    std::ofstream out(filename, std::ios::binary);

    // Verificar apertura antes de escribir
    if (!out.is_open()) {
        std::cout << "ERROR: no se pudo escribir " << filename << std::endl;
        return;
    }

    // Escribir cada Node tal como está en memoria. Esto produce un volcado binario
    // dependiente del layout de la estructura (endianness, padding, etc.).
    for (auto &n : tree) {
        out.write((char*)&n, sizeof(Node));
    }
}