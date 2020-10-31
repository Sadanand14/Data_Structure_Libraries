#pragma once
// NOTE: No includes should be added to this file.
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdexcept>

enum class Color { BLACK, RED};

template<typename T, typename F>
class PreAllocated_SimpleDictionary;

template<typename T, typename F>
class BinarySearchNode
{
    //private data
	T m_key{};
	F m_data{};
    Color color;
    int m_parent, m_left, m_right;

    friend class PreAllocated_SimpleDictionary<T,F>;

public:
    //public data


    //public interface
    inline const char* GetKey()const { return m_key; }

    BinarySearchNode();

    ~BinarySearchNode();

    BinarySearchNode& operator=(const BinarySearchNode&);
};

//Simple Dictionary Implemented as a Red-Black Tree
template<typename T, typename F>
class PreAllocated_SimpleDictionary
{

	BinarySearchNode<T, F>* m_nodePool = nullptr;
	int m_rootNode{};
	size_t m_size{}, m_capacity{};

    const size_t m_startingCapacity = 8;

    void RebalanceTree(int RootNode, int insertedNode);
    int RotateSubTreeToLeft(int TreeRoot);
    int RotateSubTreeToRight(int TreeRoot);
    int SetForDeletion(int nodeID) const;
    int InorderSuccessor(int nodeID) const;
    void FixAndDelete(int nodeID);
    void FixDoubleBlack(int node);
    void ClearNode(int node);
    int GetOnlyChild(int nodeID) const;
    bool IsLeftChild(int nodeID) const;
    bool HasRedChild(int nodeID) const;
    int GetSibling(int nodeID) const;
    bool SiblingHasRedChild(int nodeID) const;
    bool SiblingIsBlack(int nodeID) const;
    void InorderHelper(int nodeID) const;
    int Insert(int rootNode, int nodeToInsert, int parentNode);
    int Search(int rootNode, T key) const;
    void Allocate();

public:
    PreAllocated_SimpleDictionary();
    ~PreAllocated_SimpleDictionary();

    void PrintAll() { InorderHelper(m_rootNode); }
    inline int GetRootIndex() const { return m_rootNode; }
    
    // Disabled move/copy constructors and move/assignment operators.
    // These do not need to be implemented.
    PreAllocated_SimpleDictionary(PreAllocated_SimpleDictionary&&) = delete;
    PreAllocated_SimpleDictionary(const PreAllocated_SimpleDictionary&) = delete;
    PreAllocated_SimpleDictionary& operator=(PreAllocated_SimpleDictionary&&) = delete;
    PreAllocated_SimpleDictionary& operator=(const PreAllocated_SimpleDictionary&) = delete;


    bool Add(T pKey, F pValue);

    bool Remove(T pKey);

    bool Find(T pKey, F* pOutValue) const;

    //interface
    F operator[](T key) const;
};

template<typename T, typename F>
BinarySearchNode<T, F>::BinarySearchNode(): m_left(-1), m_right(-1), m_parent(-1)
{

}

template<typename T, typename F>
BinarySearchNode<T, F>::~BinarySearchNode()
{

}

template<typename T, typename F>
BinarySearchNode<T, F>& BinarySearchNode<T, F>::operator=(const BinarySearchNode<T, F>& otherNode) 
{
    m_key = otherNode.m_key;
    m_data = otherNode.m_data;
    return *this;
}


template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::InorderHelper(int nodeID)const
{
    if (nodeID == -1)
        return;

    InorderHelper(m_nodePool[nodeID].m_left);
    printf("%s\t", m_nodePool[nodeID].m_key);
    InorderHelper(m_nodePool[nodeID].m_right);

    //std::cout << "\n";
}


template<typename T, typename F>
PreAllocated_SimpleDictionary<T, F>::PreAllocated_SimpleDictionary() : m_rootNode(-1), m_nodePool(nullptr), m_capacity(0), m_size(0)
{
    Allocate();
}

template<typename T, typename F>
PreAllocated_SimpleDictionary<T, F>::~PreAllocated_SimpleDictionary()
{
    delete[] m_nodePool;
}


