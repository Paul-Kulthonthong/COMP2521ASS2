#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pagerank.h"

#define MAX 100

int main(int argc, char *argv[])
{
	char fname[MAX];

  //Test if you input a collection of files txt
	if (argc < 2) {
		fprintf(stderr, "Usage: %s Collection of File Name\n", argv[0]);
		return EXIT_FAILURE;
	}
  strcpy(fname,argv[1]);
  char *collectionfile = malloc(strlen(fname) + 1);
  strcpy(collectionfile, fname);


  gatherwebpages(collectionfile);

}
