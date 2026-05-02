#include "node.h"
#include "utils.h"
#include <vector>
#include <algorithm>
#include <cmath>

extern std::vector<Node> tree;

std::vector<Entry> buildSTR(std::vector<Entry> entries) {

    if (entries.size() <= B) {
        return entries;
    }

    std::sort(entries.begin(), entries.end(),
        [](Entry a, Entry b) {
            return centerX(a.mbr) < centerX(b.mbr);
        });

    int n = entries.size();
    int S = std::ceil(std::sqrt(n / (double)B));

    std::vector<Entry> parents;

    for (int i = 0; i < n; i += S * B) {

        int end = std::min(i + S * B, n);

        std::sort(entries.begin() + i, entries.begin() + end,
            [](Entry a, Entry b) {
                return centerY(a.mbr) < centerY(b.mbr);
            });

        for (int j = i; j < end; j += B) {

            Node node;
            node.k = 0;

            std::vector<Entry> group;

            for (int k = j; k < std::min(j + B, end); k++) {
                node.entries[node.k++] = entries[k];
                group.push_back(entries[k]);
            }

            int idx = tree.size();
            tree.push_back(node);

            Entry e;
            e.mbr = computeMBR(group);
            e.child = idx;

            parents.push_back(e);
        }
    }

    return buildSTR(parents);
}