template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::Allocate()
{
    if (m_nodePool == nullptr)
    {
        m_nodePool = new BinarySearchNode<T, F>[m_startingCapacity];
        m_capacity = m_startingCapacity;
    }
    else
    {
        // allocate a new array of node twice the previous capacity
        BinarySearchNode<T,F>* newArray = new BinarySearchNode<T,F>[m_capacity * 2];

        //copy over the contents of the node
        memcpy(newArray, m_nodePool, sizeof(BinarySearchNode<T,F>) * m_capacity);

        //delete old array
        delete[] m_nodePool;

        //assign to nodePool
        m_nodePool = newArray;
        m_capacity *= 2;
    }
}

template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::Add(T pKey, F pValue)
{
    //return false if any of the arguments are invalid
	/*if (pKey == std::default || pValue == std::default)
        return false;*/


    //if key already exists, deny insertion.
    if (Search(m_rootNode, pKey) != -1)
    {
        return false;
    }


    if (m_size == m_capacity)
        Allocate();

    int nodeID = m_size++;

    //provided node should not have a key assigned
    assert(m_nodePool[nodeID].m_key == nullptr);


    m_nodePool[nodeID].m_key = pKey;
    m_nodePool[nodeID].m_data = pValue;


    if (m_rootNode == -1)
    {
        m_rootNode = nodeID;
        RebalanceTree(m_rootNode, nodeID);
        return true;
    }

    // insert
    Insert(m_rootNode, nodeID, -1);

    // rebalance
    RebalanceTree(m_rootNode, nodeID);
    return true;
}


template<typename T, typename F>
F PreAllocated_SimpleDictionary<T, F>::operator[](const T key) const
{
	int node = Search(m_rootNode, key);
	if (node == -1)
	{
		throw;
	}

	return m_nodePool[node].m_data;
}



template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::IsLeftChild(int nodeID)const
{
	assert(nodeID != -1 && m_nodePool[nodeID].m_parent != -1);

	return m_nodePool[m_nodePool[nodeID].m_parent].m_left == nodeID;
}

template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::HasRedChild(int nodeID)const
{
	assert(nodeID != -1);

	if (((m_nodePool[nodeID].m_left != -1) && (m_nodePool[m_nodePool[nodeID].m_left].color == Color::RED)) || ((m_nodePool[nodeID].m_right != -1) && (m_nodePool[m_nodePool[nodeID].m_right].color == Color::RED)))
		return true;
	else
		return false;
}

template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::GetSibling(int nodeID)const
{
	assert(nodeID != -1);

	if (m_nodePool[nodeID].m_parent == -1)
		return -1;

	if (IsLeftChild(nodeID))
		return m_nodePool[m_nodePool[nodeID].m_parent].m_right;
	else
		return m_nodePool[m_nodePool[nodeID].m_parent].m_left;
}

