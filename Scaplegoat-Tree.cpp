#include <iostream>
#include <math.h>

using namespace std;

// Funzione per restituire il valore di log32(n)
static float const log32(int n)
{
	double const log23 = 2.4663034623764317;
	return (float)(log23 * log(n));
}

//Classe che definisce un Nodo dello Scapegoat-tree
template <class T>
class Nodo
{
private:
	T *key;
	Nodo<T> *left, *right;
	Nodo<T> *parent;

public:
	Nodo(T key)
	{
		this->key = new T(key);
		left = right = parent = NULL;
	}

	T getKey()
	{
		return *key;
	}

	Nodo<T> *getLeft()
	{
		return left;
	}

	Nodo<T> *getRight()
	{
		return right;
	}

	Nodo<T> *getParent()
	{
		return parent;
	}

	void setKey(T key)
	{
		this->key = new T(key);
	}

	void setLeft(Nodo<T> *left)
	{
		this->left = left;
	}

	void setRight(Nodo<T> *right)
	{
		this->right = right;
	}

	void setParent(Nodo<T> *parent)
	{
		this->parent = parent;
	}
};

//Classe astratta Tree, per la definizione di un qualsiasi albero binario
template <class T>
class Tree
{
public:
	virtual void preorder(Nodo<T> *){};
	virtual void postorder(Nodo<T> *){};
	virtual void inorder(Nodo<T> *){};
	virtual int getSize(Nodo<T> *) = 0;
	virtual bool insertKey(T x) = 0;
	virtual Nodo<T> *deleteKey(T x) = 0;
	virtual Nodo<T> *searchKey(T x) = 0;
};

// Classe per rappresentare lo Scaplegoat-tree
template <class T>
class SGTree : public Tree<T>
{
private:
	Nodo<T> *root;
	int nodeNumber, MaxNode;

	// Visita preorder a partire da un determinato nodo
	void preorder(Nodo<T> *node)
	{
		if (node != NULL)
		{
			cout << node->getKey() << " ";
			preorder(node->getLeft());
			preorder(node->getRight());
		}
	}

	// Visita postorder a partire da un determinato nodo
	void postorder(Nodo<T> *node)
	{
		if (node != NULL)
		{
			preorder(node->getLeft());
			preorder(node->getRight());
			cout << node->getKey() << " ";
		}
	}

	// Visita inorder a partire da un determinato nodo
	void inorder(Nodo<T> *node)
	{
		if (node != NULL)
		{
			preorder(node->getLeft());
			cout << node->getKey() << " ";
			preorder(node->getRight());
		}
	}

	// Restituisce il confronto fra due valori
	double compare(T a, T b)
	{
		return ((a) - (double)(b));
	}

	//Funzione che dopo una visita inorderd salva i nodi all'interno di un array
	int storeInArray(Nodo<T> *node, Nodo<T> *array[], int i)
	{
		if (node == NULL)
			return i;
		i = storeInArray(node->getLeft(), array, i);
		array[i++] = node;
		return storeInArray(node->getRight(), array, i);
	}

public:
	//Costruttore
	SGTree()
	{
		root = NULL;
		nodeNumber = 0;
	}

	// visita preorder dell'albero
	void _preorder()
	{
		preorder(root);
	}

	// Per contare il numero di nodi all'interno dell'sottoalbero radicato nel nodo
	int getSize(Nodo<T> *node)
	{
		if (node == NULL)
			return 0;
		return 1 + getSize(node->getLeft()) + getSize(node->getRight());
	}

	//<-- METODI PER L'INSERIMENTO -->

