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
#include <limits.h>

#include "pagerank.h"
#include "searchPagerank.h"
#include "scaledFootrule.h"

#define N 4
#define MAX 100
#define MAX_LINE 1000

int main(int argc, char *argv[])
{
    // x-cordinate represents a Worker
    // y-cordinate represents a Job



    int costMatrix[N][N] =
    {
        {9, 2, 7, 8},
        {6, 4, 3, 7},
        {5, 8, 1, 8},
        {7, 6, 9, 4}


    };
    printf("Optimal Cost is \n");
    findMinCost(costMatrix);


    /*
        {82, 83, 69, 92},
        {77, 37, 49, 92},
        {11, 69,  5, 86},
        { 8,  9, 98, 23}

        {2500, 4000, 3500},
        {4000, 6000, 3500},
        {2000, 4000, 2500}

        {90, 75, 75, 80},
        {30, 85, 55, 65},
        {125, 95, 90, 105},
        {45, 110, 95, 115}
    */
    CList c = createCList(argv);
    int sizeofc = 0;
    CList rescur = c;
    while(rescur != NULL){
        printf("Clist: [%s]\n", rescur->url);
        CinnerList rescurcur = rescur->files;

        while(rescurcur != NULL){
            printf("        %s, file#: %d, positioninfile: (%d out of %d)\n", rescurcur->filename, rescurcur->fileindex, rescurcur->urlindex, rescurcur->filewordcount);
            rescurcur = rescurcur->next;
        }
        rescur = rescur->next;
        sizeofc++;
    }

    float **footrulearray = calculatefootrule(c);

    printf("This is the matrix: \n");
    int y = 0;
    while(y<sizeofc){
        int x = 0;
        while(x<sizeofc){
            printf("%f ", footrulearray[y][x]);
            x++;
        }
        printf("\n");
        y++;
    }


    return 0;
}

//THIS IS FOR CREATING C LIST
//------------------------------------------------------------------------------
CList createCList(char **input){
    CList returnlist = NULL;
    int i = 1;
    FILE * clistfile;
    char temp[MAX];

    int fileindex = 0;
    while(input[i] != NULL){
        if ((clistfile = fopen(input[i],"r")) == NULL) {
            fprintf(stderr, "Can't open file %s\n", input[i]);

    	}
        else{
            int num_of_url = 0;
            while(fscanf(clistfile, "%s", temp) != EOF){
                num_of_url++;
            }

            rewind(clistfile);
            int urlindex = 1;
            while(fscanf(clistfile, "%s", temp) != EOF){
                CList tempnode = checkCnode(returnlist, temp);
                if(tempnode == NULL){
                    returnlist = addtoClist(returnlist, temp);
                    addCfile(checkCnode(returnlist, temp), input[i], fileindex, urlindex, num_of_url);

                }
                else{
                    addCfile(tempnode, input[i], fileindex, urlindex, num_of_url);
                }
                urlindex++;
            }
            fclose(clistfile);
            fileindex++;
        }
        i++;
    }
    return returnlist;
}

CList newCNode(char *inputurl){
    CList new = malloc(sizeof(struct CNode));
    assert(new != NULL);
    new->url = malloc(strlen(inputurl)*sizeof(char) + 1);
    strcpy(new->url, inputurl);
    new->files = NULL;
    new->next = NULL;
    return new;
}

CList addtoClist(CList addonto, char *inputurl){
    CList newcn = newCNode(inputurl);
    if(addonto == NULL){
        addonto = newcn;
        return addonto;
    }
    else{
        CList curr = addonto;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newcn;
    }
    return addonto;
}

CList checkCnode(CList c, char *checkword){
    CList returnvalue = NULL;
    CList rescur = c;
    while(rescur != NULL){
        if(strcmp(rescur->url, checkword) == 0){
            returnvalue = rescur;
            break;
        }
        rescur = rescur->next;
    }
    return returnvalue;
}