template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::SiblingHasRedChild(int nodeID)const
{
	assert((nodeID != -1) && (m_nodePool[nodeID].m_parent != -1));

	if ((m_nodePool[m_nodePool[nodeID].m_parent].m_left == -1) || (m_nodePool[m_nodePool[nodeID].m_parent].m_right == -1))
	{
		return false;
	}

	//Get sibling
	int SiblingID = GetSibling(nodeID);

	//See if it has a red Child
	if (((m_nodePool[SiblingID].m_left != -1) && (m_nodePool[m_nodePool[SiblingID].m_left].color == Color::RED))
		|| ((m_nodePool[SiblingID].m_right != -1) && (m_nodePool[m_nodePool[SiblingID].m_right].color == Color::RED)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::SiblingIsBlack(int nodeID)const
{
	assert((nodeID != -1) && (m_nodePool[nodeID].m_parent != -1));

	int siblingID = GetSibling(nodeID);

	//assume a null sibling as black
	if (siblingID == -1)
		return true;
	else
		return m_nodePool[siblingID].color == Color::BLACK;
}

template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::Insert(int rootNode, int nodeToInsert, int parentNode)
{
	//assert(nodeToInsert && m_rootNode );

	//if main rootnode is empty then we assign it.
	if (rootNode == -1)
	{
		m_nodePool[nodeToInsert].m_parent = parentNode;
		return nodeToInsert;
	}

	//if node under consideration has a key smaller than insertion Key
	if (m_nodePool[nodeToInsert].m_key < m_nodePool[rootNode].m_key)
	{
		m_nodePool[rootNode].m_left = Insert(m_nodePool[rootNode].m_left, nodeToInsert, rootNode);
	}
	else
	{
		m_nodePool[rootNode].m_right = Insert(m_nodePool[rootNode].m_right, nodeToInsert, rootNode);
	}

	return rootNode;
}

template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::Search(int nodeID, T key)const
{
	if (nodeID == -1)
		return -1;

	if (m_nodePool[nodeID].m_key == key)
		return nodeID;

	if (key < m_nodePool[nodeID].m_key)
		return Search(m_nodePool[nodeID].m_left, key);
	else
		return Search(m_nodePool[nodeID].m_right, key);
}

template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::InorderSuccessor(int nodeID)const
{
	if (nodeID == -1)
		return -1;

	//only time this gets called is when deleting node with 2 children
	assert(m_nodePool[nodeID].m_right != -1);

	//look for the next larger value on the right child side 
	int nodeNeeded = m_nodePool[nodeID].m_right;
	while (m_nodePool[nodeNeeded].m_left != -1)
	{
		nodeNeeded = m_nodePool[nodeNeeded].m_left;
	}
	return nodeNeeded;
}


template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::GetOnlyChild(int nodeID)const
{
	assert(m_nodePool[nodeID].m_left != -1 || m_nodePool[nodeID].m_right != -1);

	if (m_nodePool[nodeID].m_left != -1)
		return m_nodePool[nodeID].m_left;
	else
		return m_nodePool[nodeID].m_right;
}

template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::FixAndDelete(int nodeID)
{
	if (nodeID == -1)
	{
		return;
	}

	//ensure the node to be deleted doesnt have two children
	assert(m_nodePool[nodeID].m_left == -1 || m_nodePool[nodeID].m_right == -1);

	bool isRootNode = (nodeID == m_rootNode);

	
	BinarySearchNode<T,F> node = m_nodePool[m_rootNode];


	//single child Case
	if (m_nodePool[nodeID].m_left != -1 || m_nodePool[nodeID].m_right != -1)
	{
		bool redChild = HasRedChild(nodeID);
		//if child and node are of opposite color. Simply delete node and make child node black
		if (redChild || (!redChild && m_nodePool[nodeID].color == Color::RED))
		{
			//delete and replace with child and change its color to black.
			int onlyChild = GetOnlyChild(nodeID);
			m_nodePool[onlyChild].m_parent = m_nodePool[nodeID].m_parent;
			m_nodePool[onlyChild].color = Color::BLACK;

			//replace child with node to be deleted
			if (!isRootNode)
			{
				if (IsLeftChild(nodeID))
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_left = onlyChild;
				}
				else
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_right = onlyChild;
				}
				//m_nodePool[onlyChild].m_parent = m_nodePool[nodeID].m_parent;
			}
			else
			{
				m_rootNode = onlyChild;
			}

			//Clear the deleted node
			ClearNode(nodeID);
		}
		//both node and single child are black
		else
		{
			//first rearrange heirarchy for safe deletion
			FixDoubleBlack(nodeID);

			//replace child with the node to be deleted
			int onlyChild = GetOnlyChild(nodeID);
			if (!isRootNode)
			{
				if (IsLeftChild(nodeID))
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_left = onlyChild;
				}
				else
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_right = onlyChild;
				}
				//m_nodePool[onlyChild].m_parent = m_nodePool[nodeID].m_parent;
			}
			else
			{
				m_rootNode = onlyChild;
			}

			//empty the node for returning to pool
			ClearNode(nodeID);
		}
	}
	//has no child (is leaf node)
	else
	{
		//but is red Node
		if (m_nodePool[nodeID].color == Color::RED)
		{
			if (!isRootNode)
			{
				if (IsLeftChild(nodeID))
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_left = -1;
				}
				else
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_right = -1;
				}
			}
			else
			{
				m_rootNode = -1;
			}

			//Clear the deleted node
			ClearNode(nodeID);

		}
		//is a black node with no children (considered same as black node with single black child)
		else
		{



			//first rearrange heirarchy for safe deletion
			FixDoubleBlack(nodeID);



			//detatch from parent
			if (!isRootNode)
			{
				if (IsLeftChild(nodeID))
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_left = -1;
				}
				else
				{
					m_nodePool[m_nodePool[nodeID].m_parent].m_right = -1;
				}
			}
			else
			{
				m_rootNode = -1;
			}

			ClearNode(nodeID);
		}
	}
}

