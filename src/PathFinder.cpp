#include "PathFinder.h"
#include <queue>
#include <algorithm>
#include <limits>

// 1) BFS simple
std::vector<std::pair<int,int>> findPathBFS(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal
) {
    int R=map.getRows(), C=map.getCols();
    std::vector<std::vector<bool>> seen(R, std::vector<bool>(C,false));
    std::vector<std::vector<std::pair<int,int>>> parent(R,
        std::vector<std::pair<int,int>>(C,{-1,-1})
    );
    std::queue<std::pair<int,int>>q;
    q.push(start);
    seen[start.first][start.second]=true;
    int dr[4]={-1,1,0,0}, dc[4]={0,0,-1,1};
    bool found=false;
    while(!q.empty() && !found){
        auto [r,c]=q.front();q.pop();
        if(std::make_pair(r,c)==goal){ found=true; break; }
        for(int i=0;i<4;i++){
            int nr=r+dr[i], nc=c+dc[i];
            if(nr>=0&&nr<R&&nc>=0&&nc<C && !seen[nr][nc]
               && !map.isWall(nr,nc)) {
                seen[nr][nc]=true;
                parent[nr][nc]={r,c};
                q.push({nr,nc});
            }
        }
    }
    if(!found) return {};
    std::vector<std::pair<int,int>> path;
    for(auto cur=goal; cur!=start; cur=parent[cur.first][cur.second])
        path.push_back(cur);
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

// 2) TSP sobre ítems
std::vector<std::pair<int,int>> findCollectAllPath(
    const Map& map,
    std::pair<int,int> start,
    std::pair<int,int> goal
) {
    auto items = map.getItems();
    int M = int(items.size());
    std::vector<std::pair<int,int>> P;
    P.push_back(start);
    P.insert(P.end(), items.begin(), items.end());
    P.push_back(goal);
    int N = int(P.size()), goalIdx = N-1;
    const int INF = std::numeric_limits<int>::max()/2;

    // pre-cálculo distancias y fragmentos
    std::vector<std::vector<int>> dist(N, std::vector<int>(N, INF));
    std::vector<std::vector<std::vector<std::pair<int,int>>>> seg(N,
        std::vector<std::vector<std::pair<int,int>>>(N)
    );
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            if(i==j) continue;
            auto s = findPathBFS(map, P[i], P[j]);
            if(!s.empty()){
                dist[i][j] = int(s.size())-1;
                seg[i][j] = std::move(s);
            }
        }
    }

    // fuerza bruta permutaciones
    std::vector<int> perm(M);
    for(int i=0;i<M;i++) perm[i]=i+1;
    int bestCost=INF;
    std::vector<int> bestPerm;
    do {
        int cost = dist[0][perm[0]];
        for(int k=0;k<M-1;k++) cost += dist[perm[k]][perm[k+1]];
        cost += dist[perm[M-1]][goalIdx];
        if(cost<bestCost){
            bestCost=cost;
            bestPerm=perm;
        }
    } while(std::next_permutation(perm.begin(), perm.end()));

    std::vector<std::pair<int,int>> fullPath;
    if(bestPerm.empty()) return fullPath;

    // concatena segmentos
    auto &f0 = seg[0][bestPerm[0]];
    fullPath.insert(fullPath.end(), f0.begin(), f0.end());
    for(int k=0;k<M-1;k++){
        auto &s2 = seg[bestPerm[k]][bestPerm[k+1]];
        fullPath.insert(fullPath.end(), s2.begin()+1, s2.end());
    }
    auto &fN = seg[bestPerm[M-1]][goalIdx];
    fullPath.insert(fullPath.end(), fN.begin()+1, fN.end());

    return fullPath;
}
