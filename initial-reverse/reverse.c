/*
Sources:
fprintf usage: https://man7.org/linux/man-pages/man3/fprintf.3.html
Comparing command line arguments: https://man7.org/linux/man-pages/man3/strcmp.3.html
Finding files identificality using st_ino and st_dev of stat: https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/stat.htm
Memory allocation, Linked list and its handling: Len, (2023). A program that analyzes political speeches by processing their metadata [Unpublished source code]. Personal collection.
AI usage: This code is commented using Copilot-style comments, and has been edited and added by the creator based on those suggestions. 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

// This program reverses the lines of a text file.
// It can read from an input file and write to an output file, or use standard input/output.
// It uses a linked list to store the lines in reverse order before writing them back.

// Error messages
#define USAGE "usage: reverse <input> <output>\n"
#define ERROR_OPEN_FILE "reverse: cannot open file '%s'\n"
#define ERROR_MALLOC "malloc failed\n"
#define ERROR_SAME_FILE "reverse: input and output file must differ\n"

typedef struct informations{// This is for a linked list that contains a line from input file
  char *pLine;
  struct informations *pNext;
}INFO;

char* stringDuplicate(char* pLine);// Duplicates a string
INFO *allocateMemoryForINFO(INFO *pNew);// Allocates memory for a linked list node
INFO *freeMemory(INFO *pStart);// Frees all memory used by the linked list
INFO* readLinesFromStream(FILE *stream, INFO *pStart);// Reads lines from a file/stream into a linked list
void writeToStream(FILE *stream, INFO *pStart);// Writes the linked list to a file/stream

char* stringDuplicate(char* pLine){
  char* pAllocated = NULL;
  if(pLine == NULL){
    return NULL;
  }
  // Allocate memory for the string and copy the content
  pAllocated = (char*)malloc(strlen(pLine) + 1); // +1 for null terminator
  if(pAllocated == NULL){
    fprintf(stderr, ERROR_MALLOC);
    exit(1);
  }
  strcpy(pAllocated, pLine);
  return pAllocated;
}

INFO *allocateMemoryForINFO(INFO *pNew){
  if((pNew = (INFO*)malloc(sizeof(INFO))) == NULL){
    fprintf(stderr, ERROR_MALLOC);
    exit(1);
  }
  return pNew;
}

INFO *freeMemory(INFO *pStart){
  INFO *ptr = NULL;
  while(pStart != NULL){// Saving the next node to ptr, freeing the current node(pStart) and setting the saved node as current node
    ptr = pStart->pNext;
    free(pStart->pLine);// Free the line of text
    free(pStart);
    pStart = ptr;
  }
  return pStart;
}

INFO* readLinesFromStream(FILE *stream, INFO *pStart){
  char *pLine = NULL;// Pointer for dynamically allocated line
  size_t len = 0;// Length of the allocated buffer
  INFO *pNew = NULL;
  if(pStart != NULL){// Free existing list if any
    freeMemory(pStart);
    pStart = NULL;
  }
  while(getline(&pLine, &len, stream) != -1){// Read line dynamically
    pNew = allocateMemoryForINFO(pNew);
    pNew->pLine = stringDuplicate(pLine);
    pNew->pNext = pStart;// Insert at the beginning to reverse order
    pStart = pNew;// Update head of the list
  }
  free(pLine);// Free the buffer allocated by getline
  return pStart;
}

void writeToStream(FILE *stream, INFO *pStart){
  INFO *pInfo = pStart;
  while(pInfo != NULL){
    fprintf(stream, "%s", pInfo->pLine);// Write each line to the stream
    pInfo = pInfo->pNext;
  }
}

int main(int argc, char *argv[]){
  INFO *pStart = NULL;
  char *pInputFile = NULL, *pOutputFile = NULL;
  struct stat inputStat, outputStat;
  FILE *inputStream = stdin, *outputStream = stdout;// If there's no arguments, read from standard input write to standard output
  
  // Defining the input and output streams:
  if(argc > 3){// Too many arguments
    fprintf(stderr, USAGE);
    exit(1);
  }if(argc == 3){// Input and output file
    if(strcmp(argv[1], argv[2]) == 0){// Comparing the names of input and output file
      fprintf(stderr, ERROR_SAME_FILE);
      exit(1);
    }if(stat(argv[1], &inputStat) == 0 && stat(argv[2], &outputStat) == 0){// Comparing are given
      if(inputStat.st_dev == outputStat.st_dev && inputStat.st_ino == outputStat.st_ino){//files actually same file 
	fprintf(stderr, ERROR_SAME_FILE);
	exit(1);
      }
    }
    pInputFile = argv[1];
    pOutputFile = argv[2];
    inputStream = fopen(pInputFile, "r");
    if(inputStream == NULL){
      fprintf(stderr, ERROR_OPEN_FILE, pInputFile);
      exit(1);
    }
    outputStream = fopen(pOutputFile, "w");
    if(outputStream == NULL){
      fprintf(stderr, ERROR_OPEN_FILE, pOutputFile);
      exit(1);
    }
  }if(argc == 2){// Printing reversed input file to standard output
    pInputFile = argv[1];
    inputStream = fopen(pInputFile, "r");
    if(inputStream == NULL){
      fprintf(stderr, ERROR_OPEN_FILE, pInputFile);
      exit(1);
    }
  }
  // Input and output files are defined. Reversing the input below. 
  pStart = readLinesFromStream(inputStream, pStart);
  writeToStream(outputStream, pStart);
  freeMemory(pStart);

  if(inputStream != stdin) fclose(inputStream);
  if(outputStream != stdout) fclose(outputStream);

  return 0;
}
