struct FileListNode {
    char *filename;
	struct FileListNode *next;
};
typedef struct FileListNode *FileList;


void gatherwebpages(char *collectionFilename);
char * getfiledir(char *collectionfilename);
void PageRankW(float d, float diffPR, float maxIterations);



FileList getcollectionfilelist();
FileList newFileNode(char *inputfile);
FileList addtofilelist(FileList addonto, char *inputfile);
