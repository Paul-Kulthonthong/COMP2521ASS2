#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <math.h>
#include <err.h>
#include <stdbool.h>
#include <sysexits.h>

#include "pagerank.h"

#define MAX 100

void PageRankW(float d, float diffPR, float maxIterations){
    FileList filelistofurl = getcollectionfilelist();
    int num_of_files_in_collection = getnumofiles(filelistofurl);

    printf("{%d}\n", num_of_files_in_collection);

    Graph urlgraph = getcollectiongraph(filelistofurl, num_of_files_in_collection);
    showGraph(urlgraph, 1);


    //printf("%d is part of vertex %s %s\n", urlgraph->edges[0][1], urlgraph->vertex[0], urlgraph->vertex[1]);

//    List_Urls_PageRanks= calculatePageRank(g, d, diffPR, maxIterations);

//    Ordered_List_Urls_PageRanks= order (List_Urls_PageRanks)
//    Output Ordered_List_Urls_PageRanksto “pagerankList.txt”



    return;
}

//THIS IS FOR CREATING THE LINK LIST OF FILES TO BE OPENED
//------------------------------------------------------------------------------
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

int getnumofiles(FileList filelist){
    int num_of_files_in_collection = 0;
    FileList curr = filelist;
    while(curr != NULL){
        curr = curr->next;
        num_of_files_in_collection++;
    }
    return num_of_files_in_collection;
}
//------------------------------------------------------------------------------


//THIS IS FOR CREATING THE GRAPH OF FILES OPENED
//------------------------------------------------------------------------------
Graph getcollectiongraph(FileList file_list, int num_of_files){

    Graph newgraph = newGraph((size_t)num_of_files);
    FILE *f;
    char filestemp[MAX];
    char infileword[MAX];

    FileList curr = file_list;
    while(curr != NULL){
        if ((f = fopen(curr->filename,"r")) == NULL) {
            fprintf(stderr, "Can't open file %s\n", curr->filename);
            return NULL;
    	}

        rewind(f);
        int redflag = 2;
        while(fscanf(f, "%s", infileword) != EOF && redflag>0){
            if((strcmp(infileword, "#end") == 0) || (strcmp(infileword, "#start") == 0)){
                //NOTHING
            }
            else if(strcmp(infileword, "Section-1") == 0){
                redflag--;
            }
            else{
                if(strcmp(curr->filename, getfiledir(infileword)) != 0 ){
                    addEdge(newgraph, curr->filename, getfiledir(infileword));
                }
            }
        }
        fclose(f);
        curr = curr->next;
    }
    return newgraph;
}

Graph newGraph (size_t maxV){
	Graph new = malloc (sizeof *new);
	if (new == NULL)
		err (EX_OSERR, "couldn't allocate Graph");
	*new = (GraphRep){.nV = 0, .maxV = maxV};

	new->vertex = calloc (maxV, sizeof (char *));
	if (new->vertex == NULL)
		err (EX_OSERR, "calloc");
	new->edges = calloc (maxV, sizeof (Num *));
	if (new->edges == NULL)
		err (EX_OSERR, "calloc");

	for (size_t i = 0; i < maxV; i++) {
		new->edges[i] = calloc (maxV, sizeof (Num));
		if (new->edges[i] == NULL)
			err (EX_OSERR, "calloc");

		for (size_t j = 0; j < maxV; j++)
			new->edges[i][j] = 0;
	}
	return new;
}



// addEdge(Graph,Src,Dest)
// - add an edge from Src to Dest
// - returns 1 if edge successfully added
// - returns 0 if unable to add edge
//   (usually because nV exceeds maxV)
bool addEdge (Graph g, char *src, char *dest){
	assert (g != NULL);

	ssize_t v = vertexID (src, g->vertex, g->nV);
	if (v < 0) {
		if (g->nV >= g->maxV)
			return false;
		v = (ssize_t) addVertex (src, g->vertex, g->nV);
		g->nV++;
	}

	ssize_t w = vertexID (dest, g->vertex, g->nV);
	if (w < 0) {
		if (g->nV >= g->maxV)
			return false;
		w = (ssize_t) addVertex (dest, g->vertex, g->nV);
		g->nV++;
	}
	g->edges[v][w] = 1;

	return true;
}

// isConnected(Graph,Src,Dest)
// - check whether there is an edge from Src->Dest
bool isConnected (Graph g, char *src, char *dest){
	assert (g != NULL);
	ssize_t v = vertexID (src, g->vertex, g->nV);
	ssize_t w = vertexID (dest, g->vertex, g->nV);
	if (v < 0 || w < 0)
		return 0;
	return g->edges[v][w] == 1;
}

// nVertices(Graph)
// - return # vertices currently in Graph
size_t nVertices (Graph g){
	assert (g != NULL);
	return g->nV;
}

// showGraph(Graph)
// - display Graph
void showGraph (Graph g, int mode){
	assert (g != NULL);
	if (g->nV == 0) {
		puts ("Graph is empty");
		return;
	}

	printf ("Graph has %zu vertices:\n", g->nV);
	if (mode == 1) {
		for (size_t i = 0; i < g->nV; i++) {
			for (size_t j = 0; j < g->nV; j++)
				printf ("%d ", g->edges[i][j]);
			putchar ('\n');
		}
	} else {
		for (size_t i = 0; i < g->nV; i++) {
			printf ("Vertex: %s\n", g->vertex[i]);
			printf ("connects to\n");
			for (size_t j = 0; j < g->nV; j++) {
				if (g->edges[i][j])
					printf ("   %s\n", g->vertex[j]);
			}
		}
	}
}

// Helper functions

// vertexID(Str,Names,N)
// - searches for Str in array of Names[N]
// - returns index of Str if found, -1 if not
ssize_t vertexID (char *str, char **names, size_t N){
	for (size_t i = 0; i < N; i++)
		if (strEQ (str, names[i]))
			return (ssize_t) i;
	return -1;
}

// addVertex(Str,Names,N)
// - add Str at end of Names
size_t addVertex (char *str, char **names, size_t N){
	names[N] = strdup (str);
	return N;
}



















// dropGraph(Graph)
// - clean up memory associated with Graph
void dropGraph (Graph g){
	if (g == NULL)
		return;

	for (size_t i = 0; i < g->nV; i++)
		free (g->vertex[i]);
	for (size_t i = 0; i < g->maxV; i++)
		free (g->edges[i]);

	free (g->vertex);
	free (g->edges);
	free (g);
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