	// Per inserire nuovi elementi all'interno dell'albero
	bool insertKey(T key)
	{
		Nodo<T> *newNode = new Nodo<T>(key);
		MaxNode++;

		// Per inserire il nodo nella struttura
		int depth = InsertAndDepth(newNode, key);

		// controllo se l'albero è diventato sbilanciato
		if (depth > log32(MaxNode))
		{
			// cerco lo scaplegoat-node
			Nodo<T> *p = newNode->getParent();
			while (3 * getSize(p) <= 2 * getSize(p->getParent()))
				p = p->getParent();

			// ricostruisco l'albero partendo dallo scaplegoat-node
			rebuildTree(p->getParent());
			cout << "L'inserimento della chiave " << key << " e' avvenuto correttamente ma ha causato una ricostruzione della struttura" << endl;
		}
		else
			cout << "L'inserimento della chiave " << key << " avvenuto correttamente " << endl;
		return depth >= 0;
	}

	// Inserisce il nodo e ne restituisce la profondità
	int InsertAndDepth(Nodo<T> *newNode, T key)
	{
		int depth = 0;
		Nodo<T> *tmp = root;
		Nodo<T> *parent = NULL;

		nodeNumber++;

		while (tmp != NULL)
		{
			parent = tmp;
			if (key <= tmp->getKey())
				tmp = tmp->getLeft();
			else
				tmp = tmp->getRight();
			depth++;
		}

		// l'albero non conteneva nodi
		if (parent == NULL)
		{
			root = newNode;
			return depth;
		}

		// inseriamo il nodo come figlio del parent controllando se va a destra o a sinistra
		if (key <= parent->getKey())
			parent->setLeft(newNode);
		else
			parent->setRight(newNode);
		newNode->setParent(parent);
		return depth;
	}

	//Funzione che ricostruisce l'albero a partire da un nodo
	void rebuildTree(Nodo<T> *node)
	{
		int numNode = getSize(node);
		Nodo<T> *parentNode = node->getParent();
		Nodo<T> **arrayNode = new Nodo<T> *[numNode];

		storeInArray(node, arrayNode, 0);

		if (parentNode == NULL)
		{
			root = buildBalancedFromArray(arrayNode, 0, numNode);
			root->setParent(NULL);
		}
		else if (parentNode->getRight() == node)
		{
			parentNode->setRight(buildBalancedFromArray(arrayNode, 0, numNode));
			Nodo<T> *tp = parentNode;
			parentNode = parentNode->getRight();
			parentNode->setParent(parentNode);
		}
		else
		{
			parentNode->setLeft(buildBalancedFromArray(arrayNode, 0, numNode));
			Nodo<T> *tp = parentNode;
			parentNode = parentNode->getLeft();
			parentNode->setParent(parentNode);
		}
	}

	// Funzione che ricostruisce l'albero con i nodi bilanciati
	Nodo<T> *buildBalancedFromArray(Nodo<T> **array, int i, int nodeNumber)
	{
		if (nodeNumber == 0)
			return NULL;

		int median = nodeNumber / 2;

		// a[m] diventa la root del
		// sottoalbero a[0],.....,a[m-1]
		array[i + median]->setLeft(buildBalancedFromArray(array, i, median));

		// Gli elementi a[0],...a[m-1] vengono salvati
		// nel sottoalbero sinistro
		if (array[i + median]->getLeft() != NULL)
		{
			Nodo<T> *a1 = array[i + median]->getLeft();
			a1->setParent(array[i + median]);
		}

		// Gli elementi  a[m+1],....a[n-1] vengono salvati
		// nel sottoalbero destro
		array[i + median]->setRight(buildBalancedFromArray(array, i + median + 1, nodeNumber - median - 1));
		if (array[i + median]->getRight() != NULL)
		{
			Nodo<T> *a1 = array[i + median]->getRight();
			a1->setParent(array[i + median]);
		}
		return array[i + median];
	}

	//<-- METODI PER LA RICERCA -->
	// Metodo che cerca una determinata chiave e restituisce il nodo se esiste oppure l'iltimo nodo appartenente al percorso
	Nodo<T> *searchKey(T key)
	{
		Nodo<T> *tmp = root;
		while (tmp != NULL && compare(key, tmp->getKey()) != 0)
		{
			if (compare(key, tmp->getKey()) < 0)
				tmp = tmp->getLeft();
			else
				tmp = tmp->getRight();
		}
		return tmp;
	}