CinnerList newCinnerNode(char *filename, int fileindex, int urlindex, int filewordcount){
    CinnerList new = malloc(sizeof(struct CinnerNode));
    assert(new != NULL);
    new->filename = malloc(strlen(filename)*sizeof(char) + 1);
    strcpy(new->filename, filename);
    new->fileindex = fileindex;
    new->urlindex = urlindex;
    new->filewordcount = filewordcount;
    new->next = NULL;
    return new;
}

void addCfile(CList cnode, char *filename, int fileindex, int urlindex, int filewordcount){
    CinnerList newcin = newCinnerNode(filename, fileindex, urlindex, filewordcount);
    CinnerList rescur = cnode->files;
    if (cnode->files == NULL){
        cnode->files = newcin;
        return;
    }
    else{
        while(rescur->next != NULL){
            rescur = rescur->next;
        }
        rescur->next = newcin;
    }
}

//THIS IS FOR CALCULATING SCALEDFOOTRULE
//------------------------------------------------------------------------------

float **calculatefootrule(CList c){
    CList cur = c;
    int size_of_c = 0;
    while(cur!=NULL){
        size_of_c++;
        cur = cur->next;
    }
    //printf("Size of c: %d\n", size_of_c);

    float **returnArray;
    returnArray = calloc(size_of_c, sizeof(float *));
    int j = 0;
    while(j<size_of_c){
        returnArray[j] = calloc(size_of_c, sizeof(float));
        j++;
    }

    cur = c;
    int y = 0;
    while(cur!=NULL){
        //printf("C analysis: [%s]  ", cur->url);
        int i = 1;
        int x = 0;
        while(i<=size_of_c){
        //    printf("%f ", calculatesinglefootrules(cur, i, size_of_c));
            returnArray[y][x] = calculatesinglefootrules(cur, i, size_of_c);
            i++;
            x++;
        }
    //    printf("\n");
        y++;
        cur = cur->next;
    }
    return returnArray;
}

float calculatesinglefootrules(CList c, int position, int size_of_c){
    float returnvalue = 0;
    CinnerList cin = c->files;
    while(cin!=NULL){
        returnvalue += fabs(  ((float)cin->urlindex)/((float)cin->filewordcount)  - (float)position/(float)size_of_c ) ;
        cin = cin->next;
    }
    //printf("%f \n", returnvalue);
    return returnvalue;
}

//THIS IS FOR PRIORITY QUEUE
//------------------------------------------------------------------------------
Queue newQueue(){
	Queue q;
	q = malloc(sizeof(struct QueueRep));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

void QueueJoin(Queue Q, SFRList Tojoin){
	assert(Q != NULL);
    if(Q->head == NULL){
        Q->head = Tojoin;
        return;
    }
    if(Q->head->cost > Tojoin->cost){
        Tojoin->next = Q->head;
        Q->head = Tojoin;
        return;
    }
    else{
        SFRList curr = Q->head;
        while(curr->next != NULL){
            if(curr->next->cost > Tojoin->cost){
                break;
            }
            curr = curr->next;
        }
        Tojoin->next = curr->next;
        curr->next = Tojoin;
        return;
    }
}

SFRList QueueLeave(Queue Q){
	assert(Q != NULL);
	assert(Q->head != NULL);
	SFRList it = Q->head;
	SFRList old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL){
        Q->tail = NULL;
    }
	return it;
}

int QueueIsEmpty(Queue Q){
	return (Q->head == NULL);
}

//------------------------------------------------------------------------------

// Function to allocate a new search tree node
// Here Person x is assigned to job y
SFRList newNode(int x, int y, bool assigned[], SFRList parent, int n){
    SFRList node = malloc(sizeof(struct SFRNode));
    int j = 0;
    while(j < n){
        node->assigned[j] = assigned[j];
        j++;
    }
    node->assigned[y] = true;
    node->parent = parent;
    node->workerID = x;
    node->jobID = y;
    node->next = NULL;
    return node;
}

