#ifndef UTILS_H
#define UTILS_H

#include "node.h"

bool intersects(MBR a, MBR b);
MBR combine(MBR a, MBR b);
float centerX(MBR m);
float centerY(MBR m);
MBR computeMBR(std::vector<Entry>& group);
#endif