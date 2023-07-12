#include "suffix_tree.hpp"

TSuffTree::TSuffTree(const std::string& s) {
        data = s;
        nodes.reserve(2 * data.size());
        edges.reserve(2 * data.size());
        
        root = create_node();
        state.node = root;
        build();
}

int TSuffTree::create_node() {
    TNode new_node;
    nodes.emplace_back(new_node);
    return nodes.size() - 1;
}

int TSuffTree::create_edge(int dest, int start_index, int length) {
    TEdge new_edge(dest, start_index, length);
    edges.emplace_back(new_edge);
    return edges.size() - 1;
}

/*
 * is there a way with char `c` in the current state
 */
bool TSuffTree::has_way(const TState& st, char c) {
    if (st.on_edge) {
        /*
         * never boundaries of the text
         * assert(edges[st.edge].begin_index + st.pos_on_edge + 1 < data.size());
         */
        return (c == data[edges[st.edge].begin_index + st.pos_on_edge + 1]);
    } else {
        return nodes[st.node].edges.find(c) != nodes[st.node].edges.end();
    }
}
bool TSuffTree::has_way(char c) {
    return has_way(state, c);
}

/*
 * move down in tree only for one char
 */
void TSuffTree::go_one_char(TState& st, char c) {
    assert(has_way(st, c));
    if (st.on_edge) {
        ++st.pos_on_edge;
    } else {
        st.pos_on_edge = 0;
        st.on_edge = true;
        st.edge = nodes[st.node].edges[c];
    }
    /*
     * the end of the current edge has been reached
     * regardless of the previous st, a check is needed
     * because there can be only 1 char on the edge and
     * we immediately move on to the next Node
     */
    if (st.pos_on_edge + 1 == edges[st.edge].length) {
        st.on_edge = false;
        st.node = edges[st.edge].dest_node;
    }
}
void TSuffTree::go_one_char(char c) {
    go_one_char(state, c);
}

/*
 * goes down no more than One edge and no more than `depth`
 * chars (the first of them is `c`)
 *
 * this function assumes that the initial state is
 *                           state.on_edge = False
 * also, the Algorithm guarantees that a path for
 * descent will exist
 *
 * Return
 * `step` = how many characters were went down
 */
int TSuffTree::go_one_edge(TState& st, char c, int depth) {
    //assert(!st.on_edge && has_way(c) && depth > 0);

    /* take appropriate edge */
    st.edge = nodes[st.node].edges[c];

    int step;
    if (edges[st.edge].length <= depth) {
        /* there are fewer characters on the edge than `depth` */
        /* go down only for length of the Edge (not `depth`) */
        st.node = edges[st.edge].dest_node;
        step = edges[st.edge].length;
    } else {
        /* there are more characters on the edge than `depth` */
        /* go down only for `depth` */
        st.on_edge = true;
        st.pos_on_edge = depth - 1;
        step = depth;
    }
    return step;
}
int TSuffTree::go_one_edge(char c, int depth) {
    return go_one_edge(state, c, depth);
}

/*
 * goes down in tree for `depth`
 * chars (the first of them is data[char_ind])
 *
 * this function assumes that the initial state is
 *                           state.on_edge = False
 * also, the Algorithm guarantees that a path for
 * descent will exist
 */
void TSuffTree::go_down(TState& st, int char_ind, int depth) {
    //assert(!state.on_edge && char_ind < data.size() && "bi bu ba");
    char c;
    while (depth > 0) {
        c = data[char_ind];
        int step = go_one_edge(st, c, depth);
        char_ind += step;
        depth -= step;
    }
    return;
}
void TSuffTree::go_down(int char_ind, int depth) {
    go_down(state, char_ind, depth);
}

/*
 * the Root doesn't have a suffix link
 * nothing needs to be done
 */
void TSuffTree::go_suff_link(TState& st) {
    assert(!st.on_edge);
    if (st.node == root) return;
    st.node = nodes[st.node].suff_link;
}
void TSuffTree::go_suff_link() {
    go_suff_link(state);
}

/*
 * creates a new Node between:
 * `state.node` and `state.edge.dest_node`
 *
 * Note:
 * 1) after execution, the `state.node` points
 *    to the same node as before.
 * 2) only one new edge is created, the second
 *    one just changes its parameters
 */
void TSuffTree::break_edge() {
    assert(state.on_edge);
    /* state.node -        - - - state.edge
     *             |      |
     *             |      |
     *             v      v    pos = ..
     * Before: parent ------------------> son
     *
     * After:  {parent} ------> me -----> son
     *             ^      ^
     *             |      |
     * state.node -        - - - state.edge
     */
    int son    = edges[state.edge].dest_node;
    int me     = create_node();
    int char_ind = edges[state.edge].begin_index + state.pos_on_edge + 1;
    char c = data[char_ind];
    nodes[me].edges[c] = create_edge(
            son, char_ind,
            edges[state.edge].length - state.pos_on_edge - 1);

    edges[state.edge].dest_node = me;
    edges[state.edge].length = state.pos_on_edge + 1;

    state.on_edge = false;
}

