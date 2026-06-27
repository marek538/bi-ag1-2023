# Longest Ski Track (DAG Longest Path)

## Project Description
The objective of this algorithm is to find the longest possible continuous ski track from a given set of downhill ski routes. 

Because all paths strictly go downhill (meaning you cannot ski in a loop), the intersections and routes form a **Directed Acyclic Graph (DAG)**. The mathematical equivalent of this task is finding the longest path within this graph.

## Solution & Algorithm
The problem is solved efficiently using **Topological Sorting** (a variation of Kahn's algorithm) combined with a dynamic programming approach.
