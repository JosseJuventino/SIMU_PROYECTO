#include "PathFinder.h"
#include <queue>
#include <algorithm>

std::vector<std::pair<int,int>>
findPathBFS(const Map& map,
            std::pair<int,int> start,
            std::pair<int,int> goal)
{
    int R = map.getRows(), C = map.getCols();
    std::vector<std::vector<bool>> vis(R, std::vector<bool>(C,false));
    std::vector<std::vector<std::pair<int,int>>> parent(R, std::vector<std::pair<int,int>>(C,{-1,-1}));
    std::queue<std::pair<int,int>> q;

    vis[start.first][start.second] = true;
    q.push(start);

    int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};

    while (!q.empty()) {
        auto [r,c] = q.front(); q.pop();
        if (r==goal.first && c==goal.second) break;
        for (int i=0;i<4;++i) {
            int nr = r+dr[i], nc = c+dc[i];
            if (nr<0||nr>=R||nc<0||nc>=C) continue;
            if (!vis[nr][nc] && map.isWalkable(nr,nc)) {
                vis[nr][nc] = true;
                parent[nr][nc] = {r,c};
                q.push({nr,nc});
            }
        }
    }

    if (!vis[goal.first][goal.second]) return {};
    std::vector<std::pair<int,int>> path;
    for (auto cur = goal; cur!=start; cur = parent[cur.first][cur.second])
        path.push_back(cur);
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::pair<int,int>>
findCollectAllPath(const Map& map,
                   std::pair<int,int> start,
                   std::pair<int,int> goal)
{
    return findPathBFS(map, start, goal);
}