template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::FixDoubleBlack(int nodeID)
{
	assert(nodeID != -1);
	int X = nodeID, S;
	bool isOnLeft;
	while (X != m_rootNode && m_nodePool[X].color == Color::BLACK)
	{
		S = GetSibling(X);
		if (IsLeftChild(X))
		{
			//if Sibling exists and has red color
			if ((S != -1) && (m_nodePool[S].color == Color::RED))
			{
				m_nodePool[S].color = Color::BLACK;
				m_nodePool[m_nodePool[S].m_parent].color = Color::RED;

				RotateSubTreeToLeft(m_nodePool[X].m_parent);

				//update sibling
				S = m_nodePool[m_nodePool[X].m_parent].m_right;
			}

			//if sibling exisits and doesnt have red children
			if (!SiblingHasRedChild(X))
			{
				m_nodePool[S].color = Color::RED;
				X = m_nodePool[X].m_parent;
			}
			else
			{
				if ((m_nodePool[S].m_right == -1) || (m_nodePool[m_nodePool[S].m_right].color == Color::BLACK))
				{
					m_nodePool[m_nodePool[S].m_left].color = Color::BLACK;
					m_nodePool[S].color = Color::RED;
					RotateSubTreeToRight(S);

					//update Sibling
					S = m_nodePool[m_nodePool[X].m_parent].m_right;
				}

				m_nodePool[S].color = m_nodePool[m_nodePool[X].m_parent].color;
				m_nodePool[m_nodePool[X].m_parent].color = Color::BLACK;
				m_nodePool[m_nodePool[S].m_right].color = Color::BLACK;
				RotateSubTreeToLeft(m_nodePool[X].m_parent);

				//make sure to end the loop
				X = m_rootNode;
			}

		}
		else
		{
			if ((S != -1) && (m_nodePool[S].color == Color::RED))
			{
				m_nodePool[S].color = Color::BLACK;
				m_nodePool[m_nodePool[S].m_parent].color = Color::RED;
				RotateSubTreeToRight(m_nodePool[X].m_parent);

				//update Sibling
				S = m_nodePool[m_nodePool[X].m_parent].m_left;
			}


			if (!SiblingHasRedChild(X))
			{
				m_nodePool[S].color = Color::RED;
				X = m_nodePool[X].m_parent;
			}
			else
			{
				if (m_nodePool[S].m_left == -1 || m_nodePool[m_nodePool[S].m_left].color == Color::BLACK)
				{
					m_nodePool[m_nodePool[S].m_right].color = Color::BLACK;
					m_nodePool[S].color = Color::RED;
					RotateSubTreeToLeft(S);

					//update sibling
					S = m_nodePool[m_nodePool[X].m_parent].m_right;
				}

				m_nodePool[S].color = m_nodePool[m_nodePool[X].m_parent].color;
				m_nodePool[m_nodePool[X].m_parent].color = Color::BLACK;
				m_nodePool[m_nodePool[S].m_left].color = Color::BLACK;
				RotateSubTreeToRight(m_nodePool[X].m_parent);

				//make sure to end the loop
				X = m_rootNode;
			}
		}
	}
	m_nodePool[X].color = Color::BLACK;
}


