#pragma once

#include "HappyMath/PolygonMesh.h"
#include <functional>

namespace HappyMath
{
	/**
	 * These are duels of the typical graph that can be generated from a polygon mesh.
	 * Note that here, however, we are not independent of the polygon mesh.  The polygon
	 * mesh must stay in scope as long as the graph does.
	 * 
	 * Here, the vertices of the graph are the polygons of the mesh, and the edges
	 * of the graph are the adjacencies between the polygons.  You might use this graph,
	 * for example, to tri-strip a triangle mesh.
	 */
	class PolygonGraph
	{
	public:
		PolygonGraph();
		virtual ~PolygonGraph();

		class Node
		{
		public:
			Node();
			virtual ~Node();

			/**
			 * Note that this pointer can go stale if you're not careful!
			 */
			const PolygonMesh::Polygon* polygon;

			/**
			 * This array is parallel to the vertex array in the owned polygon.
			 */
			std::vector<Node*> adjacentNodeArray;
		};

		/**
		 * Delete all nodes in this graph, making it empty.
		 */
		void Clear();

		/**
		 * Clear this graph and then construct it as a function of the given mesh.
		 * 
		 * @return Failure can occur here if more than two polygons share an adjacency.
		 */
		bool Regenerate(const PolygonMesh& mesh, std::function<Node* ()> nodeFactory = []() -> Node* { return new Node(); });

		const std::vector<Node*> GetNodeArray() const { return this->nodeArray; }

	private:
		std::vector<Node*> nodeArray;
	};
}