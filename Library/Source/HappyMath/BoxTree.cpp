#include "HappyMath/BoxTree.h"

using namespace HappyMath;

//-------------------------------------- BoxTree --------------------------------------

BoxTree::BoxTree()
{
	this->minBoxVolume = 1.0;
}

/*virtual*/ BoxTree::~BoxTree()
{
}

void BoxTree::Reset(const AxisAlignedBoundingBox& boundingBox, double minBoxVolume)
{
	this->rootNode = std::make_shared<Node>();
	this->rootNode->boundingBox = boundingBox;
	this->minBoxVolume = minBoxVolume;
}

bool BoxTree::InsertObject(std::shared_ptr<Object> object)
{
	if (!this->rootNode.get())
		return false;

	AxisAlignedBoundingBox objectBox = object->GetMinimalBoundingBox();

	if (!this->rootNode->boundingBox.ContainsBox(objectBox))
		return false;

	Node* insertionNode = this->rootNode->FindInsertionNode(object.get(), objectBox, this);
	if (!insertionNode)
		return false;

	insertionNode->objectArray.push_back(object);
	return true;
}

bool BoxTree::RemoveObject(std::shared_ptr<Object> object)
{
	if (!this->rootNode.get())
		return false;

	AxisAlignedBoundingBox objectBox = object->GetMinimalBoundingBox();

	if (!this->rootNode->boundingBox.ContainsBox(objectBox))
		return false;

	Node* insertionNode = this->rootNode->FindInsertionNode(object.get(), objectBox, this);
	if (!insertionNode)
		return false;

	for (int i = 0; i < (int)insertionNode->objectArray.size(); i++)
	{
		if (insertionNode->objectArray[i].get() == object.get())
		{
			insertionNode->objectArray.erase(insertionNode->objectArray.begin() + i);
			return true;
		}
	}

	return false;
}

bool BoxTree::FindObjectsOverlappingBox(const AxisAlignedBoundingBox& box, std::vector<std::shared_ptr<Object>>& objectArray)
{
	if (!this->rootNode.get())
		return false;

	if (!this->rootNode->boundingBox.OverlapsAABB(box))
		return false;

	return this->rootNode->FindObjectsOverlappingBox(box, objectArray);
}

bool BoxTree::FindObjectsOverlappingSphere(const Vector3& center, double radius, std::vector<std::shared_ptr<Object>>& objectArray)
{
	if (!this->rootNode.get())
		return false;

	if (!this->rootNode->boundingBox.OverlapsSphere(center, radius))
		return false;

	return this->rootNode->FindObjectsOverlappingSphere(center, radius, objectArray);
}

std::shared_ptr<BoxTree::Object> BoxTree::FindClosestObjectToPoint(const Vector3& point, double* foundSquareDistance /*= nullptr*/)
{
	if (!this->rootNode.get())
		return nullptr;

	double squareDistance = 0.0;

	std::shared_ptr<Object> foundObject = this->rootNode->FindClosestObjectToPoint(point, squareDistance);

	if (foundSquareDistance)
		*foundSquareDistance = squareDistance;

	return foundObject;
}

void BoxTree::ForAllObjects(std::function<void(Object*)> objectFunc)
{
	if (this->rootNode.get())
		this->rootNode->ForAllObjects(objectFunc);
}

//-------------------------------------- BoxTree::Object --------------------------------------

BoxTree::Object::Object()
{
}

/*virtual*/ BoxTree::Object::~Object()
{
}

//-------------------------------------- BoxTree::Node --------------------------------------

BoxTree::Node::Node()
{
}

/*virtual*/ BoxTree::Node::~Node()
{
}

