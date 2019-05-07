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


int main()
{
	CFTree root;
	root = InitCFTree();
	int len = 0;
	Data *ds = (Data*)malloc(sizeof(Data)* 10);
	read_pos(&ds, &len);
	print_pos(ds, len);
	for (int i = 0; i < len; ++ i)
		InsertCFTree(root, ds, i);
	DestroyCFTree(root);
	free(ds);
	return 0;
}