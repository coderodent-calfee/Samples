#pragma once
#include <queue>
#include <vector>



class CycleCheck {
public:
	void addEdge(int fromVert, int toVert) {
		checked_ = false;
		while (adjacencyList.size() <= (size_t)std::max(fromVert, toVert)) {
			adjacencyList.push_back(std::vector<int>());
		}
		adjacencyList[fromVert].push_back(toVert);
	}

	bool checkCycle(const int V, int E) {
		if (checked_) {
			return cyclic_;
		}
		checked_ = true;
		std::vector<int> indegrees(V);

		for (int i = 0; i < V; i++) {
			for (int x : adjacencyList[i])
			{
				indegrees[x] = indegrees[x] + 1;
			}
		}

		std::queue<int> zeroIndegrees;

		for (int i = 0; i < V; i++) {
			if (indegrees[i] == 0) {
				zeroIndegrees.push(i);
			}
		}
		int counter = 0;
		std::queue<int> sortedGraph;

		while (!zeroIndegrees.empty()) {
			int fromVertex = zeroIndegrees.front();
			zeroIndegrees.pop();
			sortedGraph.push(fromVertex);
			counter++;
			for (int toVertex : adjacencyList[fromVertex]) {
				indegrees[toVertex]--;
				if (indegrees[toVertex] == 0)
					zeroIndegrees.push(toVertex);
			}
		}
		cyclic_ = counter != V;// per Kahn algorithm

		// TODO: fromVertex is a topological sort
		
		return cyclic_; 
	}
private:
	bool checked_ = false;
	bool cyclic_ = false;
	std::vector<std::vector<int>> adjacencyList;

};
