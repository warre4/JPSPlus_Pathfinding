#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		std::vector<T_NodeType*> path;
		std::vector<NodeRecord> openList;
		std::vector<NodeRecord> closedList;
		NodeRecord currentRecord;

		NodeRecord startRecord;
		startRecord.pNode = pStartNode;
		startRecord.pConnection = nullptr;
		startRecord.costSoFar = 0;
		startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);

		openList.push_back(startRecord);

		while(!openList.empty())
		{
			currentRecord = *std::min_element(openList.begin(), openList.end());

			if (currentRecord.pNode == pGoalNode)
				break;

			for (T_ConnectionType* pConnection : m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex()))
			{
				float gCost = currentRecord.costSoFar + pConnection->GetCost();
				T_NodeType* pNextNode = m_pGraph->GetNode(pConnection->GetTo());

				std::vector<NodeRecord>::iterator nextNodeClosedIt = std::find_if(closedList.begin(), closedList.end(),
					[pNextNode](const NodeRecord& nodeRecord)
					{
						return nodeRecord.pNode == pNextNode;
					});

				std::vector<NodeRecord>::iterator nextNodeOpenIt = std::find_if(openList.begin(), openList.end(),
					[pNextNode](const NodeRecord& nodeRecord)
					{
						return nodeRecord.pNode == pNextNode;
					});

				if (nextNodeClosedIt != closedList.end()) //pNextNode is in closedList
				{
					if (nextNodeClosedIt->costSoFar <= gCost)
						continue;
					else
						closedList.erase(std::remove(closedList.begin(), closedList.end(), *nextNodeClosedIt), closedList.end());
				}
				else if (nextNodeOpenIt != openList.end()) //pNextNode is in openList
				{
					if (nextNodeOpenIt->costSoFar <= gCost)
						continue;
					else
						openList.erase(std::remove(openList.begin(), openList.end(), *nextNodeOpenIt), openList.end());
				}

				NodeRecord newNodeRecord;
				newNodeRecord.pNode = pNextNode;
				newNodeRecord.pConnection = pConnection;
				newNodeRecord.costSoFar = gCost;
				newNodeRecord.estimatedTotalCost = gCost + GetHeuristicCost(pNextNode, pGoalNode);

				openList.push_back(newNodeRecord);
			}

			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord), openList.end());
			closedList.push_back(currentRecord);
		}

		//path found?
		if (currentRecord.pNode != pGoalNode)
			return path;
		
		//BackTracking
		while (currentRecord.pNode != pStartNode)
		{
			path.push_back(currentRecord.pNode);
			currentRecord = *std::find_if(closedList.begin(), closedList.end(),
				[currentRecord, this](const NodeRecord& nodeRecord)
				{
					return nodeRecord.pNode == m_pGraph->GetNode(currentRecord.pConnection->GetFrom());
				});
		}

		path.push_back(pStartNode);
		
		std::reverse(path.begin(), path.end());

		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}