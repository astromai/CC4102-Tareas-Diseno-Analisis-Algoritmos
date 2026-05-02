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

    ofstream build_out("results/build_times.csv");
    ofstream query_out("results/query_results.csv");

    if (!build_out.is_open() || !query_out.is_open()) {
        cout << "ERROR: no se pudo abrir results/" << endl;
        return 1;
    }

    build_out << "N,metodo,dataset,tiempo_ms\n";
    query_out << "metodo,dataset,s,avg_io,avg_points,std_points\n";

    vector<int> Ns;
    for (int i = 15; i <= 17; i++) Ns.push_back(1<<i);

    vector<float> Ss = {0.01, 0.05};

    for (auto dataset : {"data/random.bin","data/europa.bin"}) {

        string dname = (string(dataset).find("random")!=string::npos)?"random":"europa";

        cout << "\nDataset: " << dname << endl;

        for (auto N : Ns) {

            cout << "  N = " << N << endl;

            auto points = readPoints(dataset, N);

            cout << "  puntos leídos: " << points.size() << endl;

            if (points.empty()) {
                cout << "  ERROR: no se leyeron puntos" << endl;
                continue;
            }

            for (auto metodo : {"nearest","str"}) {

                cout << "    metodo: " << metodo << endl;

                tree.clear();

                auto start = chrono::high_resolution_clock::now();

                vector<Entry> rootEntries;

                if (string(metodo)=="nearest")
                    rootEntries = buildNearestX(points);
                else
                    rootEntries = buildSTR(points);

                cout << "    rootEntries: " << rootEntries.size() << endl;

                Node root;
                root.k = rootEntries.size();

                for (int i = 0; i < root.k; i++)
                    root.entries[i] = rootEntries[i];

                int rootIdx = tree.size();
                tree.push_back(root);

                auto end = chrono::high_resolution_clock::now();
                int time = chrono::duration_cast<chrono::milliseconds>(end-start).count();

                cout << "    tiempo: " << time << " ms" << endl;

                build_out << N << "," << metodo << "," << dname << "," << time << "\n";

                string fname = "results/tree_" + dname + "_" + metodo + ".bin";
                writeTree(fname, tree);

                cout << "    árbol guardado en: " << fname << endl;

                for (auto s : Ss) {

                    cout << "      query s=" << s << endl;

                    vector<int> ios;
                    vector<int> pts;

                    for (int q = 0; q < 10; q++) {

                        float x = randFloat()*(1-s);
                        float y = randFloat()*(1-s);

                        MBR R = {x, x+s, y, y+s};

                        ifstream file(fname, ios::binary);
                        if (!file.is_open()) {
                            cout << "ERROR abriendo archivo binario" << endl;
                            continue;
                        }

                        vector<MBR> res;

                        disk_reads = 0;
                        rangeQuery(file, rootIdx, R, res); 

                        ios.push_back(disk_reads);
                        pts.push_back(res.size());
                    }

                    double avg_io=0, avg_pts=0;
                    for (int x:ios) avg_io+=x;
                    for (int x:pts) avg_pts+=x;

                    avg_io/=ios.size();
                    avg_pts/=pts.size();

                    double std=0;
                    for (int x:pts) std+=(x-avg_pts)*(x-avg_pts);
                    std = sqrt(std/pts.size());

                    query_out << metodo << "," << dname << "," << s << ","
                              << avg_io << "," << avg_pts << "," << std << "\n";
                }
            }
        }
    }

    cout << "=== END ===" << endl;

    build_out.close();
    query_out.close();

    return 0;
}