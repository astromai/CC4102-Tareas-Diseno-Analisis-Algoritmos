#include "node.h"
#include "utils.h"
#include <vector>
#include <algorithm>

extern std::vector<Node> tree;

std::vector<Entry> buildNearestX(std::vector<Entry> entries) {

    // 🔥 CASO BASE CORRECTO
    if (entries.size() <= B) {
        return entries;
    }

    // ordenar por X
    std::sort(entries.begin(), entries.end(),
        [](Entry a, Entry b) {
            return centerX(a.mbr) < centerX(b.mbr);
        });

    std::vector<Entry> parents;

    for (int i = 0; i < (int)entries.size(); i += B) {

        Node node;
        node.k = 0;

        std::vector<Entry> group;

        for (int j = i; j < std::min(i + B, (int)entries.size()); j++) {
            node.entries[node.k++] = entries[j];
            group.push_back(entries[j]);
        }

        int idx = tree.size();
        tree.push_back(node);

        Entry e;
        e.mbr = computeMBR(group);
        e.child = idx;

        parents.push_back(e);
    }

    // 🔥 IMPORTANTE: usar parents.size(), no entries.size()
    if (parents.size() <= B) {
        return parents;
    }

    return buildNearestX(parents);
}