#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <utility>
#include "Map.h"

/// Ruta BFS simple entre start y goal
std::vector<std::pair<int,int>> findPathBFS(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal
);

/// Ruta óptima pasando por todos los ítems
std::vector<std::pair<int,int>> findCollectAllPath(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal
);

#endif // PATHFINDER_H
