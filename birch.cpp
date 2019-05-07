#include "birch.h"
#include <cmath>


static CFTree _Malloc_CFNode()
{
	CFTree newNode = (CFTree)malloc(sizeof(CFNode));
	if (newNode == NULL)
	{
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < B + 1; ++i)
	{
		newNode->child[i] = NULL;
	}
	newNode->ls = (Data*)malloc(sizeof(Data));
	newNode->n = 0, newNode->ls->x = 0, newNode->ls->y = 0, newNode->ss = 0;
	newNode->leaf = true;
	newNode->next = newNode->prev = NULL;
	newNode->subLen = newNode->childLen = 0;
	return newNode;
}


CFTree InitCFTree()
{
	CFTree tree = _Malloc_CFNode();
	return tree;
}


CFTree DestroyCFTree(CFTree t)
{
	for (int i = 0; i < B + 1; ++i)
	{
		if (t->child[i] != NULL)
			DestroyCFTree(t->child[i]);
	}
	free(t->ls);
	free(t);
	t = NULL;
	return t;
}


double cluster_dist(CFTree node, Data *data)
{
	double dx = node->ls->x / node->n - data->x, dy = node->ls->y / node->n - data->y;
	return sqrt(dx * dx + dy * dy);
}


CFTree InsertLeafSub(CFTree node, CFTree parent, Data *pds, int idx)
{
	if (parent == NULL)
	{
		parent = _Malloc_CFNode();
		parent->leaf = false;
		// split current leaf into 2 leafs(node & subline), which are the children of new node
		CFTree subline = _Malloc_CFNode();
		subline = InsertLeaf(subline, parent, pds, idx);
		parent = InsertNonLeaf(node, parent);
		parent = InsertNonLeaf(subline, parent);
		Update_CFtree_Static(parent);
		return parent;
	}
}


CFTree InsertLeaf(CFTree node, CFTree parent, Data *pds, int idx)
{
	node->subCluster[node->subLen++] = idx;
	Insert_CFTree_Static(node, pds + idx);
	return node;
}


CFTree InsertNonLeaf(CFTree node, CFTree parent)
{
	parent->child[parent->childLen++] = node;
	return parent;
}

void Update_CFtree_Static(CFTree node)
{
	node->n = 0;
	node->ls->x = node->ls->y = node->ss = 0;
	for (int i = 0; i < node->childLen; ++i)
	{
		node->n += node->child[i]->n;
		node->ls->x += node->child[i]->ls->x;
		node->ls->y += node->child[i]->ls->y;
		node->ss += node->child[i]->ss;
	}
}

void Insert_CFTree_Static(CFTree node, Data* data)
{
	node->n++;
	node->ls->x += data->x, node->ls->y += data->y;
	node->ss += data->x * data->x + data->y * data->y;
}


CFTree Split_CFTree(CFTree node, CFTree parent, Data *pds)
{
	if (parent == NULL)
	{
		
		// move last element to new leaf
		node->subLen = L;
		
		
	}
	else
	{

	}
	return NULL;
}


CFTree _Insert_CFTree(CFTree node, CFTree parent, Data* pds, int idx)
{
	if (node->leaf)		// recurse finally stop here
	{
		if (node->subLen == L)		// exceed max sub-cluster number
			node = InsertLeafSub(node, parent, pds, idx);
		else
		{
			node = InsertLeaf(node, parent, pds, idx);
		}	
	}
	else    //	non leaf
	{
		int sel_idx = 0;
		double min_dist = 1e20;
		for (int i = 0; i < B; ++i)
		{
			if (node->child[i] == NULL)
				continue;
			double dist = cluster_dist(node, pds + idx);
			if (dist < min_dist)
			{
				min_dist = dist;
				sel_idx = i;
			}
		}
		node->child[sel_idx] = _Insert_CFTree(node->child[sel_idx], node, pds, idx);
	}
	int Limit = L;
	return node;
}


CFTree InsertCFTree(CFTree t, Data* pds, int idx)
{
	t = _Insert_CFTree(t, NULL, pds, idx);
	return t;
}