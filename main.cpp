#include <iostream>
#include <algorithm>
#include <vector>
#include "suffix_tree.hpp"

using namespace std;

int main() {
    string text, pattern;
    cin >> text >> pattern;
    
    text += SENTINEL;
    TSuffTree tree(text);

    vector<int> ocs = tree.find_occurrences(pattern);
    sort(ocs.begin(), ocs.end());
    for (int pos: ocs) {
        cout << pos + 1 << " ";
    }
    cout << '\n';
    return 0;
}

