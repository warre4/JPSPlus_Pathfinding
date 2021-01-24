#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	const std::vector<Line*>& lines{ m_pNavMeshPolygon->GetLines() };
	int nodeIdx{ 0 };
	
	for (const Line* pLine : lines)
	{
		int lineIdx{ pLine->index };

		if (m_pNavMeshPolygon->GetTrianglesFromLineIndex(lineIdx).size() == 2)
			AddNode(new NavGraphNode(nodeIdx++, lineIdx, pLine->p1 + (pLine->p2 - pLine->p1) / 2));
	}
	
	//2. Create connections now that every node is created
	const std::vector<Triangle*>& triangles{ m_pNavMeshPolygon->GetTriangles() };
	std::vector<int> indices;

	for (const Triangle* pTriangle : triangles)
	{
		indices.clear();
		
		for (int lineIdx : pTriangle->metaData.IndexLines)
		{
			if (lineIdx != -1)
			{
				int idx{ GetNodeIdxFromLineIdx(lineIdx) };

				if (idx != invalid_node_index)
					indices.push_back(idx);
			}
		}

		if (indices.size() == 2)
		{
			AddConnection(new GraphConnection2D(indices[0], indices[1]));
		}
		else
		{
			AddConnection(new GraphConnection2D(indices[0], indices[1]));
			AddConnection(new GraphConnection2D(indices[1], indices[2]));
			AddConnection(new GraphConnection2D(indices[2], indices[0]));
		}
	}
	
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();
}

