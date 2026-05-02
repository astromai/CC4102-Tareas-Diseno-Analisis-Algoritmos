#ifndef NODE_H
#define NODE_H

#include <vector>

const int B = 204;

struct MBR {
    float x1, x2, y1, y2;
};

struct Entry {
    MBR mbr;
    int child;
};

struct Node {
    int k;
    Entry entries[B];
    char pad[12];
};

#endif