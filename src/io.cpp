#include "node.h"
#include <fstream>
#include <vector>
#include <iostream>

std::vector<Entry> readPoints(std::string filename, int N) {
    std::ifstream in(filename, std::ios::binary);

    std::vector<Entry> points;

    if (!in.is_open()) {
        std::cout << "ERROR: no se pudo abrir " << filename << std::endl;
        return points;
    }

    for (int i = 0; i < N; i++) {

        float x, y;

        if (!in.read((char*)&x, sizeof(float))) break;
        if (!in.read((char*)&y, sizeof(float))) break;

        Entry e;
        e.mbr = {x, x, y, y};
        e.child = -1;

        points.push_back(e);
    }

    std::cout << "    puntos reales leídos: " << points.size() << std::endl;

    return points;
}

void writeTree(std::string filename, std::vector<Node>& tree) {

    std::ofstream out(filename, std::ios::binary);

    if (!out.is_open()) {
        std::cout << "ERROR: no se pudo escribir " << filename << std::endl;
        return;
    }

    for (auto &n : tree) {
        out.write((char*)&n, sizeof(Node));
    }
}