//function to empty this node and return it to the pool 
template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::ClearNode(int nodeID)
{
	if (nodeID != (m_size - 1))
	{
		//shift all data from last used node in the array to the node to be deleted.
		m_nodePool[nodeID].color = m_nodePool[m_size - 1].color;
		m_nodePool[nodeID].m_key = m_nodePool[m_size - 1].m_key;
		m_nodePool[nodeID].m_parent = m_nodePool[m_size - 1].m_parent;
		m_nodePool[nodeID].m_left = m_nodePool[m_size - 1].m_left;
		m_nodePool[nodeID].m_right = m_nodePool[m_size - 1].m_right;

		//if parent exists, update its link to new position
		if (m_nodePool[nodeID].m_parent != -1)
		{
			if (IsLeftChild(m_size - 1))
			{
				m_nodePool[m_nodePool[nodeID].m_parent].m_left = nodeID;
			}
			else
			{
				m_nodePool[m_nodePool[nodeID].m_parent].m_right = nodeID;
			}
		}

		//if left child exists, update its link to new position
		if (m_nodePool[nodeID].m_left != -1)
		{
			m_nodePool[m_nodePool[nodeID].m_left].m_parent = nodeID;
		}

		//if right child exists, update its link to new position
		if (m_nodePool[nodeID].m_right != -1)
		{
			m_nodePool[m_nodePool[nodeID].m_right].m_parent = nodeID;
		}
	}


	//empty the last node
	m_nodePool[m_size - 1].color = Color::RED;
	m_nodePool[m_size - 1].m_key = nullptr;
	m_nodePool[m_size - 1].m_parent = -1;
	m_nodePool[m_size - 1].m_left = -1;
	m_nodePool[m_size - 1].m_right = -1;


	--m_size;
}

template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::SetForDeletion(int nodeID) const
{
	if (nodeID == -1)
		return -1;

	if (m_nodePool[nodeID].m_left != -1 && m_nodePool[nodeID].m_right != -1)
	{
		int inorderSucc = InorderSuccessor(nodeID);
		m_nodePool[nodeID] = m_nodePool[inorderSucc];
		return inorderSucc;
	}
	else
	{
		return nodeID;
	}
}

template<typename T, typename F>
void PreAllocated_SimpleDictionary<T, F>::RebalanceTree(int RootNode, int insertedNode)
{
	if (insertedNode == -1 || RootNode == -1)
		return;


	int currentNode = insertedNode;
	int parent, grandParent;

	parent = m_nodePool[currentNode].m_parent;
	//temporary holder for swapping
	Color color;

	while ((currentNode != RootNode) && (m_nodePool[parent].color == Color::RED))
	{
		//get parent and grandparents
		parent = m_nodePool[insertedNode].m_parent;
		grandParent = m_nodePool[parent].m_parent;

		//parent is left child of grand parent
		if (IsLeftChild(parent))
		{
			//both parent and uncle are red
			if ((m_nodePool[grandParent].m_right != -1) && (m_nodePool[m_nodePool[grandParent].m_right].color == Color::RED))
			{
				m_nodePool[grandParent].color = Color::RED;
				m_nodePool[m_nodePool[grandParent].m_right].color = Color::BLACK;
				m_nodePool[parent].color = Color::BLACK;
				currentNode = m_nodePool[m_nodePool[currentNode].m_parent].m_parent;
			}
			//if uncle is black
			else
			{
				// Left Right CASE
				if (currentNode == m_nodePool[parent].m_right)
				{
					RotateSubTreeToLeft(parent);
					currentNode = parent;
					parent = m_nodePool[currentNode].m_parent;
				}

				//Left Left case
				//swap colors between parent and grand parent and then do a right rotate across grandparent
				color = m_nodePool[grandParent].color;
				m_nodePool[grandParent].color = m_nodePool[parent].color;
				m_nodePool[parent].color = color;
				RotateSubTreeToRight(grandParent);
			}
		}
		//parent is right child of grand parent
		else
		{
			// both parent and uncle are RED
			if ((m_nodePool[grandParent].m_left != -1) && (m_nodePool[m_nodePool[grandParent].m_left].color == Color::RED))
			{
				m_nodePool[grandParent].color = Color::RED;
				m_nodePool[m_nodePool[grandParent].m_left].color = Color::BLACK;
				m_nodePool[parent].color = Color::BLACK;

				currentNode = m_nodePool[m_nodePool[currentNode].m_parent].m_parent;
			}
			// if uncle is black
			else
			{
				// Right Left Case
				if (currentNode == m_nodePool[parent].m_left)
				{
					RotateSubTreeToRight(parent);
					currentNode = parent;
					parent = m_nodePool[currentNode].m_parent;
				}

				//Right Right Case
				//swap colors between parent and grand parent and then do a left rotate across grandparent
				color = m_nodePool[grandParent].color;
				m_nodePool[grandParent].color = m_nodePool[parent].color;
				m_nodePool[parent].color = color;
				RotateSubTreeToLeft(grandParent);
			}
		}
	}

	if (currentNode == RootNode)
	{
		//++m_blackHeight;
		m_nodePool[currentNode].color = Color::BLACK;
	}
}

