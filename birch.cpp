#include "birch.h"
#include <cmath>
#include <string.h>

static int node_id = 0;

void add_static(CFTree src, CFTree y)
{
	src->n += y->n, src->ls->x += y->ls->x, src->ls->y += y->ls->y, src->ss += y->ss;
}


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
	newNode->nid = node_id++;
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


double cluster_radius_inserted(CFTree node, Data *data)
{
	if (!node->leaf)
	{
		printf("not leaf\n");
		return 0;
	}
	double nss = node->ss + (data->x * data->x + data->y * data->y);
	Data *nls = (Data*)malloc(sizeof(Data));
	nls->x = node->ls->x + data->x, nls->y = node->ls->y + data->y;
	int nn = node->n + 1;
	return sqrt((nss * nn - (nls->x * nls->x + nls->y * nls->y)) / (nn * nn));
}


double CFDist(CFTree src, CFTree dst)
{
	double dx = src->ls->x / src->n - dst->ls->x / dst->n;
	double dy = src->ls->y / src->n - dst->ls->y / dst->n;
	return sqrt(dx * dx + dy * dy);
}


double DataDist(Data *x, Data *y)
{
	double dx = x->x - y->x, dy = x->y - y->y;
	return sqrt(dx * dx + dy * dy);
}


CFTree InsertLeafSplit(CFTree node, CFTree parent, Data *pds, int idx, int pos)
{
	// like split node
	double maxDist = -1e10;
	int sel_idx0 = -1;
	Data ct;
	ct.x = node->ls->x / node->n, ct.y = node->ls->y / node->n;
	for (int i = 0; i < L; ++i)
	{
		double dist = DataDist(pds + node->subCluster[i], &ct);
		if (dist > maxDist)
		{
			maxDist = dist;
			sel_idx0 = i;
		}
	}
	maxDist = -1e10;
	int sel_idx1 = -1;
	double dist0[L] = {};			// store, for next step
	for (int i = 0; i < L; ++i)
	{
		double dist = DataDist(pds + node->subCluster[i], &ct);
		if (dist > maxDist)
		{
			maxDist = dist;
			sel_idx1 = i;
		}
		dist0[i] = dist;
	}

	CFTree seed0 = _Malloc_CFNode();			// near to sel0
	CFTree seed1 = _Malloc_CFNode();
	for (int i = 0; i < L; ++i)
	{
		double dist1 = DataDist(pds + node->subCluster[i], pds + node->subCluster[sel_idx1]);
		if (dist1 < dist0[i])
			seed1 = InsertLeaf(seed1, pds, node->subCluster[i]);
		else
			seed0 = InsertLeaf(seed0, pds, node->subCluster[i]);
	}
	double d0 = DataDist(pds + node->subCluster[sel_idx0], pds + idx);
	double d1 = DataDist(pds + node->subCluster[sel_idx1], pds + idx);
	if (d0 > d1)
		seed1 = InsertLeaf(seed1, pds, idx);
	else
		seed0 = InsertLeaf(seed0, pds, idx);
	
	if (NULL != parent)
	{
		parent->child[pos] = seed0;
		InsertNonLeaf(seed1, parent);
		free(node);
		return seed0;
	}
	else
	{
		// if parent is root, should return new root 
		CFTree root = _Malloc_CFNode();
		InsertNonLeaf(seed0, root);
		InsertNonLeaf(seed1, root);
		free(node);
		Update_CFtree_Static(root);
		return root;
	}
}


CFTree InsertLeafSub(CFTree node, CFTree parent, Data *pds, int idx)
{
	if (parent == NULL)   // root
	{
		parent = _Malloc_CFNode();
		parent->leaf = false;
		// split current leaf into 2 leafs(node & subline), which are the children of new node
		CFTree subline = _Malloc_CFNode();
		subline = InsertLeaf(subline, pds, idx);
		parent = InsertNonLeaf(node, parent);
		parent = InsertNonLeaf(subline, parent);
		Update_CFtree_Static(parent);
		return parent;
	}
	else
	{
		CFTree nd = _Malloc_CFNode();
		nd = InsertLeaf(nd, pds, idx);
		parent = InsertNonLeaf(nd, parent);
		Insert_CFTree_Static(node, pds + idx);
		return node;
	}
}


CFTree InsertLeaf(CFTree node, Data *pds, int idx)
{
	node->subCluster[node->subLen++] = idx;
	Insert_CFTree_Static(node, pds + idx);
	return node;
}


