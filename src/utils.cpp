#include "node.h"
#include "utils.h"
#include <algorithm>

bool intersects(MBR a, MBR b) {
    return !(a.x2 < b.x1 || a.x1 > b.x2 ||
             a.y2 < b.y1 || a.y1 > b.y2);
}

MBR combine(MBR a, MBR b) {
    return {
        std::min(a.x1, b.x1),
        std::max(a.x2, b.x2),
        std::min(a.y1, b.y1),
        std::max(a.y2, b.y2)
    };
}

MBR computeMBR(std::vector<Entry>& group) {
    MBR m = group[0].mbr;
    for (auto &e : group)
        m = combine(m, e.mbr);
    return m;
}

float centerX(MBR m) {
    return (m.x1 + m.x2) / 2;
}

float centerY(MBR m) {
    return (m.y1 + m.y2) / 2;
}