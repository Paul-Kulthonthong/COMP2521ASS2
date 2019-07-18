#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <math.h>

#include "pagerank.h"

#define MAX 100

void PageRankW(d, diffPR, maxIterations){
    getcollectionfilelist();




    return;
}


void getcollectionfilelist(){
    FILE * collectionfile;
    if ((collectionfile = fopen("collection.txt","r")) == NULL) {
        fprintf(stderr, "Can't open file %s\n", collectionFilename);
        return;
	}
    while(fscanf(f, "%s", filestemp) != EOF){
        char * filename = getfiledir(dir, filestemp);
        printf("Gonna go to this file: [%s]\n", filename);
    }
    return;

}


void gatherwebpages(char *collectionFilename){
    FILE *f;
    FILE *ff;
    char filestemp[MAX];
    char infileword[MAX];
    char *temp;
    int num_in_collection_file = 0;

    if ((f = fopen(collectionFilename,"r")) == NULL) {
        fprintf(stderr, "Can't open file %s\n", collectionFilename);
        return;
	}

    char *collectionfullcopy = malloc(strlen(collectionFilename)+1);
    strcpy(collectionfullcopy, collectionFilename);
    temp = dirname(collectionfullcopy);
    char* dir = malloc(strlen(temp)*sizeof(char));
    strcpy(dir, temp);

    while(fscanf(f, "%s", filestemp) != EOF){
        char * filename = getfiledir(dir, filestemp);
        printf("Gonna go to this file: [%s]\n", filename);


        if ((ff = fopen(filename,"r")) == NULL) {
            fprintf(stderr, "Can't open file %s\n", filename);
            return;
      	}

        rewind(ff);

        printf("Section 1:\n");
        fscanf(ff, "%s", infileword);
        fscanf(ff, "%s", infileword);
        while(fscanf(ff, "%s", infileword) != EOF){
            if((strcmp(infileword, "Section-1") == 0) || (strcmp(infileword, "#end") == 0)){
                //NOTHING
            }
            else{
                printf("%s\n", infileword);
            }
            if(strcmp(infileword, "Section-1") == 0){
                break;
            }
        }
        printf("\n");

        printf("Section 2:\n");
        fscanf(ff, "%s", infileword);
        fscanf(ff, "%s", infileword);
        while(fscanf(ff, "%s", infileword) != EOF){
            if((strcmp(infileword, "Section-2") == 0) || (strcmp(infileword, "#end") == 0)){
                //NOTHING
            }
            else{
                printf("%s\n", infileword);
            }
            if(strcmp(infileword, "Section-2") == 0){
                break;
            }
        }
        printf("\n");

        num_in_collection_file++;
    }
}

char * getfiledir(char * dir, char *collectionfilename){
    char *filedir = malloc((strlen(collectionfilename)+strlen(dir)+6)*sizeof(char));
    strcpy(filedir, dir);
    strcat(filedir, "/");
    strcat(filedir, collectionfilename);
    strcat(filedir, ".txt");
    strcat(filedir, "\0");
    return filedir;
}