	bool _searchKey(T key)
	{
		if (searchKey(key) != NULL)
			return 1;
		return 0;
	}

	//<-- METODI PER LA CANCELLAZIONE -->
	Nodo<T> *minimum(Nodo<T> *node)
	{
		if (node == NULL)
			return node;
		Nodo<T> *tmp = node;
		while (tmp->getLeft())
			tmp = tmp->getLeft();
		return tmp;
	}

	Nodo<T> *maximum(Nodo<T> *n)
	{
		if (n == NULL)
			return n;
		Nodo<T> *tmp = n;
		while (tmp->getRight())
			tmp = tmp->getRight();
		return tmp;
	}

	Nodo<T> *predecessor(Nodo<T> *n)
	{
		if (n == NULL)
			return n;
		if (n->getLeft())
			return maximum(n->getLeft());
		Nodo<T> *tmp = n->getParent();
		while (tmp != NULL && n == tmp->getLeft())
		{
			n = tmp;
			tmp = tmp->getParent();
		}
		return tmp;
	}

	Nodo<T> *successor(Nodo<T> *node)
	{
		if (node == NULL)
			return node;
		if (node->getRight())
			return minimum(node->getRight());
		Nodo<T> *tmp = node->getParent();
		while (tmp != NULL && node == tmp->getRight())
		{
			node = tmp;
			tmp = tmp->getParent();
		}
		return tmp;
	}

	Nodo<T> *deleteKey(T key)
	{
		Nodo<T> *node = searchKey(key);
		if (!node)
			return NULL;
		Nodo<T> *parent = node->getParent();
		// n non ha figlio sinistro
		if (node->getLeft() == NULL)
		{
			if (parent)
			{
				if (node == parent->getLeft())
					parent->setLeft(node->getRight());
				else
					parent->setRight(node->getRight());
			}
			else
				root = node->getRight();
			if (node->getRight())
				node->getRight()->setParent(parent);
			return node;
		}
		// n ha figlio sinistro ma non figlio destro
		if (node->getRight() == NULL)
		{
			if (parent)
			{
				if (node == parent->getLeft())
					parent->setLeft(node->getLeft());
				else
					parent->setRight(node->getLeft());
			}
			else
				root = node->getLeft();
			node->getLeft()->setParent(parent);
			return node;
		}
		// n ha entrambi i figli
		Nodo<T> *successive = successor(node);
		deleteKey(successive->getKey());
		node->setKey(successive->getKey());

		//	qui avviene in controllo
		nodeNumber--;
		if (nodeNumber < MaxNode / 2)
		{
			MaxNode = nodeNumber;
			Nodo<T> **array = new Nodo<T> *[nodeNumber];
			root = buildBalancedFromArray(array, 0, nodeNumber);
			root->setParent(NULL);
		}
		return successive;
	}
};

int main()
{
	SGTree<float> *sgt = new SGTree<float>();
	sgt->insertKey(12);
	sgt->insertKey(11);
	sgt->insertKey(13);
	sgt->insertKey(10);
	sgt->insertKey(14);
	sgt->insertKey(7);
	sgt->insertKey(6);
	sgt->insertKey(9);
	sgt->insertKey(5);
	sgt->insertKey(8);
	sgt->insertKey(8.5);
	cout<<"La visita preorder dello ScapeGoat tree risulta: ";
	sgt->_preorder();
	cout << endl;
	cout << "Ricerca della chiave 8.5: " << sgt->_searchKey(8.5) << endl;
	cout << "Ricerca della chiave 60: " << sgt->_searchKey(60) << endl;
	sgt->deleteKey(12);
	sgt->_preorder();
	return 0;
}
