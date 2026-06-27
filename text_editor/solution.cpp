#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Point : size_t {};

struct Path {
  Point from, to;
  unsigned length;

  Path(size_t f, size_t t, unsigned l) : from{f}, to{t}, length{l} {}

  friend bool operator == (const Path& a, const Path& b) {
    return std::tie(a.from, a.to, a.length) == std::tie(b.from, b.to, b.length);
  }
  
  friend bool operator != (const Path& a, const Path& b) { return !(a == b); }
};

#endif
using namespace std;

void mapAllPathsAndSources(vector<vector<Path>> &myAllRoutes, const vector<Path>&  all_paths, queue<Point> &toVisit, vector<pair<uint, pair<uint, uint>>> &visited, size_t points)
{
    vector<int> numOfPredecessors(points, 0);
    for(auto path : all_paths)
    {
        numOfPredecessors[path.to]++;
        myAllRoutes[path.from].push_back({path});
    }

    for(uint i = 0; i < numOfPredecessors.size(); i++)
    {
        if(numOfPredecessors[i] == 0)
        {
            toVisit.push(Point(i));
            visited[i] = make_pair(0, make_pair(i,0));
        }
    }
}

std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths)
{
    vector<vector<Path>> myAllRoutes(points); //points with paths from them

    queue<Point> toVisit;
    vector<pair<uint, pair<uint, uint>>> visited(points, {0,{0,0}}); //currentPoint (distance to get here, pair(predecesor, route))

    mapAllPathsAndSources(myAllRoutes, all_paths, toVisit, visited, points);
    //TODO WORKS

    uint maxLength = 0;
    Point maxReached = Point(SIZE_MAX);

    while(!toVisit.empty())
    {
        Point current = toVisit.front();
        toVisit.pop();

        //auto neighbours = myAllRoutes.equal_range(current);
        //for(auto it = neighbours.first; it != neighbours.second; it++)

        for(uint num = 0; num < myAllRoutes[current].size(); num++)
        {
             if(visited[myAllRoutes[current][num].to].first == 0 || visited[myAllRoutes[current][num].to].first < visited[current].first + myAllRoutes[current][num].length)
            //either it has not been reached yet or it was with smaller distance
            {
                auto o = visited[current].first + myAllRoutes[current][num].length;
                visited[myAllRoutes[current][num].to] = make_pair(o, make_pair(current, myAllRoutes[current][num].length));
                toVisit.push(myAllRoutes[current][num].to);
                if((visited[current].first + myAllRoutes[current][num].length) > maxLength)
                {
                    maxLength = visited[current].first + myAllRoutes[current][num].length;
                    maxReached = myAllRoutes[current][num].to;
                }
            }

        }
    }

    vector<Path> result;
    result.emplace_back(visited[maxReached].second.first, maxReached, visited[maxReached].second.second);
    uint current = visited[maxReached].second.first;
    while(Point(current) != visited[Point(current)].second.first)
    {
        result.emplace_back(visited[Point(current)].second.first, current, visited[Point(current)].second.second);
        current = visited[Point(current)].second.first;
    }

    reverse(result.begin(), result.end());

    return result;
}


#ifndef __PROGTEST__


struct Test {
  unsigned longest_track;
  size_t points;
  std::vector<Path> all_paths;
};

inline const Test TESTS[] = {
  {13, 5, { {3,2,10}, {3,0,9}, {0,2,3}, {2,4,1} } },
  {11, 5, { {3,2,10}, {3,1,4}, {1,2,3}, {2,4,1} } },
  {16, 8, { {3,2,10}, {3,1,1}, {1,2,3}, {1,4,15} } },
  {5,2,{{0,1,5},{0,1,4}}},
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Fail: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool run_test(const Test& t) {
  auto sol = longest_track(t.points, t.all_paths);

  unsigned length = 0;
  for (auto [ _, __, l ] : sol) length += l;

  CHECK(t.longest_track == length,
    "Wrong length: got %u but expected %u", length, t.longest_track);

  for (size_t i = 0; i < sol.size(); i++) {
    CHECK(std::count(t.all_paths.begin(), t.all_paths.end(), sol[i]),
      "Solution contains non-existent path: %zu -> %zu (%u)",
      sol[i].from, sol[i].to, sol[i].length);

    if (i > 0) CHECK(sol[i].from == sol[i-1].to,
      "Paths are not consecutive: %zu != %zu", sol[i-1].to, sol[i].from);
  }

  return true;
}
#undef CHECK

int main()
{
    //TEST[0] {13, 5, { {3,2,10}, {3,0,9}, {0,2,3}, {2,4,1} } },

    if(!run_test(TESTS[3]))
        cout << "bad" << endl;

  int ok = 0, fail = 0;

  for (auto&& t : TESTS) (run_test(t) ? ok : fail)++;

  if (!fail) printf("Passed all %i tests!\n", ok);
  else printf("Failed %u of %u tests.\n", fail, fail + ok);
}

#endif


