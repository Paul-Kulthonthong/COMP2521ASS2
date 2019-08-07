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
#include "searchPagerank.h"

#define MAX 100
#define MAX_LINE 1000

int main(int argc, char *argv[])
{
    if (argc < 2) {
		fprintf(stderr, "Usage: %s searchwords\n", argv[0]);
		return EXIT_FAILURE;
	}
    SearchList searchwords = createWordList(argv);
    searchpagerank(searchwords);
}


//From a input of words, search up the words within an invertedIndex.txt and then
// print out the top 30 most relevant url based on the searched num_of_words
// most relevant would be url containing the most matched words then if
// same number of matched words, relevancy is determined by pagerank of url from pagerankList.txt
void searchpagerank(SearchList searchwords){
    ResultList sorted = getassociatedurl(searchwords);
    ResultList rescur = sorted;
    int i = 0;
    while(rescur != NULL && i < 30){
        printf("%s\n", rescur->url);
        rescur = rescur->next;
        i++;
    }
    return;
}

//THIS IS FOR CREATING THE SEARCH WORD LINKED LIST
//------------------------------------------------------------------------------
//Create a linked list of the search words provided by the stdin via the keyboard command line
SearchList createWordList(char **input){
    SearchList returnlist = NULL;
    int i = 1;
    while(input[i] != NULL){
        returnlist = addtosearchlist(returnlist, input[i]);
        i++;
    }
    return returnlist;
}

//Create the new search word node within the search word linked list
SearchList newSearchNode(char *inputword){
  SearchList new = malloc(sizeof(struct SearchWordNode));
  assert(new != NULL);
  new->word = malloc(strlen(inputword)*sizeof(char) + 1);
  strcpy(new->word, inputword);
  new->next = NULL;
  return new;
}

//Add the search word node onto the end of the linked list of search
SearchList addtosearchlist(SearchList addonto, char *inputword){
    SearchList newsn = newSearchNode(inputword);
    if(addonto == NULL){
        addonto = newsn;
        return addonto;
    }
    else{
        SearchList curr = addonto;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newsn;
    }
    return addonto;
}
//------------------------------------------------------------------------------

//THIS IS FOR CREATING THE RESULT LINKED LIST
//------------------------------------------------------------------------------
//Return a link list of urls in order of the number of matched words and pagerank mentioned above
ResultList getassociatedurl(SearchList searchwords){
    FILE * invertedindexfile;
    char temp[MAX];
    char line[MAX_LINE];
    char temp_temp[MAX];
    ResultList returnlist = NULL;

    if ((invertedindexfile = fopen("invertedIndex.txt","r")) == NULL) {
        fprintf(stderr, "Can't open file invertedIndex.txt\n");
        return NULL;
	}

    SearchList curr = searchwords;
    while(curr!= NULL){
        rewind(invertedindexfile);
        int exist_flag = 0;
        while(fscanf(invertedindexfile, "%s", temp) != EOF){
            if(strcmp(curr->word, temp) == 0){
                exist_flag = 1;
                fgets(line, MAX_LINE, invertedindexfile);
                break;
            }
        }
        if(exist_flag == 1){
            //printf("URLS in %s\n", temp);
            int pos = 0;
            int pos_cur=0;
            while(sscanf(line+pos_cur, "%s%n", temp_temp, &pos) != EOF){
                //printf("%s\n", temp_temp);
                pos_cur += pos;
                ResultList hold;
                if(resultnodeexist(returnlist, temp_temp) != NULL){
                    hold = resultnodeexist(returnlist, temp_temp);
                    hold->num_of_words++;
                }
                else{
                    returnlist = addtoresultlist(returnlist, temp_temp);
                    hold = resultnodeexist(returnlist, temp_temp);
                    hold->num_of_words++;
                }
            }
        }
        curr = curr->next;

    }
    fclose(invertedindexfile);

    returnlist = sortsearchresults(returnlist);
    return returnlist;
}

//Creates the new node required of the linked list of relevant urls
ResultList newResultNode(char *inputword){
  ResultList new = malloc(sizeof(struct ResultNode));
  assert(new != NULL);
  new->url = malloc(strlen(inputword)*sizeof(char) + 1);
  strcpy(new->url, inputword);
  new->num_of_words = 0;
  new->pagerank = getpagerank(inputword);
  new->next = NULL;
  return new;
}

