#include "node.h"
#include "utils.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

vector<Node> tree;

// Declaraciones de funciones externas definidas en otros archivos.
extern vector<Entry> buildNearestX(vector<Entry>);
extern vector<Entry> buildSTR(vector<Entry>);
extern vector<Entry> readPoints(string, int);
extern void writeTree(string, vector<Node>&);
extern void rangeQuery(ifstream&, int, MBR, vector<MBR>&);
extern int disk_reads;

/**
 * @file main.cpp
 * @brief Programa principal para benchmark de construcción y consultas sobre R-tree.
 *
 * Este archivo orquesta dos fases experimentales:
 * 1) Construcción de árboles para distintos tamaños N, datasets y métodos de bulk-loading.
 * 2) Ejecución de consultas de rango sobre los árboles más grandes (N = 2^24),
 *    registrando costo de I/O y cardinalidad de resultados.
 *
 * Salidas generadas:
 * - results/build_times.csv: tiempos de construcción por configuración.
 * - results/query_results.csv: estadísticas agregadas de consultas.
 * - results/tree_<dataset>_<metodo>.bin: árboles serializados para N máximo.
 */


/**
 * @brief Genera un número flotante pseudoaleatorio uniforme en [0, 1].
 *
 * @return Valor flotante normalizado a partir de rand().
 */
float randFloat() {
    return (float)rand() / RAND_MAX;
}

/**
 * @brief Punto de entrada del benchmark.
 *
 * Flujo general:
 * 1) Prepara carpeta y archivos de salida CSV.
 * 2) Fase de construcción:
 *    - Lee puntos desde datasets binarios.
 *    - Construye R-tree con métodos "nearest" y "str".
 *    - Mide tiempo de construcción y lo registra.
 *    - Serializa árbol en disco solo para N = 2^24.
 * 3) Fase de consultas:
 *    - Carga cada árbol serializado para N = 2^24.
 *    - Ejecuta 100 range queries por tamaño de ventana s.
 *    - Registra promedio de lecturas de disco, promedio de puntos y desviación estándar.
 *
 * @return 0 si la ejecución finaliza correctamente, 1 ante error al abrir salidas.
 */