BoxTree::Node* BoxTree::Node::FindInsertionNode(Object* object, const AxisAlignedBoundingBox& objectBox, BoxTree* boxTree)
{
	if (this->boundingBox.GetVolume() <= boxTree->minBoxVolume)
		return this;

	if (!this->node[0].get() || !this->node[1].get())
	{
		this->node[0] = std::make_shared<Node>();
		this->node[1] = std::make_shared<Node>();

		this->boundingBox.Split(this->node[0]->boundingBox, this->node[1]->boundingBox);
	}

	for (int i = 0; i < 2; i++)
	{
		if (this->node[i]->boundingBox.ContainsBox(objectBox))
		{
			return this->node[i]->FindInsertionNode(object, objectBox, boxTree);
		}
	}

	return this;
}

bool BoxTree::Node::FindObjectsOverlappingBox(const AxisAlignedBoundingBox& box, std::vector<std::shared_ptr<Object>>& objectArray)
{
	for (std::shared_ptr<Object> object : this->objectArray)
	{
		if (object->GetMinimalBoundingBox().OverlapsAABB(box))
		{
			objectArray.push_back(object);
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (this->node[i] && this->node[i]->boundingBox.OverlapsAABB(box))
		{
			this->node[i]->FindObjectsOverlappingBox(box, objectArray);
		}
	}

	return objectArray.size() > 0;
}

bool BoxTree::Node::FindObjectsOverlappingSphere(const Vector3& center, double radius, std::vector<std::shared_ptr<Object>>& objectArray)
{
	for (std::shared_ptr<Object> object : this->objectArray)
	{
		if (object->GetMinimalBoundingBox().OverlapsSphere(center, radius) && object->OverlapsSphere(center, radius))
		{
			objectArray.push_back(object);
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (this->node[i] && this->node[i]->boundingBox.OverlapsSphere(center, radius))
		{
			this->node[i]->FindObjectsOverlappingSphere(center, radius, objectArray);
		}
	}

	return objectArray.size() > 0;
}

std::shared_ptr<BoxTree::Object> BoxTree::Node::FindClosestObjectToPoint(const Vector3& point, double& foundSquareDistance)
{
	std::shared_ptr<Object> foundObject;

	foundSquareDistance = std::numeric_limits<double>::max();

	if (this->node[0].get() && this->node[1].get())
	{
		double squareDistanceToNode[2]
		{
			this->node[0]->boundingBox.CalcShortestSquareDistanceToPoint(point),
			this->node[1]->boundingBox.CalcShortestSquareDistanceToPoint(point)
		};

		int i, j;

		if (squareDistanceToNode[0] < squareDistanceToNode[1])
		{
			i = 0;
			j = 1;
		}
		else
		{
			i = 1;
			j = 0;
		}

		double squareDistanceA = 0.0;
		std::shared_ptr<BoxTree::Object> objectA = this->node[i]->FindClosestObjectToPoint(point, squareDistanceA);
		if (squareDistanceA < squareDistanceToNode[j])
		{
			foundSquareDistance = squareDistanceA;
			foundObject = objectA;
		}
		else
		{
			double squareDistanceB = 0.0;
			std::shared_ptr<BoxTree::Object> objectB = this->node[j]->FindClosestObjectToPoint(point, squareDistanceB);
			if (squareDistanceB < squareDistanceA)
			{
				foundSquareDistance = squareDistanceB;
				foundObject = objectB;
			}

			foundSquareDistance = squareDistanceA;
			foundObject = objectA;
		}
	}

	for (std::shared_ptr<Object> object : this->objectArray)
	{
		double squareDistance = object->CalcSquareDistanceToPoint(point);
		if (squareDistance < foundSquareDistance)
		{
			foundSquareDistance = squareDistance;
			foundObject = object;
		}
	}

	return foundObject;
}

void BoxTree::Node::ForAllObjects(std::function<void(Object*)> objectFunc)
{
	for (int i = 0; i < (int)this->objectArray.size(); i++)
		objectFunc(this->objectArray[i].get());

	if (this->node[0])
		this->node[0]->ForAllObjects(objectFunc);

	if (this->node[1])
		this->node[1]->ForAllObjects(objectFunc);
}