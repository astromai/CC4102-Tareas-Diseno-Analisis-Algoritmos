#include "node.h"
#include "utils.h"
#include <fstream>
#include <vector>

int disk_reads = 0;

Node readNode(std::ifstream &file, int index) {
    Node n;
    file.seekg(index * sizeof(Node));
    file.read((char*)&n, sizeof(Node));
    disk_reads++;
    return n;
}

void rangeQuery(std::ifstream &file, int idx, MBR R, std::vector<MBR>& result) {
    Node node = readNode(file, idx);

    for (int i = 0; i < node.k; i++) {
        if (intersects(node.entries[i].mbr, R)) {
            if (node.entries[i].child == -1)
                result.push_back(node.entries[i].mbr);
            else
                rangeQuery(file, node.entries[i].child, R, result);
        }
    }
}