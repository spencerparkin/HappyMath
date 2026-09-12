#pragma once

#include "HappyMath/Common.h"
#include "HappyMath/AxisAlignedBoundingBox.h"
#include <memory>
#include <functional>

namespace HappyMath
{
	/**
	 * This is a spacial-sorting data-structure that can be used to accelerate
	 * some questions about the space and the objects in it.  I think it has
	 * some weaknesses, but it can be beneficial.
	 */
	class HAPPY_MATH_API BoxTree
	{
		friend class Node;

	public:
		BoxTree();
		virtual ~BoxTree();

		/**
		 * These are objects that can be placed in the tree.
		 */
		class Object
		{
		public:
			Object();
			virtual ~Object();

			/**
			 * The smallest possible box containing the objects should be returned here.
			 */
			virtual AxisAlignedBoundingBox GetMinimalBoundingBox() const = 0;

			/**
			 * Tell the caller if this object overlaps the given sphere.  Note that this
			 * function is not called unless a check against this object's bounding box
			 * and the sphere passes first.
			 */
			virtual bool OverlapsSphere(const Vector3& center, double radius) const = 0;

			/**
			 * Calculate and return the shortest square distance from the given point to this object.
			 */
			virtual double CalcSquareDistanceToPoint(const Vector3& point) const = 0;
		};

		/**
		 * 
		 */
		void Reset(const AxisAlignedBoundingBox& boundingBox, double minBoxVolume);

		/**
		 * 
		 */
		bool InsertObject(std::shared_ptr<Object> object);

		/**
		 * 
		 */
		bool RemoveObject(std::shared_ptr<Object> object);

		/**
		 * Quickly find and return all objects in this tree that overlap the given AABB.
		 */
		bool FindObjectsOverlappingBox(const AxisAlignedBoundingBox& box, std::vector<std::shared_ptr<Object>>& objectArray);

		/**
		 * Quickly find and return all objects in this tree that overlap the given sphere.
		 */
		bool FindObjectsOverlappingSphere(const Vector3& center, double radius, std::vector<std::shared_ptr<Object>>& objectArray);

		/**
		 * Quickly find and return the object in this tree that is closest to the given point.
		 * If there is more than one such object, which we return is left undefined.
		 */
		std::shared_ptr<Object> FindClosestObjectToPoint(const Vector3& point, double* foundSquareDistance = nullptr);

		/**
		 * Iterate through all objects of this tree.
		 */
		void ForAllObjects(std::function<void(Object*)> objectFunc);

	private:

		/**
		 * These are the nodes that make up the box-tree.  Each represents a sub-region of space.
		 */
		class Node
		{
		public:
			Node();
			virtual ~Node();

			Node* FindInsertionNode(Object* object, const AxisAlignedBoundingBox& objectBox, BoxTree* boxTree);
			bool FindObjectsOverlappingBox(const AxisAlignedBoundingBox& box, std::vector<std::shared_ptr<Object>>& objectArray);
			bool FindObjectsOverlappingSphere(const Vector3& center, double radius, std::vector<std::shared_ptr<Object>>& objectArray);
			std::shared_ptr<Object> FindClosestObjectToPoint(const Vector3& point, double& foundSquareDistance);
			void ForAllObjects(std::function<void(Object*)> objectFunc);

			std::vector<std::shared_ptr<Object>> objectArray;
			std::shared_ptr<Node> node[2];
			AxisAlignedBoundingBox boundingBox;
		};

		std::shared_ptr<Node> rootNode;
		double minBoxVolume;
	};
}