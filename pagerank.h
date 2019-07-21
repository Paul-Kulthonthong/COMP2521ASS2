#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>


#define strEQ(g, t) (strcmp ((g), (t)) == 0)
typedef unsigned char Num;

struct FileListNode {
    char *filename;
	struct FileListNode *next;
};
typedef struct FileListNode *FileList;

struct PRNode {
    char *filename;
    int outdegree;
    float pr;
	struct PRNode *next;
};
typedef struct PRNode *PRList;

typedef struct GraphRep {
	size_t nV;
	size_t maxV;
	char **vertex;
	Num **edges;
} GraphRep;
typedef struct GraphRep *Graph;



void PageRankW(float d, float diffPR, float maxIterations);


char * getfiledir(char *collectionfilename);
FileList getcollectionfilelist();
FileList newFileNode(char *inputfile);
FileList addtofilelist(FileList addonto, char *inputfile);
int getnumofiles(FileList filelist);


Graph getcollectiongraph(FileList file_list, int num_of_files);
int getGraphindegree(Graph g, int index);
int getGraphoutdegree(Graph g, int index);
float *initialisePR(Graph g);
float calculateWin(Graph g, int src, int dest);
float calculateWout(Graph g, int src, int dest);
float PRfirsthalf(Graph g, float d);
float PRsecondhalf(Graph g, float d, float *prev, int pr_index);
float calculatediff(Graph g, float *prev, float *curr);
float *calculatePR(Graph g, float d, float diffPR, float maxIterations);

PRList newPRNode(char *name, int out,float prinput);
PRList generatePRlist(Graph g, float *pr_indexes);
PRList insertPRnode(PRList addonto, Graph g, float *pr, int index);
void fprintPR(PRList print);



void printpagerankinfo(Graph urlgraph);

Graph newGraph (size_t maxV);
void dropGraph (Graph g);
bool addEdge (Graph g, char *src, char *dest);
bool isConnected (Graph g, char *src, char *dest);
size_t nVertices (Graph g);
void showGraph (Graph g, int mode);
ssize_t vertexID (char *str, char **names, size_t N);
size_t addVertex (char *, char **, size_t);
