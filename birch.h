#pragma once

#include <cstdlib>
#include <cstdio>
#pragma pack(4)

// Cluster Feature Tree

#define B	(2)		// max child-node number in non-leaf
#define L	(3)		// max sub-cluster number in leaf
#define T	(0.1)


typedef struct CFNode *CFTree;

struct Data
{
	double x, y;
};

struct CFNode
{
	int n;				// number of elements in all tree which root is this node   S0
	double ss;			// S2
	int subCluster[L];			// sub cluster
	CFNode *child[B + 1];
	CFNode *next, *prev;
	Data *ls;			// center point		S1
	unsigned char subLen;		// number of sub-cluster
	unsigned char childLen;		// number of child
	bool leaf;
};


CFTree InitCFTree();
CFTree DestroyCFTree(CFTree t);
CFTree InsertCFTree(CFTree, Data*, int);


void Insert_CFTree_Static(CFTree node, Data* data);
void Update_CFtree_Static(CFTree node);
CFTree InsertLeaf(CFTree node, Data *pds, int idx); 
CFTree InsertNonLeaf(CFTree node, CFTree parent);