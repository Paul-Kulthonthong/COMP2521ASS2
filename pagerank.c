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
#include <sys/types.h>

#include "pagerank.h"

#define MAX 100

void PageRankW(float d, float diffPR, float maxIterations){

    FileList filelistofurl = getcollectionfilelist();

    int num_of_files_in_collection = getnumofiles(filelistofurl);
    Graph urlgraph = getcollectiongraph(filelistofurl, num_of_files_in_collection);

    printpagerankinfo(urlgraph);

    float *curr = calculatePR(urlgraph, d, diffPR, maxIterations);

    int k = 0;
    while(k<urlgraph->nV){
        printf("%s, %d, %.7f\n", urlgraph->vertex[k], getGraphoutdegree(urlgraph, k),curr[k]);
        k++;
    }


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
        char * filename = malloc((strlen(filestemp)+1)*sizeof(char));
        strcpy(filename, filestemp);
        strcat(filename, "\0");
        //char * filename = getfiledir(filestemp);
        returnlist = addtofilelist(returnlist, filename);
    }
    fclose(collectionfile);
    return returnlist;
}

char * getfiledir(char *collectionfilename){
    char *filedir = malloc((strlen(collectionfilename)+5)*sizeof(char));
    strcpy(filedir, collectionfilename);
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
    char infileword[MAX];


    FileList curr = file_list;
    while(curr != NULL){
        if ((f = fopen(getfiledir(curr->filename),"r")) == NULL) {
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
                if(strcmp(curr->filename, infileword) != 0 ){
                    addEdge(newgraph, curr->filename, infileword);
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

bool isConnected (Graph g, char *src, char *dest){
	assert (g != NULL);
	ssize_t v = vertexID (src, g->vertex, g->nV);
	ssize_t w = vertexID (dest, g->vertex, g->nV);
	if (v < 0 || w < 0)
		return 0;
	return g->edges[v][w] == 1;
}

size_t nVertices (Graph g){
	assert (g != NULL);
	return g->nV;
}

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

ssize_t vertexID (char *str, char **names, size_t N){
	for (size_t i = 0; i < N; i++)
		if (strEQ (str, names[i]))
			return (ssize_t) i;
	return -1;
}

size_t addVertex (char *str, char **names, size_t N){
    names[N] = malloc(strlen(str)+1);
    assert(names[N]!=NULL);
    strcpy(names[N], str);
    return N;
}

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

//THIS IS FOR CALCULATIONS
//------------------------------------------------------------------------------

int getGraphindegree(Graph g, int index){
    int i = 0;
    int indegcounter = 0;
    while(i<g->nV){
        if(g->edges[i][index] == 1){
            indegcounter++;
        }
        i++;
    }
    return indegcounter;
}

int getGraphoutdegree(Graph g, int index){
    int i = 0;
    int outdegcounter = 0;
    while(i<g->nV){
        if(g->edges[index][i] == 1){
            outdegcounter++;
        }
        i++;
    }
    return outdegcounter;
}

float *initialisePR(Graph g){
    float *initial =  malloc((g->nV)*sizeof(float));
    int i = 0;
    while(i<g->nV){
        initial[i] = 1/((float)(g->nV));
        i++;
    }
    return initial;
}

float calculateWin(Graph g, int src, int dest){
    float Win = 0;
    float numerator = getGraphindegree(g, dest);
    float denominator = 0;
    int j = 0;
    while(j < g->nV){
        if(isConnected(g, g->vertex[src], g->vertex[j])){
            denominator += getGraphindegree(g, j);
        }
        j++;
    }
    return Win = numerator/denominator;
}

float calculateWout(Graph g, int src, int dest){
    float Wout = 0;
    float numerator = getGraphoutdegree(g, dest);
    if(numerator == 0){
        numerator = 0.5;
    }
    float denominator = 0;
    int j = 0;
    while(j < g->nV){
        if(isConnected(g, g->vertex[src], g->vertex[j])){
            if(getGraphoutdegree(g, j) != 0){
                denominator += getGraphoutdegree(g, j);
            }
            else{
                denominator += 0.5;
            }

        }
        j++;
    }
    return Wout = numerator/denominator;
}

float PRfirsthalf(Graph g, float d){
    float firsthalfPR;
    return firsthalfPR = (1-d)/((float)(g->nV));
}

float PRsecondhalf(Graph g, float d, float *prev, int pr_index){
    int j = 0;
    float sum_of_second_half = 0;
    while(j < g->nV){
        if(isConnected(g, g->vertex[j], g->vertex[pr_index])){
            sum_of_second_half +=  prev[j]*calculateWin(g, j, pr_index)*calculateWout(g, j, pr_index);
        }
        j++;
    }
    return sum_of_second_half*d;
}

float calculatediff(Graph g, float *prev, float *curr){
    float diff  = 0;
    int i = 0;
    while(i<g->nV){
        //printf("i[%d] diff = %f - %f\n", i, curr[i], prev[i]);
        if((curr[i]-prev[i])>=0){
            diff = diff + (curr[i]-prev[i]);
        }
        else if((curr[i]-prev[i])<0){
            diff = diff - (curr[i]-prev[i]);
        }
        i++;
    }
    return diff;
}

float *calculatePR(Graph g, float d, float diffPR, float maxIterations){
    float *prev =  initialisePR(g);
    float *curr =  initialisePR(g);
    float iteration = 0;
    float diff = diffPR;
    while(iteration < maxIterations && diff >= diffPR){
        int pi = 0;
        while(pi<g->nV){
            curr[pi] = PRfirsthalf(g, d) + PRsecondhalf(g, d, prev, pi);
            pi++;
        }
        diff = calculatediff(g, prev, curr);
        int q = 0;
        while(q<g->nV){
            prev[q] = curr[q];
            q++;
        }
        iteration++;
    }
    return curr;
}

//THIS IS FOR PRINTING
//------------------------------------------------------------------------------

void printpagerankinfo(Graph urlgraph){

    showGraph(urlgraph, 1);

    printf("urlsNo {%zu}\n", urlgraph->nV);

    int i = 0;
    while(i<urlgraph->nV){
        printf("nodeid: %d , indegree: %d , outdegree: %d\n", i, getGraphindegree(urlgraph, i), getGraphoutdegree(urlgraph, i));
        i++;
    }

    int m = 0;
    while(m<urlgraph->nV){
        int n = 0;
        while(n<urlgraph->nV){
            if(isConnected(urlgraph, urlgraph->vertex[m], urlgraph->vertex[n])){
                printf("Edge[%d][%d]: ", m, n);
                printf("Win: %.7f , ", calculateWin(urlgraph, m, n));
                printf("Wout: %.7f\n", calculateWout(urlgraph, m, n));
            }
            n++;
        }
        m++;
    }

    return;

}








// addEdge(Graph,Src,Dest)
// - add an edge from Src to Dest
// - returns 1 if edge successfully added
// - returns 0 if unable to add edge
//   (usually because nV exceeds maxV)

// isConnected(Graph,Src,Dest)
// - check whether there is an edge from Src->Dest

// nVertices(Graph)
// - return # vertices currently in Graph

// showGraph(Graph)
// - display Graph

// Helper functions

// vertexID(Str,Names,N)
// - searches for Str in array of Names[N]
// - returns index of Str if found, -1 if not

// addVertex(Str,Names,N)
// - add Str at end of Names

// dropGraph(Graph)
// - clean up memory associated with Graph