void TSuffTree::create_leaf(int parent, int char_ind) {
    int leaf = create_node();
    nodes[leaf].leaf_id = leaves_count++;

    /* acording to Alforithm set length = INF on Edge to the leaf */
    int e = create_edge(leaf, char_ind, INF);
    nodes[parent].edges[data[char_ind]] = e;
}

void TSuffTree::set_suff_link(int from, int to) {
    /* only if it's not there yet */
    if (from != -1 && nodes[from].suff_link == -1) {
        nodes[from].suff_link = to;
    }
}

void TSuffTree::build() {
    /* sequentially add one by one chars to the tree */
    for (size_t i = 0; i < data.size(); ++i) {
        add_part_of_str(i);
    }
}

/* the main function of the Algorithm */
/*
 * Ukkonen's algorithm is a linear-time for constructing Suffix Tree.
 * Add one char `C` to the existing tree from the last step:
 * Cases:
 *     - In current state we have way by `C`.
 *       (it doesn't matter if we are the Node or on the Edge)
 *       It means that all current suffixes ending with the `C` char
 *       has already been added earlier. Just do one step by this
 *       char `C` and stop.
 *     - In current state we have no way by `C`.
 *           1) We are in the Node:
 *              Create a new leaf by char `C`, than go through
 *              suffix link.
 *
 *           2) We are on the Edge:
 *              Breake the Edge and create a leaf from the new node,
 *              new node doesn't have suffix link yet, so we can't
 *              go through it. Trick - use your parent's* siffix link
 *              and than go down to the same depth as it was from
 *              the parent (state.pos_on_edge).
 *
 *              *if parent = root, just go down from root to (depth-1)
 *                                 Edge string but without first char.
 *        Than check all cases from the beginning.
 *        And always in a Node try to set suffix link from the last
 *        created Node.
 */
void TSuffTree::add_part_of_str(int current_len) {
    char a = data[current_len];
    int prev_node = -1;                // the last created node (to set its suff link in next step)

    while (true) {
        if (has_way(a)) {
            if (!state.on_edge) {
                /* try to set suff link */
                set_suff_link(prev_node, state.node);
            }
            go_one_char(a);
            break;
        }

        if (state.on_edge) {
            int char_ind = edges[state.edge].begin_index;
            int depth = state.pos_on_edge + 1;

            break_edge(); // new node has been created => try to set suff link from prev to new one
            set_suff_link(prev_node, edges[state.edge].dest_node);

            create_leaf(edges[state.edge].dest_node, current_len);
            prev_node = edges[state.edge].dest_node;  // strore new node to set suff link later

            if (state.node == root) {
                ++char_ind;
                --depth;
            }

            go_suff_link();            // is does nothing when state.node = root
            go_down(char_ind, depth);
        } else {
            set_suff_link(prev_node, state.node);
            create_leaf(state.node, current_len);
            if (state.node == root) {
                /* suffixes of all lengths have already been added in current phase */
                break;
            }
            go_suff_link();
        }
    }
}

void TSuffTree::print(int n, int h) {
    TNode node = nodes[n];
    for (auto x : node.edges) {
        TEdge e  = edges[x.second];
        for (int i = 0; i < (h - 1); ++i) {
            std::cout << "    ";
        }
        std::string tmp;
        for (int i = 0; i < e.length && e.begin_index + i < (int)data.size(); ++i) {
            tmp.push_back(data[i + e.begin_index]);
        }
        std::cout << "|-> {" << tmp <<", " << e.dest_node << ", " << nodes[e.dest_node].leaf_id <<  "}"<< "\n";
        print(e.dest_node, h + 1);
    }
}

void TSuffTree::Print() {
    print(root, 1);
    return;
}

std::vector<int> TSuffTree::find_occurrences(const std::string& s) {
    TState st;
    std::vector<int> res;
    if (find(s, st) == false) return res;
    if (st.on_edge) {
        st.node = edges[st.edge].dest_node;
    }
    // BFS
    std::queue<int> q;
    q.push(st.node);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        if (nodes[u].edges.empty()) {
            res.push_back(nodes[u].leaf_id);
            continue;
        }
        for (std::pair<char, int> x: nodes[u].edges) {
            q.push(edges[x.second].dest_node);
        }
    }
    return res;
}

bool TSuffTree::find(const std::string& s, TState& st) {
    st.node = root;
    st.on_edge = false;

    int n = s.size();
    int i = 0;
    while (i < n) {
        if (!has_way(st, s[i])) {
            return false;
        }
        go_one_char(st, s[i]);
        ++i;
    }
    return true;
}


