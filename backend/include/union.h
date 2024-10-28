#ifndef UNION_H
#define UNION_H

#include <vector>

class UnionFind {
private:
    std::vector<int> parent;  // To store parent of each element
    std::vector<int> rank;    // To store rank (depth) of trees
    
public:
    // Constructor to initialize the Union-Find structure
    UnionFind(int size) {
        parent.resize(size);
        rank.resize(size, 0); // Initially, all ranks are 0
        
        // Every element is its own parent initially (self-loop)
        for (int i = 0; i < size; i++) {
            parent[i] = i;
        }
    }

    // Find the root of the set containing element x, with path compression
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // Path compression: point to the root
        }
        return parent[x];
    }


    // Union two sets (components), using rank to keep trees balanced
    bool unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        
        // If they are already in the same set, no need to union
        if (rootX == rootY) {
            return false;
        }
        
        // Union by rank: attach smaller tree under the root of the larger tree
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;  // Increment rank if ranks are equal
        }
        return true;
    }
};

#endif