int main() {

    cout << "=== START ===" << endl;

    // Crear carpeta de resultados si no existe.
    system("mkdir -p results");

    ofstream build_out("results/build_times.csv");
    ofstream query_out("results/query_results.csv");

    // Validar apertura de archivos de salida.
    if (!build_out.is_open() || !query_out.is_open()) {
        cout << "ERROR: no se pudo abrir results/" << endl;
        return 1;
    }

    // Escribir cabeceras CSV.
    build_out << "N,metodo,dataset,tiempo_ms\n";
    query_out << "metodo,dataset,s,avg_io,avg_points,std_points\n";

    // Lista de tamaños: N = 2^15 ... 2^24.
    vector<int> Ns;
    for (int i = 15; i <= 24; i++) Ns.push_back(1 << i);

    // Tamaños relativos de la ventana de consulta.
    vector<float> Ss = {0.0025f, 0.005f, 0.01f, 0.025f, 0.05f};

    // Datasets de entrada (coordenadas binarias x,y).
    vector<string> datasets = {"data/random.bin", "data/europa.bin"};


    // =========================================================================
    // FASE 1: Construcción para todos los N
    // =========================================================================
    for (auto dataset : datasets) {

        // Nombre corto del dataset para etiquetar métricas/archivos.
        string dname = (string(dataset).find("random") != string::npos) ? "random" : "europa";

        cout << "\n[CONSTRUCCION] Dataset: " << dname << endl;

        for (auto N : Ns) {

            cout << "  N = " << N << endl;

            auto points = readPoints(dataset, N);

            if (points.empty()) {
                cout << "  ERROR: no se leyeron puntos" << endl;
                continue;
            }

            for (auto metodo : {"nearest", "str"}) {

                cout << "    metodo: " << metodo << endl;

                // Reinicia estructura global antes de construir un árbol nuevo.
                tree.clear();

                auto start = chrono::high_resolution_clock::now();

                vector<Entry> rootEntries;

                // Bulk-loading según método seleccionado.
                if (string(metodo) == "nearest")
                    rootEntries = buildNearestX(points);
                else
                    rootEntries = buildSTR(points);

                // Construcción explícita de raíz con entries del nivel superior.
                Node root;
                root.k = rootEntries.size();
                for (int i = 0; i < root.k; i++)
                    root.entries[i] = rootEntries[i];

                // La raíz se inserta al final del vector global.
                int rootIdx = tree.size();
                tree.push_back(root);

                auto end = chrono::high_resolution_clock::now();
                int ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                // Escribir métricas de construcción en CSV.
                build_out << N << "," << metodo << "," << dname << "," << ms << "\n";

                cout << "    tiempo: " << ms << " ms" << endl;

                // Guardar árbol binario solo para el N máximo (2^24).
                if (N == (1 << 24)) {
                    string fname = "results/tree_" + dname + "_" + metodo + ".bin";
                    writeTree(fname, tree);
                    cout << "    arbol guardado: " << fname << endl;
                }
            }
        }
    }

    build_out.close();


    // =========================================================================
    // FASE 2: Consultas solo con N = 2^24
    // =========================================================================
    cout << "\n[CONSULTAS] N = 2^24 = " << (1<<24) << endl;

    for (auto dataset : datasets) {

        string dname = (string(dataset).find("random") != string::npos) ? "random" : "europa";

        cout << "\n  Dataset: " << dname << endl;

        for (auto metodo : {"nearest", "str"}) {

            string fname = "results/tree_" + dname + "_" + metodo + ".bin";

            // Recuperar índice de raíz desde archivo:
            // la raíz es el último Node serializado.
            ifstream tmp(fname, ios::binary | ios::ate);
            if (!tmp.is_open()) {
                cout << "    ERROR: no se encontro " << fname << endl;
                continue;
            }
            int total_nodes = tmp.tellg() / sizeof(Node);
            int rootIdx = total_nodes - 1;
            tmp.close();

            cout << "    metodo: " << metodo << " | nodos totales: " << total_nodes << " | rootIdx: " << rootIdx << endl;

            for (auto s : Ss) {

                cout << "      s = " << s << flush;

                vector<int> ios_vec;
                vector<int> pts_vec;

                // Ejecutar 100 consultas aleatorias por cada s.
                for (int q = 0; q < 100; q++) {

                    // Ventana [x, x+s] x [y, y+s] dentro del cuadrado unitario.
                    float x = randFloat() * (1.0f - s);
                    float y = randFloat() * (1.0f - s);
                    MBR R = {x, x + s, y, y + s};

                    ifstream file(fname, ios::binary);
                    if (!file.is_open()) {
                        cout << "\n    ERROR abriendo " << fname << endl;
                        continue;
                    }

                    vector<MBR> res;
                    disk_reads = 0;

                    // Recorre árbol en disco y acumula resultados.
                    rangeQuery(file, rootIdx, R, res);

                    // Métricas por consulta.
                    ios_vec.push_back(disk_reads);
                    pts_vec.push_back(res.size());
                }

                // Estadísticos agregados de las 100 consultas.
                double avg_io = 0, avg_pts = 0;
                for (int x : ios_vec) avg_io += x;
                for (int x : pts_vec) avg_pts += x;
                avg_io /= ios_vec.size();
                avg_pts /= pts_vec.size();

                double stddev = 0;
                for (int x : pts_vec)
                    stddev += (x - avg_pts) * (x - avg_pts);
                stddev = sqrt(stddev / pts_vec.size());

                // Guardar resultados de consulta en CSV.
                query_out << metodo << "," << dname << "," << s << "," << avg_io << "," << avg_pts << "," << stddev << "\n";

                cout << " -> avg_io=" << avg_io << " avg_pts=" << avg_pts << endl;
            }
        }
    }

    query_out.close();

    cout << "\n=== END ===" << endl;
    return 0;
}