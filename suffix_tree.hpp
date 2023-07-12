#ifndef SUFFIXTREE_H
#define SUFFIXTREE_H

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <unordered_map>
//#include <map>
#include <queue>

const char SENTINEL = 'z' + 1;

class TSuffTree {
private:
    struct TNode {
        std::unordered_map <char, int> edges;
        //std::map <char, int> edges;
        int suff_link;
        int leaf_id;                       // start index in text for this suffix
        TNode() {
            suff_link = -1;                // has no suffix link yet
            leaf_id = -1;                  // by default is not a leaf
        }
    };

    struct TEdge {
        int dest_node;
        int begin_index;                   // index in text of the first char on edge
        int length;                        // number of char on edge

        TEdge() {
            length = 0;
        };

        TEdge(int dest_node, int begin_index, int length) {
            this->dest_node = dest_node;
            this->begin_index = begin_index;
            this->length = length;
        }
    };

    // structure for storing the current position in the tree
    struct TState {
        int node;                          // current node
        int edge;                          // current edge
        int pos_on_edge;                   // position from the beginning of the edge (zero-based)
        bool on_edge;                      // flag of standing on the edge
        /*
         * when on_edge = false
         * it's assumed that the Edge leads to the current Node
         * !!! exept after break_edge() !!!
         * Then Edge starts from Node (as in the case below)
         *
         * when on_edge = true
         * it's assumed that current Edge starts from Node
         */

        TState() {
            on_edge = false;               // initial state in node (root)
            // before build() required to set: state.node = Root!
        }
    };

    std::string data;                      // text for building the Suffix Tree
    TState state;                          // current position in the tree
    int root;                              // root of the tree (cannot make it const although it does not change)
    std::vector<TNode> nodes;              // storage for all nodes of the tree
    std::vector<TEdge> edges;              // storage for all edges of the tree
    int leaves_count = 0;
    int INF = 1e9;                         // infinite length of edge to leaf

    int  create_node();
    int  create_edge(int dest, int start_index, int length);
    bool has_way(const TState& st, char c);
    bool has_way(char c);
    void go_one_char(TState& st, char c);
    void go_one_char(char c);
    int  go_one_edge(TState& st, char c, int depth);
    int  go_one_edge(char c, int depth);
    void go_down(TState& st, int char_ind, int depth);
    void go_down(int char_ind, int depth);
    void go_suff_link(TState& st);
    void go_suff_link();
    void break_edge();
    void create_leaf(int parent, int char_ind);
    void set_suff_link(int from, int to);
    void build();
    void add_part_of_str(int current_len);
    void print(int n, int h);
    bool find(const std::string& s, TState& st);

public:
    TSuffTree(const std::string& s);
    void Print();
    std::vector<int> find_occurrences(const std::string& s);
};

#endif /* ifndef SUFFIXTREE_H */

