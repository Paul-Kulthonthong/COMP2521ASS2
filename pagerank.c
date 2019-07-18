#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <math.h>

#include "pagerank.h"

#define MAX 100

void PageRankW(float d, float diffPR, float maxIterations){
    FileList returnedfilelist = getcollectionfilelist();

    while(returnedfilelist != NULL){
        printf("[%s]\n", returnedfilelist->filename);
        returnedfilelist = returnedfilelist->next;
    }


    return;
}


FileList getcollectionfilelist(){
    FILE * collectionfile;
    char filestemp[MAX];
    FileList returnlist = NULL;
    if ((collectionfile = fopen("collection.txt","r")) == NULL) {
        fprintf(stderr, "Can't open file collection.txt\n");
        return NULL;
	}
    while(fscanf(collectionfile, "%s", filestemp) != EOF){
        char * filename = getfiledir(filestemp);
        returnlist = addtofilelist(returnlist, filename);
    }
    fclose(collectionfile);
    return returnlist;
}

char * getfiledir(char *collectionfilename){
    char *filedir = malloc((strlen(collectionfilename)+5)*sizeof(char));
    strcat(filedir, collectionfilename);
    strcat(filedir, ".txt");
    strcat(filedir, "\0");
    return filedir;
}

FileList newFileNode(char *inputfile){
  FileList new = malloc(sizeof(struct FileListNode));
  assert(new != NULL);
  new->filename = malloc(strlen(inputfile)*sizeof(char) + 1);
  strcpy(new->filename, inputfile);
  new->next = NULL;
  return new;
}

FileList addtofilelist(FileList addonto, char *inputfile){
    FileList newfn = newFileNode(inputfile);
    if(addonto == NULL){
        addonto = newfn;
        return addonto;
    }
    else{
        FileList curr = addonto;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newfn;
    }
    return addonto;
}

/*void gatherwebpages(char *collectionFilename){
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

*/
