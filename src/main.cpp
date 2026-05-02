#include "node.h"
#include "utils.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

vector<Node> tree;

extern vector<Entry> buildNearestX(vector<Entry>);
extern vector<Entry> buildSTR(vector<Entry>);
extern vector<Entry> readPoints(string, int);
extern void writeTree(string, vector<Node>&);
extern void rangeQuery(ifstream&, int, MBR, vector<MBR>&);
extern int disk_reads;

float randFloat() {
    return (float)rand() / RAND_MAX;
}

int main() {

    cout << "=== START ===" << endl;

    // Crear carpeta results si no existe
    system("mkdir -p results");

    ofstream build_out("results/build_times.csv");
    ofstream query_out("results/query_results.csv");

    if (!build_out.is_open() || !query_out.is_open()) {
        cout << "ERROR: no se pudo abrir results/" << endl;
        return 1;
    }

    build_out << "N,metodo,dataset,tiempo_ms\n";
    query_out << "metodo,dataset,s,avg_io,avg_points,std_points\n";

    // N hasta 2^24
    vector<int> Ns;
    for (int i = 15; i <= 24; i++) Ns.push_back(1 << i);

    // Valores de s
    vector<float> Ss = {0.0025f, 0.005f, 0.01f, 0.025f, 0.05f};

    vector<string> datasets = {"data/random.bin", "data/europa.bin"};

   
    // FASE 1: Construcción para todos los N
    for (auto dataset : datasets) {

        string dname = (string(dataset).find("random") != string::npos)
                       ? "random" : "europa";

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

                tree.clear();

                auto start = chrono::high_resolution_clock::now();

                vector<Entry> rootEntries;

                if (string(metodo) == "nearest")
                    rootEntries = buildNearestX(points);
                else
                    rootEntries = buildSTR(points);

                Node root;
                root.k = rootEntries.size();
                for (int i = 0; i < root.k; i++)
                    root.entries[i] = rootEntries[i];

                int rootIdx = tree.size();
                tree.push_back(root);

                auto end = chrono::high_resolution_clock::now();
                int ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                build_out << N << "," << metodo << "," << dname << "," << ms << "\n";

                cout << "    tiempo: " << ms << " ms" << endl;

                // Guardar árbol para N=2^24
                if (N == (1 << 24)) {
                    string fname = "results/tree_" + dname + "_" + metodo + ".bin";
                    writeTree(fname, tree);
                    cout << "    arbol guardado: " << fname << endl;
                }
            }
        }
    }

    build_out.close();

   
    // FASE 2: Consultas solo con N=2^24
    cout << "\n[CONSULTAS] N = 2^24 = " << (1<<24) << endl;

    for (auto dataset : datasets) {

        string dname = (string(dataset).find("random") != string::npos)
                       ? "random" : "europa";

        cout << "\n  Dataset: " << dname << endl;

        for (auto metodo : {"nearest", "str"}) {

            string fname = "results/tree_" + dname + "_" + metodo + ".bin";

            // Necesitamos el rootIdx del árbol guardado.
            // Como la raíz siempre es el último nodo insertado,
            // calculamos el tamaño del archivo para encontrarlo.
            ifstream tmp(fname, ios::binary | ios::ate);
            if (!tmp.is_open()) {
                cout << "    ERROR: no se encontro " << fname << endl;
                continue;
            }
            int total_nodes = tmp.tellg() / sizeof(Node);
            int rootIdx = total_nodes - 1;  // la raíz es el último nodo
            tmp.close();

            cout << "    metodo: " << metodo
                 << " | nodos totales: " << total_nodes
                 << " | rootIdx: " << rootIdx << endl;

            for (auto s : Ss) {

                cout << "      s = " << s << flush;

                vector<int> ios_vec;
                vector<int> pts_vec;

                //100 consultas
                for (int q = 0; q < 100; q++) {

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

                    rangeQuery(file, rootIdx, R, res);

                    ios_vec.push_back(disk_reads);
                    pts_vec.push_back(res.size());
                }

                // Calcular estadísticas
                double avg_io = 0, avg_pts = 0;
                for (int x : ios_vec) avg_io += x;
                for (int x : pts_vec) avg_pts += x;
                avg_io /= ios_vec.size();
                avg_pts /= pts_vec.size();

                double stddev = 0;
                for (int x : pts_vec)
                    stddev += (x - avg_pts) * (x - avg_pts);
                stddev = sqrt(stddev / pts_vec.size());

                query_out << metodo << "," << dname << "," << s << ","
                          << avg_io << "," << avg_pts << "," << stddev << "\n";

                cout << " -> avg_io=" << avg_io
                     << " avg_pts=" << avg_pts << endl;
            }
        }
    }

    query_out.close();

    cout << "\n=== END ===" << endl;
    return 0;
}