// Function to calculate the least promising cost
// of node after worker x is assigned to job y.
int calculateCost(int costMatrix[N][N], int x, int y, bool assigned[]){
    int cost = 0;
    // to store unavailable jobs
    bool available[N] = {true};

    // start from next worker
    for (int i = x + 1; i < N; i++)
    {
        int min = INT_MAX, minIndex = -1;

        // do for each job
        for (int j = 0; j < N; j++)
        {
            // if job is unassigned
            if (!assigned[j] && available[j] && costMatrix[i][j] < min)
            {
                // store job number
                minIndex = j;

                // store cost
                min = costMatrix[i][j];
            }
        }

        // add cost of next worker
        cost += min;

        // job becomes unavailable
        available[minIndex] = false;
    }

    return cost;
}




// print Assignments
void printAssignments(SFRList min){
    if(min->parent==NULL){
        return;
    }

    printAssignments(min->parent);
    printf("Assign Worker id: %c(%d) to Job %d\n", min->workerID+'A', min->workerID, min->jobID);

}

// Finds minimum cost using Branch and Bound.
int findMinCost(int costMatrix[N][N]){
    // Create a priority queue to store live nodes of
    // search tree;
    Queue nq = newQueue();

    // initailize heap to dummy node with cost 0
    bool assigned[N] = {false};
    SFRList root = newNode(-1, -1, assigned, NULL, N);
    root->pathCost = root->cost = 0;
    root->workerID = -1;

    // Add dummy node to list of live nodes;
    QueueJoin(nq, root);

    // Finds a live node with least cost,
    // add its childrens to list of live nodes and
    // finally deletes it from the list.
    while (!QueueIsEmpty(nq)){
      // Find a live node with least estimated cost
      SFRList min = QueueLeave(nq);

      // The found node is deleted from the list of
      // live nodes

      // i stores next worker
      int i = min->workerID + 1;

      // if all workers are assigned a job
      if (i == N)
      {
          printAssignments(min);
          return min->cost;
      }

      // do for each job
      for (int j = 0; j < N; j++){
        // If unassigned
        if (!min->assigned[j])
        {
          // create a new tree node
          SFRList child = newNode(i, j, min->assigned, min, N);

          // cost for ancestors nodes including current node
          child->pathCost = min->pathCost + costMatrix[i][j];

          // calculate its lower bound
          child->cost = child->pathCost +
            calculateCost(costMatrix, i, j, child->assigned);

          // Add child to list of live nodes;
          QueueJoin(nq, child);
        }
      }
    }
    return 0;
}

/*
float findMinDist(float **costMatrix, int n){
    // Create a priority queue to store live nodes of
    // search tree;
    Queue nq = newQueue();

    // initailize heap to dummy node with cost 0
    bool assigned[n] = {false};
    SFRList root = newNode(-1, -1, assigned, NULL, n);
    root->pathCost = root->cost = 0;
    root->workerID = -1;

    // Add dummy node to list of live nodes;
    QueueJoin(nq, root);

    // Finds a live node with least cost,
    // add its childrens to list of live nodes and
    // finally deletes it from the list.
    while (!QueueIsEmpty(nq)){
      // Find a live node with least estimated cost
      SFRList min = QueueLeave(nq);

      // The found node is deleted from the list of
      // live nodes

      // i stores next worker
      int i = min->workerID + 1;

      // if all workers are assigned a job
      if (i == n)
      {
          printAssignments(min);
          return min->cost;
      }

      // do for each job
      for (int j = 0; j < n; j++){
        // If unassigned
        if (!min->assigned[j])
        {
          // create a new tree node
          SFRList child = newNode(i, j, min->assigned, min, n);

          // cost for ancestors nodes including current node
          child->pathCost = min->pathCost + costMatrix[i][j];

          // calculate its lower bound
          child->cost = child->pathCost +
            calculateCost(costMatrix, i, j, child->assigned);

          // Add child to list of live nodes;
          QueueJoin(nq, child);
        }
      }
    }
    return 0;
}
*/
