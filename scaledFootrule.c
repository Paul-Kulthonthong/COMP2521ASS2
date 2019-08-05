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
#include <float.h>

#include "pagerank.h"
#include "searchPagerank.h"
#include "scaledFootrule.h"

#define N 4
#define MAX 100
#define MAX_LINE 1000

int main(int argc, char *argv[])
{

    CList c = createCList(argv);
    double **footrulearray = calculatefootrule(c);
    //printmatrix(footrulearray, Clistlength(c));
    findMinDist(footrulearray, Clistlength(c), c);
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

int Clistlength(CList c){
    int size = 0;
    CList rescur = c;
    while(rescur != NULL){
    /*    printf("Clist: [%s]\n", rescur->url);
        CinnerList rescurcur = rescur->files;
        while(rescurcur != NULL){
            printf("        %s, file#: %d, positioninfile: (%d out of %d)\n", rescurcur->filename, rescurcur->fileindex, rescurcur->urlindex, rescurcur->filewordcount);
            rescurcur = rescurcur->next;
        }*/
        rescur = rescur->next;
        size++;
    }
    return size;
}

//THIS IS FOR CALCULATING SCALEDFOOTRULE
//------------------------------------------------------------------------------

double **calculatefootrule(CList c){
    CList cur = c;
    int size_of_c = 0;
    while(cur!=NULL){
        size_of_c++;
        cur = cur->next;
    }
    //printf("Size of c: %d\n", size_of_c);

    double **returnArray;
    returnArray = calloc(size_of_c, sizeof(double *));
    int j = 0;
    while(j<size_of_c){
        returnArray[j] = calloc(size_of_c, sizeof(double));
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

double calculatesinglefootrules(CList c, int position, int size_of_c){
    double returnvalue = 0;
    CinnerList cin = c->files;
    while(cin!=NULL){
        returnvalue += fabs(  ((double)cin->urlindex)/((double)cin->filewordcount)  - (double)position/(double)size_of_c ) ;
        cin = cin->next;
    }
    //printf("%f \n", returnvalue);
    return returnvalue;
}

void printmatrix(double **array, int n){

    printf("This is the matrix: \n");
    int y = 0;
    while(y<n){
        int x = 0;
        while(x<n){
            printf("%f ", array[y][x]);
            x++;
        }
        printf("\n");
        y++;
    }

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
    if(Q->head->dist > Tojoin->dist){
        Tojoin->next = Q->head;
        Q->head = Tojoin;
        return;
    }
    else{
        SFRList curr = Q->head;
        while(curr->next != NULL){
            if(curr->next->dist > Tojoin->dist){
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

PrintList newPrintNode(int s, char *c){
    PrintList new = malloc(sizeof(struct PrintNode));
    assert(new != NULL);
    new->url = malloc(strlen(c)*sizeof(char)+1);
    strcpy(new->url, c);
    new->position = s;
    new->next = NULL;
    return new;
}

PrintList createPrintList(PrintList addonto, int s, char *c){
    PrintList newpn = newPrintNode(s, c);

    if(addonto == NULL){
        addonto = newpn;
        return addonto;
    }
    else if(newpn->position < addonto->position){
        newpn->next = addonto;
        addonto = newpn;
        return addonto;
    }
    else{
        PrintList curr = addonto;
        while(curr->next!=NULL){
            if(newpn->position < curr->next->position){
                break;
            }
            curr = curr->next;
        }
        curr->next = newpn;
        return addonto;
    }

}


// Function to allocate a new search tree node
// Here url with index x  withoin the c-list is assigned to a specific column
// within the matrix passed in.
// Using C++ reference code from
SFRList newNode(int x, int y, bool assigned[], SFRList parent, int n){
    SFRList node = malloc(sizeof(struct SFRNode));
    int j = 0;
    while(j < n){
        node->assigned[j] = assigned[j];
        j++;
    }
    node->assigned[y] = true;
    node->parent = parent;
    node->urlID = x;
    node->matrixindex = y;
    node->next = NULL;
    return node;
}

double calculateDist(double **costMatrix, int x, int y, bool assigned[], int n){
    double dist = 0;
    // to store unavailable jobs
    bool *available = malloc(n*sizeof(bool));
    int g = 0;
    while(g<n){
        available[g] = true;
        g++;
    }

    // start from next worker
    int i = x + 1;
    while(i<n){
        double min = DBL_MAX;
        int minIndex = -1;
        // do for each job
        int j = 0;
        while(j < n){
            // if job is unassigned
            if (!assigned[j] && available[j] && costMatrix[i][j] < min)
            {
                // store job number
                minIndex = j;

                // store dist
                min = costMatrix[i][j];

            }
            j++;
        }

        // add dist of next worker
        dist += min;
        // job column becomes unavailable
        available[minIndex] = false;
        i++;
    }
    return dist;
}


// Finds minimum dist using Branch and Bound.
double findMinDist(double **costMatrix, int n, CList c){
    // Create a priority queue to store live nodes of
    // search tree;
    Queue nq = newQueue();

    // initailize heap to dummy node with dist 0
    //bool assigned[n] = {false};
    bool *assigned = malloc(n*sizeof(bool));
    int g = 0;
    while(g<n){
        assigned[g] = false;
        g++;
    }

    SFRList dummy = newNode(-1, -1, assigned, NULL, n);
    dummy->total_dist = dummy->dist = 0;
    dummy->urlID = -1;
    QueueJoin(nq, dummy);

    // Finds a live node with least dist,
    // add its childrens to list of live nodes and
    // finally deletes it from the list.
    while (!QueueIsEmpty(nq)){
      // Find a live node with least estimated dist found node is deleted from the queue
      SFRList min = QueueLeave(nq);

      // i stores next worker
      int i = min->urlID + 1;

      // if all workers are assigned a job
      if (i == n)
      {
          printf("%.6f\n", min->dist);
          PrintList p = NULL;
          p = generatePrintList(p, min, c);

          PrintList pcurr = p;
          while(pcurr != NULL){
              printf("%s\n", pcurr->url);
              pcurr = pcurr->next;
          }
          return min->dist;
      }

      // do for each column
      int j = 0;
      while(j < n){
        // If unassigned
        if (!min->assigned[j]){
          SFRList child = newNode(i, j, min->assigned, min, n);
          // dist for ancestors nodes including current node
          child->total_dist = min->total_dist + costMatrix[i][j];
          //printf("out: %lf\n",calculateDist(costMatrix, i, j, child->assigned, n));
          // calculate its lower bound
          child->dist = child->total_dist + calculateDist(costMatrix, i, j, child->assigned, n);
          // Add child to list of live nodes;
          QueueJoin(nq, child);
        }
        j++;
      }
    }
    return 0;
}



// Generate the print list in order of their position
PrintList generatePrintList(PrintList p, SFRList min, CList c){
    if(min->parent==NULL){
        return p ;
    }
    p = generatePrintList(p, min->parent, c);
    CList curr = c;
    int i = 0;
    while(i<min->urlID){
        curr = curr->next;
        i++;
    }
    p = createPrintList(p, min->urlID, curr->url);
    //printf("Assign Worker id: %c(%d)  %s to Position: %d\n", min->urlID+'A', min->urlID, curr->url, min->matrixindex + 1);
    return p;
}
