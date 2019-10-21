#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include"timing.h"
#define STRING_LENGTH 25
#define FALSE  0
#define TRUE 1
#define ASCTOIDX   97
#define TASKBAGSZ  26
// structs used in app

// a task to be used in the task bag
typedef struct Task
{
    char letter;
    int StartPos;  
    int EndPos;
    int * uniqueCount;
    char ** dataarr;
}Task;

// the info needed by a thread
typedef struct ThreadInfo
{
    int threadID;
    long long itemCnt;
    Task * tskBag;   
}ThreadInfo;

// mutexes for sychronization
pthread_mutex_t sycnBagidx        = PTHREAD_MUTEX_INITIALIZER;
// thread globals
int curProcTask;

// function prototypes
long long  getFileLength  ( FILE *file );
long long  fillStringArray(FILE *file, char ** arr , long long flen, Task * TskBg);
void  ShowArray (char ** arr, long long  len);
long long  getUsableFileLength( FILE *file );
long long  fillStringArray_All(FILE *file, char ** arr , long long flen, Task * TskBg);
int Unique( char * testString);
void TestUnique(char ** arr, int arrsz);
void getUniques( char ** data, int * uniqArr, int numItems);
long long getCountUniqueSix(int * uniqArr,char ** dataArr, int numItems);
void *ProcThread( void *arg);
// constants
const char *OutFileNM = "results";
//main
int main( int argc, char *argv[] )  
{
    if(argc<4)
    {
        perror("\nUsage ./part_1_parallel.out <inputfile.csv> <threadCount>\n");
        exit(-1);
    }
    int numThreads =atoi(argv[2]);

    char** inputData;
    char * infile = argv[1];
    FILE * inFilep = fopen(infile, "r");
    long long  numStrings = getUsableFileLength(inFilep);
    Task * TaskBag = malloc (26 * sizeof(Task) );
    inputData =malloc(sizeof(char*)*numStrings);
    for(long long  i=0;i<numStrings;i++)
    {
        inputData[i] = malloc(STRING_LENGTH*sizeof(char));
    }
    int * resultsArray = malloc(sizeof(int) * numStrings);
    printf("\n len = %lld\n",numStrings);
    fillStringArray(inFilep,inputData,numStrings, TaskBag);
    timing_start();
    curProcTask=0;
    getUniques( inputData, resultsArray, numStrings);
    
     


    printf( "\nUnique items with len greater than 6 %lld\n" );

    
    free(TaskBag);
    free(inputData);
    free(resultsArray);
    fclose(inFilep);
}

// functions


long long  getFileLength( FILE *file )
{
    long long  cnt =0;
    char ch;
    ch = getc(file);
    while(ch!=EOF){
        if( ch=='\n'){
            cnt ++;
        }
        ch = getc(file);
    }
    return cnt;
}

long long  getUsableFileLength( FILE *file )
{
    
    fseek(file, 0, SEEK_SET);
    int cnt =0;
    char word[25];
    while (fgets(word,STRING_LENGTH,file))
    {
        if( word[0]>=97 && word[0]<=122 )
        {
            ++cnt;
        }
    }
    return cnt;
}


///Fill string array and TaskBag
long long  fillStringArray_All(FILE *file, char ** arr , long long flen, Task * TskBg)
{
    fseek(file, 0, SEEK_SET);
    long long  i=0;
    while((i<flen) && (fgets(arr[i], STRING_LENGTH, file) )    ) 
	{
        printf("\na-%lld-%s\n",i,arr[i]);
        arr[i][strlen(arr[i]) - 1] = '\0';
        printf("\nb-%lld-%s\n",i,arr[i]);
        i++;
    }
    return i;
}

///Fill string array and TaskBag
long long  fillStringArray(FILE *file, char ** arr , long long flen, Task * TskBg) 
{
    fseek(file, 0, SEEK_SET);
    long long  i=0;
    char word[25];
    char currentchr ='a';
    TskBg[0].StartPos=0;
    TskBg[0].letter='a';
    int tbidx=0;
    while((i<flen) && (fgets(word, STRING_LENGTH, file) )    ) 
	{
        if( word[0]>=97 && word[0]<=122 )
        {
            if( currentchr!= word[0])
            {   
                TskBg[tbidx].EndPos=(i-1);        
                currentchr = word[0];
                tbidx = currentchr - ASCTOIDX;
                TskBg[tbidx].StartPos=(i-1);
            }strcpy(arr[i],word);
            arr[i][strlen(arr[i]) - 1] = '\0';
            i++;
        }
    }
    TskBg[tbidx].EndPos=(i-1);
    return i;
}

int Unique( char * testString)
{
    int retval =TRUE;
    int arr[255] ={0};
    int len =strlen(testString);
    for(int i = 0; i < len; ++i)
        {
            int chasInt = testString[i];
            if(arr[chasInt])
            {
                return FALSE;
            }
            arr[chasInt] = TRUE;
        }
    return retval;
}
void getUniques( char ** data, int * uniqArr, int numItems)
{
    int cnt ;
    for (cnt =0; cnt< numItems; ++cnt)
    {
        int u =Unique(data[cnt]);                                       
        if(u)
        {
            uniqArr[cnt]=strlen(data[cnt]);
        }
    }
}


/*
    display the array
    for debugging porposes
*/
void ShowArray (char ** arr, long long  len)
{
    long long  cnt ;
    printf("Items in array\n"   );
    for (cnt=0;cnt<len;cnt++)
    {
        printf("%s\n" , arr[cnt] );
    }
    printf("END Items in array"   );
}
void TestUnique(char ** arr, int arrsz)
{
    int numtotest =arrsz;
    long long  cnt ;
    printf("Items in array\n"   );
    for (cnt=0;cnt<numtotest;cnt++)
    {
        int u =Unique(arr[cnt]);
        if(u){
        printf("%s\n" , arr[cnt] );
        }    
        
    }
    printf("END Items in array"   );
}
long long getCountUniqueSix(int * uniqArr,char ** dataArr, int numItems)
{
   FILE * outFilep = fopen(OutFileNM, "w");
    long long numUniqueSix =0;
    int cntr=0;
    for (;cntr<numItems;++cntr)
    {
        if( uniqArr[cntr]>=6)
        {
            ++numUniqueSix;    
            fprintf(outFilep,"%s\n",dataArr[cntr]);
        }
    }
    fclose(outFilep);
    return numUniqueSix;
}

void *ProcThread( void *arg)
{
    ThreadInfo *inParams = (ThreadInfo *)arg;
    int hasTasks = TRUE;
    while(hasTasks)
    {
        pthread_mutex_lock(&sycnBagidx);
        if( curProcTask>=TASKBAGSZ)
        {
            hasTasks=FALSE;
            //process currprocTas.
            pthread_mutex_unlock(&sycnBagidx);

            
        }else
        {
            int toproc = curProcTask;
            ++curProcTask;
            pthread_mutex_unlock(&sycnBagidx);
            // process toProc
            int start = inParams->tskBag->StartPos;
            for( ;start<= inParams->tskBag->EndPos;++start)
            {
                if(inParams->tskBag->uniqueCount[start]>=6)
                {

                    inParams->itemCnt++;
                }
            }
        }
    }






}