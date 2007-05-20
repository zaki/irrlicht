// Copyright 2006-2007 by Kat'Oun
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_MAP_H_INCLUDED__
#define __IRR_MAP_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace core
{

//! map template for associative arrays using a red-black tree
template <class KeyType, class ValueType>
class map
{
	//! red/black tree for map
	template <class KeyTypeRB, class ValueTypeRB>
	class RBTree
	{
	public:

		RBTree(const KeyTypeRB& k, const ValueTypeRB& v)
			: mLeftChild(0), mRightChild(0), mParent(0), mKey(k),
				mValue(v), mIsRed(true) {}

		virtual ~RBTree(){}

		void setLeftChild(RBTree* p)	{ mLeftChild=p; if (p) p->setParent(this); }
		void setRightChild(RBTree* p)	{ mRightChild=p;if (p) p->setParent(this); }
		void setParent(RBTree* p)		{ mParent=p; }

		void setValue(const ValueTypeRB& v)	{ mValue = v; }

		void setRed()					{ mIsRed = true; }
		void setBlack()					{ mIsRed = false; }

		RBTree* getLeftChild() const	{ return mLeftChild; }
		RBTree* getRightChild() const	{ return mRightChild; }
		RBTree* getParent() const		{ return mParent; }

		ValueTypeRB getValue() const	
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mValue; 
		}

		KeyTypeRB getKey() const		
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mKey; 
		}


		bool isRoot() const				
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mParent==0;
		}

		bool isLeftChild() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mParent!=0 && mParent->getLeftChild()==this;
		}

		bool isRightChild() const		
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mParent!=0 && mParent->getRightChild()==this;
		}

		bool isLeaf() const				
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mLeftChild==0 && mRightChild==0;
		}

		unsigned int getLevel() const	
		{ 
			if (isRoot()) 
				return 1;
			else 
				return getParent()->getLevel() + 1;
		}


		bool isRed()	const	
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return  mIsRed;
		}

		bool isBlack()	const
		{ 
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return !mIsRed;
		}

	private:
		RBTree();

		RBTree*		mLeftChild;
		RBTree*		mRightChild;

		RBTree*		mParent;

		KeyTypeRB	mKey;
		ValueTypeRB	mValue;

		bool mIsRed;
	};

	public:

	typedef RBTree<KeyType,ValueType> Node;

	//! Normal Iterator
	class Iterator
	{
	public:

		Iterator() : mRoot(0), mCur(0) {}

		// Constructor(Node*)
		Iterator(Node* root):mRoot(root)
		{
			reset();
		}

		// Copy constructor
		Iterator(const Iterator& src) : mRoot(src.mRoot), mCur(src.mCur){}

		void reset(bool atLowest=true)
		{
			if (atLowest)
				mCur = getMin(mRoot);
			else
				mCur = getMax(mRoot);
		}

		bool atEnd() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mCur==0;
		}

		Node* getNode()
		{
			return mCur;
		}

		Iterator& operator=(const Iterator& src)
		{
			mRoot = src.mRoot;
			mCur = src.mCur;
			return (*this);
		}

		void operator++(int)
		{
			inc();
		}

		void operator--(int)
		{
			dec();
		}


		Node* operator -> ()
		{
			return getNode();
		}

		Node& operator* ()
		{
			if (atEnd())
				throw "Iterator at end";

			return *mCur;
		}

	private:

		Node* getMin(Node* n)
		{
			while(n && n->getLeftChild())
				n = n->getLeftChild();
			return n;
		}

		Node* getMax(Node* n)
		{
			while(n && n->getRightChild())
				n = n->getRightChild();
			return n;
		}

		void inc()
		{
			// Already at end?
			if (mCur==0)
				return;

			if (mCur->getRightChild())
			{
				// If current node has a right child, the next higher node is the
				// node with lowest key beneath the right child.
				mCur = getMin(mCur->getRightChild());
			}
			else if (mCur->isLeftChild())
			{
				// No right child? Well if current node is a left child then
				// the next higher node is the parent
				mCur = mCur->getParent();
			}
			else
			{
				// Current node neither is left child nor has a right child.
				// Ie it is either right child or root
				// The next higher node is the parent of the first non-right
				// child (ie either a left child or the root) up in the
				// hierarchy. Root's parent is 0.
				while(mCur->isRightChild())
					mCur = mCur->getParent();
				mCur = mCur->getParent();
			}
		}

		void dec()
		{
			// Already at end?
			if (mCur==0)
				return;

			if (mCur->getLeftChild())
			{
				// If current node has a left child, the next lower node is the
				// node with highest key beneath the left child.
				mCur = getMax(mCur->getLeftChild());
			}
			else if (mCur->isRightChild())
			{
				// No left child? Well if current node is a right child then
				// the next lower node is the parent
				mCur = mCur->getParent();
			}
			else
			{
				// Current node neither is right child nor has a left child.
				// Ie it is either left child or root
				// The next higher node is the parent of the first non-left
				// child (ie either a right child or the root) up in the
				// hierarchy. Root's parent is 0.

				while(mCur->isLeftChild())
					mCur = mCur->getParent();
				mCur = mCur->getParent();
			}
		}

		Node* mRoot;
		Node* mCur;
	}; // Iterator



	//! Parent First Iterator.
	//! Traverses the tree from top to bottom. Typical usage is
	//! when storing the tree structure, because when reading it
	//! later (and inserting elements) the tree structure will
	//! be the same.
	class ParentFirstIterator
	{
	public:


	ParentFirstIterator():mRoot(0),mCur(0)
	{
	}


	explicit ParentFirstIterator(Node* root):mRoot(root),mCur(0)
	{
		reset();
	}

	void reset()
	{
		mCur = mRoot;
	}


	bool atEnd() const
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return mCur==0;
	}

	Node* getNode()
	{
		return mCur;
	}


	ParentFirstIterator& operator=(const ParentFirstIterator& src)
	{
		mRoot = src.mRoot;
		mCur = src.mCur;
		return (*this);
	}


	void operator++(int)
	{
		inc();
	}


	Node* operator -> ()
	{
		return getNode();
	}

	Node& operator* ()
	{
		if (atEnd())
			throw "ParentFirstIterator at end";
		return *getNode();
	}

	private:

	void inc()
	{
		// Already at end?
		if (mCur==0)
			return;

		// First we try down to the left
		if (mCur->getLeftChild())
		{
			mCur = mCur->getLeftChild();
		}
		else if (mCur->getRightChild())
		{
			// No left child? The we go down to the right.
			mCur = mCur->getRightChild();
		}
		else
		{
			// No children? Move up in the hierarcy until
			// we either reach 0 (and are finished) or
			// find a right uncle.
			while (mCur!=0)
			{
				// But if parent is left child and has a right "uncle" the parent
				// has already been processed but the uncle hasn't. Move to
				// the uncle.
				if (mCur->isLeftChild() && mCur->getParent()->getRightChild())
				{
					mCur = mCur->getParent()->getRightChild();
					return;
				}
				mCur = mCur->getParent();
			}
		}
	}

	Node* mRoot;
	Node* mCur;

	}; // ParentFirstIterator


	//! Parent Last Iterator
	//! Traverse the tree from bottom to top.
	//! Typical usage is when deleting all elements in the tree
	//! because you must delete the children before you delete
	//! their parent.
	class ParentLastIterator
	{
	public:

		ParentLastIterator():mRoot(0),mCur(0){}

		explicit ParentLastIterator(Node* root) : mRoot(root), mCur(0)
		{
			reset();
		}

		void reset()
		{
			mCur = getMin(mRoot);
		}

		bool atEnd() const
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return mCur==0;
		}

		Node* getNode()
		{
			return mCur;
		}

		ParentLastIterator& operator=(const ParentLastIterator& src)
		{
			mRoot = src.mRoot;
			mCur = src.mCur;
			return (*this);
		}

		void operator++(int)
		{
			inc();
		}

		Node* operator -> ()
		{
			return getNode();
		}

		Node& operator* ()
		{
			if (atEnd())
				throw "ParentLastIterator at end";
			return *getNode();
		}
	private:

		Node* getMin(Node* n)
		{
			while(n!=0 && (n->getLeftChild()!=0 || n->getRightChild()!=0))
			{
				if (n->getLeftChild())
					n = n->getLeftChild();
				else
					n = n->getRightChild();
			}
			return n;
		}

		void inc()
		{
			// Already at end?
			if (mCur==0)
				return;

			// Note: Starting point is the node as far down to the left as possible.

			// If current node has an uncle to the right, go to the
			// node as far down to the left from the uncle as possible
			// else just go up a level to the parent.
			if (mCur->isLeftChild() && mCur->getParent()->getRightChild())
			{
				mCur = getMin(mCur->getParent()->getRightChild());
			}
			else
				mCur = mCur->getParent();
		}


		Node* mRoot;
		Node* mCur;
	}; // ParentLastIterator


	// AccessClass is a temporary class used with the [] operator.
	// It makes it possible to have different behavior in situations like:
	// myTree["Foo"] = 32;
	//   If "Foo" already exists, just update its value else insert a new
	//   element.
	// int i = myTree["Foo"]
	// If "Foo" exists return its value, else throw an exception.
	class AccessClass
	{
		// Let map be the only one who can instantiate this class.
		friend class map<KeyType, ValueType>;

	public:

		// Assignment operator. Handles the myTree["Foo"] = 32; situation
		void operator=(const ValueType& value)
		{
			// Just use the Set method, it handles already exist/not exist situation
			mTree.set(mKey,value);
		}

		// ValueType operator
		operator ValueType()
		{
			Node* node = mTree.find(mKey);

			// Not found
			if (node==0)
				throw "Item not found";

			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return node->getValue();
		}

	private:

		AccessClass(map& tree, const KeyType& key):mTree(tree),mKey(key){}

		AccessClass();

		map& mTree;
		const KeyType& mKey;
	}; // AccessClass


	// Constructor.
	map() : mRoot(0),mSize(0) {}

	// Destructor
	~map()
	{
		clear();
	}

	//------------------------------
	// Public Commands
	//------------------------------

	//! Inserts a new node into the tree
	//! \param keyNew: the index for this value
	//! \param v: the value to insert
	//! \return Returns true if successful,
	//! false if it fails (already exists)
	bool insert(const KeyType& keyNew, const ValueType& v)
	{
		// First insert node the "usual" way (no fancy balance logic yet)
		Node* newNode = new Node(keyNew,v);
		if (!insert(newNode))
		{
			delete newNode;
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}

		// Then attend a balancing party
		while (!newNode->isRoot() && (newNode->getParent()->isRed()))
		{
			if (newNode->getParent()->isLeftChild())
			{
				// If newNode is a left child, get its right 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->getRightChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if ( newNode->isRightChild())
					{
						// and newNode is to the right
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateLeft(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateRight(newNode->getParent()->getParent());
				}
			}
			else
			{
				// If newNode is a right child, get its left 'uncle'
				Node* newNodesUncle = newNode->getParent()->getParent()->getLeftChild();
				if ( newNodesUncle!=0 && newNodesUncle->isRed())
				{
					// case 1 - change the colours
					newNode->getParent()->setBlack();
					newNodesUncle->setBlack();
					newNode->getParent()->getParent()->setRed();
					// Move newNode up the tree
					newNode = newNode->getParent()->getParent();
				}
				else
				{
					// newNodesUncle is a black node
					if (newNode->isLeftChild())
					{
						// and newNode is to the left
						// case 2 - move newNode up and rotate
						newNode = newNode->getParent();
						rotateRight(newNode);
					}
					// case 3
					newNode->getParent()->setBlack();
					newNode->getParent()->getParent()->setRed();
					rotateLeft(newNode->getParent()->getParent());
				}

			}
		}
		// Color the root black
		mRoot->setBlack();
		return true;
	}

	//! Replaces the value if the key already exists,
	//! otherwise inserts a new element.
	//! \param k: the index for this value
	//! \param v: the new value of
	void set(const KeyType& k, const ValueType& v)
	{
		Node* p = find(k);
		if (p)
			p->setValue(v);
		else
			insert(k,v);
	}

	//! Removes a node from the tree and returns it.
	//! The returned node must be deleted by the user
	//! \param k: the key to remove
	//! \return: A pointer to the node, or 0 if not found
	Node* delink(const KeyType& k)
	{
		Node* p = find(k);
		if (p == 0)
			return 0;

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while(p->getRightChild())
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft(p);
		}
		// p now has no right child but might have a left child
		Node* left = p->getLeftChild();

		// Let p's parent point to p's child instead of point to p
		if (p->isLeftChild())
			p->getParent()->setLeftChild(left);

		else if (p->isRightChild())
			p->getParent()->setRightChild(left);

		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			setRoot(left);
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it, so return it.

		mSize--;
		return p;
	}

	//! Removes a node from the tree and deletes it.
	//! \return True if the node was found and deleted
	bool remove(const KeyType& k)
	{
		Node* p = find(k);
		if (p == 0)
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}

		// Rotate p down to the left until it has no right child, will get there
		// sooner or later.
		while(p->getRightChild())
		{
			// "Pull up my right child and let it knock me down to the left"
			rotateLeft(p);
		}
		// p now has no right child but might have a left child
		Node* left = p->getLeftChild();

		// Let p's parent point to p's child instead of point to p
		if (p->isLeftChild())
			p->getParent()->setLeftChild(left);

		else if (p->isRightChild())
			p->getParent()->setRightChild(left);

		else
		{
			// p has no parent => p is the root.
			// Let the left child be the new root.
			setRoot(left);
		}

		// p is now gone from the tree in the sense that
		// no one is pointing at it. Let's get rid of it.
		delete p;

		mSize--;
		return true;
	}

	//! Clear the entire tree
	void clear()
	{
		ParentLastIterator i(getParentLastIterator());

		while(!i.atEnd())
		{
			Node* p = i.getNode();
			i++; // Increment it before it is deleted
				// else iterator will get quite confused.
			delete p;
		}
		mRoot = 0;
		mSize= 0;
	}

	//! Is the tree empty?
	//! \return Returns true if empty, false if not
	bool isEmpty() const
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return mRoot == 0;
	}

	//! Search for a node with the specified key.
	//! \param keyToFind: The key to find
	//! \return Returns 0 if node couldn't be found.
	Node* find(const KeyType& keyToFind) const
	{
		Node* pNode = mRoot;

		while(pNode!=0)
		{
			KeyType key(pNode->getKey());

			if (keyToFind == key)
				return pNode;
			else if (keyToFind < key)
				pNode = pNode->getLeftChild();
			else //keyToFind > key
				pNode = pNode->getRightChild();
		}

		return 0;
	}

	//! Gets the root element.
	//! \return Returns a pointer to the root node, or
	//! 0 if the tree is empty.
	Node* getRoot() const
	{
		return mRoot;
	}

	//! Returns the number of nodes in the tree.
	u32 size() const
	{
		return mSize;
	}

	//------------------------------
	// Public Iterators
	//------------------------------

	//! Returns an iterator
	Iterator getIterator()
	{
		Iterator it(getRoot());
		return it;
	}
	//! Returns a ParentFirstIterator.
	//! Traverses the tree from top to bottom. Typical usage is
	//! when storing the tree structure, because when reading it
	//! later (and inserting elements) the tree structure will
	//! be the same.
	ParentFirstIterator getParentFirstIterator()
	{
		ParentFirstIterator it(getRoot());
		return it;
	}
	//! Returns a ParentLastIterator to traverse the tree from
	//! bottom to top.
	//! Typical usage is when deleting all elements in the tree
	//! because you must delete the children before you delete
	//! their parent.
	ParentLastIterator getParentLastIterator()
	{
		ParentLastIterator it(getRoot());
		return it;
	}

	//------------------------------
	// Public Operators
	//------------------------------

	//! operator [] for accesss to elements
	//! for example myMap["key"]
	AccessClass operator[](const KeyType& k)
	{
		return AccessClass(*this, k);
	}
	private:

	//------------------------------
	// Disabled methods
	//------------------------------
	//! Copy constructor and assignment operator deliberately
	//! defined but not implemented. The tree should never be
	//! copied, pass along references to it instead (or use auto_ptr to it).
	explicit map(const map& src);
	map& operator = (const map& src);

	void setRoot(Node* newRoot)
	{
		mRoot = newRoot;
		if (mRoot!=0)
			mRoot->setParent(0);
	}

	//! Insert a node into the tree without using any fancy balancing logic.
	//! Returns false if that key already exist in the tree.
	bool insert(Node* newNode)
	{
		bool result=true; // Assume success

		if (mRoot==0)
		{
			setRoot(newNode);
			mSize = 1;
		}
		else
		{
			Node* pNode = mRoot;
			KeyType keyNew = newNode->getKey();
			while (pNode)
			{
				KeyType key(pNode->getKey());

				if (keyNew == key)
				{
					result = false;
					pNode = 0;
				}
				else if (keyNew < key)
				{
					if (pNode->getLeftChild() == 0)
					{
						pNode->setLeftChild(newNode);
						pNode = 0;
					}
					else
						pNode = pNode->getLeftChild();
				}
				else // keyNew > key
				{
					if (pNode->getRightChild()==0)
					{
						pNode->setRightChild(newNode);
						pNode = 0;
					}
					else
					{
						pNode = pNode->getRightChild();
					}
				}
			}

			if (result)
				mSize++;
		}

		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return result;
	}

	//! Rotate left.
	//! Pull up node's right child and let it knock node down to the left
	void rotateLeft(Node* p)
	{
		Node* right = p->getRightChild();

		p->setRightChild(right->getLeftChild());

		if (p->isLeftChild())
			p->getParent()->setLeftChild(right);
		else if (p->isRightChild())
			p->getParent()->setRightChild(right);
		else
			setRoot(right);

		right->setLeftChild(p);
	}

	//! Rotate right.
	//! Pull up node's left child and let it knock node down to the right
	void rotateRight(Node* p)
	{

		Node* left = p->getLeftChild();

		p->setLeftChild(left->getRightChild());

		if (p->isLeftChild())
			p->getParent()->setLeftChild(left);
		else if (p->isRightChild())
			p->getParent()->setRightChild(left);
		else
			setRoot(left);

		left->setRightChild(p);
	}

	//------------------------------
	// Private Members
	//------------------------------
	Node* mRoot; // The top node. 0 if empty.
	u32 mSize; // Number of nodes in the tree

};

} // end namespace core
} // end namespace irr

#endif // __IRR_MAP_H_INCLUDED__

