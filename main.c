#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pagerank.h"

#define MAX 100

int main(int argc, char *argv[])
{
	char fname[MAX];

  //Test if you input a collection of files txt
/*	if (argc < 2) {
		fprintf(stderr, "Usage: %s Collection of File Name\n", argv[0]);
		return EXIT_FAILURE;
	}*/

    PageRankW(0.85,  0.00001,  1000);

}