inline CFTree InsertNonLeaf(CFTree node, CFTree parent)
{
	parent->child[parent->childLen++] = node;
	parent->leaf = false;
	return parent;
}

void Update_CFtree_Static(CFTree node)
{
	node->n = 0;
	node->ls->x = node->ls->y = node->ss = 0;
	for (int i = 0; i < node->childLen; ++i)
	{
		add_static(node, node->child[i]);
	}
}

void Insert_CFTree_Static(CFTree node, Data* data)
{
	node->n++;
	node->ls->x += data->x, node->ls->y += data->y;
	node->ss += data->x * data->x + data->y * data->y;
}


CFTree Split_CFTree(CFTree node, CFTree parent, Data *pds, int pos)
{
	// here, node already has B children, so has to be splited into two-parts
	double maxDist = -1e10;
	int sel_idx0 = -1;
	// find 2 seeds, then split by dist
	for (int i = 0; i <= B; ++i)
	{
		double dist = CFDist(node, node->child[i]);
		if (dist > maxDist)
		{
			maxDist = dist;
			sel_idx0 = i;
		}
	}
	maxDist = -1e10;
	int sel_idx1 = -1;
	double dist0[B + 1] = {};			// store, for next step
	for (int i = 0; i <= B; ++i)
	{
		double dist = CFDist(node->child[sel_idx0], node->child[i]);
		if (dist > maxDist)
		{
			maxDist = dist;
			sel_idx1 = i;
		}
		dist0[i] = dist;
	}

	// allocate 
	CFTree seed0 = _Malloc_CFNode();
	CFTree seed1 = _Malloc_CFNode();
	for (int i = 0; i <= B; ++i)
	{
		double dist1 = CFDist(node->child[sel_idx1], node->child[i]);
		if (dist1 < dist0[i])
			seed1 = InsertNonLeaf(node->child[i], seed1);
		else
			seed0 = InsertNonLeaf(node->child[i], seed0);
	}
	Update_CFtree_Static(seed0);
	Update_CFtree_Static(seed1);
	if (NULL != parent)
	{
		parent->child[pos] = seed0;
		InsertNonLeaf(seed1, parent);
		free(node);
		return seed0;
	}
	else
	{
		// if parent is root, should return new root 
		CFTree root = _Malloc_CFNode();
		InsertNonLeaf(seed0, root);
		InsertNonLeaf(seed1, root);
		free(node);
		Update_CFtree_Static(root);
		return root;
	}
}


CFTree _Insert_CFTree(CFTree node, CFTree parent, Data* pds, int idx, int pos)
{
	if (node->leaf)		// recurse finally stop here
	{
		if (node->subLen == L)		
		{
			node = InsertLeafSplit(node, parent, pds, idx, pos);
		}
		else
		{
			double r = cluster_radius_inserted(node, pds + idx);
			if (r > T)
				node = InsertLeafSub(node, parent, pds, idx);
			else
				node = InsertLeaf(node, pds, idx);
		}
		return node;
	}
	else    
	{
		const int CF_NOT_FOUND = -1;
		int sel_idx = CF_NOT_FOUND;
		double min_dist = 1e20;
		for (int i = 0; i < node->childLen; ++i)
		{
			if (node->child[i] == NULL)
				continue;
			if (node->child[i]->leaf)
			{
				double r = cluster_radius_inserted(node->child[i], pds + idx);
				if (r > T)
					continue;
			}
			double dist = cluster_dist(node->child[i], pds + idx);
			if (dist < min_dist)
			{
				min_dist = dist;
				sel_idx = i;
			}    
		}
		if (sel_idx == CF_NOT_FOUND)	// add one more child, check if split
		{
			node->child[node->childLen] = _Malloc_CFNode();
			node->child[node->childLen] = InsertLeaf(node->child[node->childLen], pds, idx);
			++ node->childLen;
		}
		else
		{
			node->child[sel_idx] = _Insert_CFTree(node->child[sel_idx], node, pds, idx, sel_idx);	
		}
		Insert_CFTree_Static(node, pds + idx);
		int Limit = B;
		// exceed children limit, need split
		// choose the farthest CFNode from node center as one seed, then choose farthest CFNode from seed as another
		// then insert 2 seeds into parent
		if (node->childLen > Limit)
		{
			node = Split_CFTree(node, parent, pds, pos);
		}
		return node;
	}
}


CFTree InsertCFTree(CFTree t, Data* pds, int idx)
{
	t = _Insert_CFTree(t, NULL, pds, idx, -1);
	return t;
}