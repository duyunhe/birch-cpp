#include <cstdio>
#include "birch.h"



void read_pos(Data **ppds, int *len)
{
	int cnt = 0;
	FILE *fp = fopen("position.txt", "r");
	double tx, ty;
	while (fscanf(fp, "%lf%lf", &tx, &ty) != EOF)
	{
		(*ppds + cnt)->x = tx;
		(*ppds + cnt)->y = ty;
		++cnt;
	}
	*len = cnt;
	fclose(fp);
}


void print_pos(Data *pds, int len)
{
	for (int i = 0; i < len; ++i)
		printf("%.1f %.1f\n", (pds + i)->x, (pds + i)->y);
}


void print_tree(FILE *fp, CFTree tr)
{
	if (tr->leaf)
	{
		fprintf(fp, "(%d", tr->nid);
		fprintf(fp, "[");
		for (int i = 0; i < tr->subLen; ++i)
		{
			if (i)
				fprintf(fp, ",");
			fprintf(fp, "%d", tr->subCluster[i]);
		}
		fprintf(fp, "])");
		return;
	}
	fprintf(fp, "(%d", tr->nid);
	for (int i = 0; i < tr->childLen; ++i)
	{
		print_tree(fp, tr->child[i]);
	}
	fprintf(fp, ")");
}


int main()
{
	CFTree root;
	root = InitCFTree();
	int len = 0;
	Data *ds = (Data*)malloc(sizeof(Data)* 20);
	read_pos(&ds, &len);
	print_pos(ds, len);
	FILE *fp = fopen("E://job//basic//birch_tree//tree.txt", "w");
	for (int i = 0; i < len; ++i)
	{
		root = InsertCFTree(root, ds, i);
		if (i >= 0)
		{
			print_tree(fp, root);
			fprintf(fp, "\n");
		}

	}
	fclose(fp);
	DestroyCFTree(root);
	free(ds);
	return 0;
}