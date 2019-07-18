
void gatherwebpages(char *collectionFilename);
char * getfiledir(char * dir, char *collectionfilename);
void PageRankW(d, diffPR, maxIterations);
void getcollectionfilelist();

struct FileListNode {
    char *filepath;
	struct FileListNode *next;
};
typedef struct FileListNode *FileList;
