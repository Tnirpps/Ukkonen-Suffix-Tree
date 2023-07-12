# Ukkonen-Suffix-Tree
Suffix tree (also called PAT tree or, in an earlier form, position tree) is a compressed trie containing all the suffixes of the given text as their keys and positions in the text as their values.

Example for **aboba$** :
![image](https://github.com/Tnirpps/Ukkonen-Suffix-Tree/assets/87409111/3705e9fb-2352-4044-9b8f-a77b39dd7c68)

Naive construction method have $`O(n^2) `$ space and time complexity.

The Ukkonen algorithm is implemented here, which allows you to build a tree $`O(n) `$ space and time complexity (*assuming that the size of the alphabet is bounded by a constant*).
