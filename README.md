# Longest Ski Track (DAG Longest Path)

## Project Description
The objective of this algorithm is to find the longest possible continuous ski track from a given set of downhill ski routes. 

Because all paths strictly go downhill (meaning you cannot ski in a loop), the intersections and routes form a **Directed Acyclic Graph (DAG)**. The mathematical equivalent of this task is finding the longest path within this graph.

## Solution & Algorithm
The problem is solved efficiently using **Topological Sorting** (a variation of Kahn's algorithm) combined with a dynamic programming approach.


# Text Editor Backend (Order Statistic Tree)

## Project Description
A highly efficient backend data structure for a text editor, supporting dynamic text manipulation (insert, delete, query) with strict logarithmic performance guarantees.

## Solution & Algorithm
Implemented using a custom **AVL Tree** functioning as an **Order Statistic Tree**.
* **Augmented Nodes:** Each node tracks `treeSize` (for fast index lookup) and `newsInTree` (for fast line calculation).
* **Self-Balancing:** Ensures O(log N) height after any structural modifications.
