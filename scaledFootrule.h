#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#define N 4

// state space tree node

struct SFRNode{
    // stores parent node of current node
    // helps in tracing path when answer is found
    struct SFRNode *parent;
    // contains cost for ancestors nodes
    // including current node
    int pathCost;
    // contains least promising cost
    int cost;
    // contain worker number
    int workerID;
    // contains Job ID
    int jobID;
    // Boolean array assigned will contains
    // info about available jobs
    bool assigned[N];

    struct SFRNode *next;
};
typedef struct SFRNode *SFRList;


struct QueueRep {
	SFRList head;  // ptr to first node
	SFRList tail;  // ptr to last node
};
typedef struct QueueRep *Queue;

struct CinnerNode{
    char *filename;
    int fileindex;
    int urlindex; //position in file
    int filewordcount;
    struct CinnerNode *next;
};
typedef struct CinnerNode *CinnerList;

struct CNode{
    char *url;
    CinnerList files;
	struct CNode *next;
};
typedef struct CNode *CList;




CList createCList(char **input);
CList newCNode(char *inputurl);
CList addtoClist(CList addonto, char *inputurl);
CList checkCnode(CList c, char *checkword);
CinnerList newCinnerNode(char *filename, int fileindex, int urlindex, int filewordcount);
void addCfile(CList cnode, char *filename, int fileindex, int urlindex, int filewordcount);

float **calculatefootrule(CList c);
float calculatesinglefootrules(CList c, int position, int size_of_c);

SFRList newNode(int x, int y, bool assigned[], SFRList parent, int n);
int findMinCost(int costMatrix[N][N]);
void printAssignments(SFRList min);
int calculateCost(int costMatrix[N][N], int x, int y, bool assigned[]);

Queue newQueue(); // create new empty queue
void QueueJoin(Queue Q, SFRList Tojoin); // add item on queue
SFRList QueueLeave(Queue Q); // remove item from queue
int QueueIsEmpty(Queue); // check for no items