//Rotates the SubTree to left if possible and returns the new rootnode for that subtree
template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::RotateSubTreeToLeft(int TreeRoot)
{
	if (TreeRoot == -1)
		return -1;

	//index to right child of root
	int pt_RC = m_nodePool[TreeRoot].m_right;

	if (pt_RC == -1)
		return TreeRoot; //nothing on the right to rotate to left.

	//attach left child of right child of root as right child of root
	m_nodePool[TreeRoot].m_right = m_nodePool[pt_RC].m_left;

	if (m_nodePool[pt_RC].m_left != -1)
	{
		m_nodePool[m_nodePool[pt_RC].m_left].m_parent = TreeRoot;
	}

	//attach old right child of treeroot to parent of treeroot
	m_nodePool[pt_RC].m_parent = m_nodePool[TreeRoot].m_parent;

	//if root had a parent
	if (m_nodePool[TreeRoot].m_parent != -1)
	{
		//change the child of that parent from tree root to tree roots leftchild
		if (IsLeftChild(TreeRoot))
			m_nodePool[m_nodePool[TreeRoot].m_parent].m_left = pt_RC;
		//change the child of that parent from tree root to tree roots rightchild
		else
			m_nodePool[m_nodePool[TreeRoot].m_parent].m_right = pt_RC;
	}
	else
	{
		m_rootNode = pt_RC;
	}

	// Assign treeRoot as the new left Child of the old treeroot right child.
	m_nodePool[TreeRoot].m_parent = pt_RC;
	m_nodePool[pt_RC].m_left = TreeRoot;

	return pt_RC;
}

//Rotates the SubTree to right if possible and returns the new rootnode for that subtree (no change in color)
template<typename T, typename F>
int PreAllocated_SimpleDictionary<T, F>::RotateSubTreeToRight(int TreeRoot)
{
	if (TreeRoot == -1)
		return TreeRoot;



	//pt to left child of root
	int pt_LC = m_nodePool[TreeRoot].m_left;

	if (pt_LC == -1)
		return TreeRoot; //nothing on the left to rotate to right

	//attach right child of the left child of root as left child of root
	m_nodePool[TreeRoot].m_left = m_nodePool[pt_LC].m_right;

	if (m_nodePool[pt_LC].m_right != -1)
	{
		m_nodePool[m_nodePool[pt_LC].m_right].m_parent = TreeRoot;
	}

	//attach old left child of treenode to parent of treeNode
	m_nodePool[pt_LC].m_parent = m_nodePool[TreeRoot].m_parent;

	//if root had a parent
	if (m_nodePool[TreeRoot].m_parent != -1)
	{
		//change the child of that parent from tree root to tree roots leftchild
		if (IsLeftChild(TreeRoot))
			m_nodePool[m_nodePool[TreeRoot].m_parent].m_left = pt_LC;
		//change the child of that parent from tree root to tree roots rightchild
		else
			m_nodePool[m_nodePool[TreeRoot].m_parent].m_right = pt_LC;
	}
	else
	{
		m_rootNode = pt_LC;
	}

	//Assign treeRoot as the new Right child of the old treeRoot right Child;
	m_nodePool[TreeRoot].m_parent = pt_LC;
	m_nodePool[pt_LC].m_right = TreeRoot;
}



template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::Remove(T pKey)
{
	int getNode = Search(m_rootNode, pKey);
	if (getNode == -1)
		return false;

	//delete node and return it to the pool without causing tree violations
	FixAndDelete(SetForDeletion(getNode));

	return true;
}

template<typename T, typename F>
bool PreAllocated_SimpleDictionary<T, F>::Find(T pKey, F* pOutValue) const
{
	int node = Search(m_rootNode, pKey);
	if (node == -1)
	{
		pOutValue = nullptr;
		return false;
	}
	else
	{
		*pOutValue = m_nodePool[node].m_data;
		return true;
	}
}