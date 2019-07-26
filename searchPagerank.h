#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>


struct SearchWordNode {
    char *word;
	struct SearchWordNode *next;
};
typedef struct SearchWordNode *SearchList;

struct ResultNode {
    char *url;
    int num_of_words;
    float pagerank;
	struct ResultNode *next;
};
typedef struct ResultNode *ResultList;

void searchpagerank(SearchList searchwords);
SearchList createWordList(char **input);
SearchList newSearchNode(char *inputword);
SearchList addtosearchlist(SearchList addonto, char *inputword);


ResultList getassociatedurl(SearchList searchwords);
ResultList newResultNode(char *inputword);
ResultList addtoresultlist(ResultList addonto, char *inputword);
ResultList resultnodeexist(ResultList addonto, char *inputurl);
float getpagerank(char *inputurl);
ResultList duplicate(ResultList original);
ResultList sortsearchresults(ResultList tosort);