//Add result nodes to the end of the linked list of relevant urls
ResultList addtoresultlist(ResultList addonto, char *inputword){
    ResultList newsn = newResultNode(inputword);
    ResultList curr = addonto;
    if(addonto == NULL){
        addonto = newsn;
        return addonto;
    }
    else{
        while(curr != NULL){
            if(curr->next == NULL){
                curr->next = newsn;
                break;
            }
            curr = curr->next;
        }
    }
    return addonto;
}

//Check if the link list of relevant urls already contain a specific url as we do not want duplicates but instead
//increment the num_or_words of the already added url
ResultList resultnodeexist(ResultList addonto, char *inputurl){
    ResultList curr = addonto;
    while(curr!=NULL){
        if(strcmp(curr->url, inputurl) == 0){
            break;
        }
        curr = curr->next;
    }
    return curr;
}

//Get the associated pagerank of a particular url
float getpagerank(char *inputurl){
    FILE * pagerank;
    float returnvalue = 0;
    char temp[MAX];
    if ((pagerank = fopen("pagerankList.txt","r")) == NULL) {
        fprintf(stderr, "Can't open file pagerankList.txt\n");
        return 0;
	}


    while(fscanf(pagerank, "%s" , temp) != EOF){
        if(strstr(temp, inputurl) != NULL){
            fscanf(pagerank, "%s", temp);
            fscanf(pagerank, "%s", temp);
            //printf("float: %.7f\n", atof(temp));
            returnvalue = atof(temp);
        }
    }

    fclose(pagerank);
    return returnvalue;
}

//Create a duplicate node of the finalised url result list
ResultList duplicate(ResultList original){
  ResultList new = malloc(sizeof(struct ResultNode));
  assert(new != NULL);
  new->url = malloc(strlen(original->url)*sizeof(char) + 1);
  strcpy(new->url, original->url);
  new->num_of_words = original->num_of_words;
  new->pagerank = original->pagerank;
  new->next = NULL;
  return new;
}

//Sort out the finalised url result list
ResultList sortsearchresults(ResultList tosort){
    ResultList sorted = NULL;
    ResultList curr = tosort;

    while(curr!= NULL){
        ResultList dup = duplicate(curr);
        ResultList sortedcurr = sorted;
        if(sorted == NULL){
            sorted = dup;
        }
        else if(dup->num_of_words >= sorted->num_of_words){
            if(dup->num_of_words > sorted->num_of_words){
                dup->next = sorted;
                sorted = dup;
            }
            else{
                if(dup->pagerank > sorted->pagerank){
                    dup->next = sorted;
                    sorted = dup;
                }
                else{
                    dup->next = sorted->next;
                    sorted->next = dup;
                }
            }
        }
        else{
            while(sortedcurr != NULL){
                if(sortedcurr->next == NULL){
                    sortedcurr->next = dup;
                    break;
                }
                else if(dup->num_of_words >= sortedcurr->next->num_of_words){
                    if(dup->num_of_words > sortedcurr->next->num_of_words){
                        dup->next = sortedcurr->next;
                        sortedcurr->next = dup;
                    }
                    else{

                        ResultList iter = sortedcurr->next;
                        if(dup->pagerank >= iter->pagerank){
                            dup->next = iter;
                            sortedcurr->next = dup;
                        }
                        else{
                            while(iter!= NULL){
                                if(iter->next == NULL){
                                    iter->next = dup;
                                    break;
                                }
                                if(iter->next->num_of_words < dup->num_of_words){
                                    dup->next = iter->next;
                                    iter->next = dup;
                                    break;
                                }
                                if((iter->next->pagerank < dup->pagerank) && (iter->next->num_of_words == dup->num_of_words)){
                                    dup->next = iter->next;
                                    iter->next = dup;
                                    break;
                                }

                                iter = iter->next;
                            }

                        }
                    }
                    break;
                }
                sortedcurr = sortedcurr->next;
            }
        }
        curr = curr->next;
    }

    return sorted;
}
