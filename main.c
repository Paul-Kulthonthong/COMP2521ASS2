#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pagerank.h"

#define MAX 100

int main(int argc, char *argv[])
{
	//char fname[MAX];

  //Test if you input a collection of files txt
	if (argc < 4) {
		fprintf(stderr, "Usage: %s damping-factor difference-in-PageRank-sum maximum-iterations\n", argv[0]);
		return EXIT_FAILURE;
	}

    PageRankW(atof(argv[1]),  atof(argv[2]),  atof(argv[3]));

}
