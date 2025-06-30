#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include "Map.h"
#include <vector>
#include <utility>

std::vector<std::pair<int,int>> findPathBFS(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal);

std::vector<std::pair<int,int>> findCollectAllPath(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal);

#endif