#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include"timing.h"
#define STRING_LENGTH 30
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
    long long itemCnt6U;
    long long itemCntAllU;
    Task * tskBag; 
    pthread_t thread;  
}ThreadInfo;
// mutexes for thread  sychronization
pthread_mutex_t sycnBagidx        = PTHREAD_MUTEX_INITIALIZER;

// thread globals
int curProcTask;

// constants
const char *OutFileNM = "results";

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
void DisplayArray( FILE * outLocation, char ** arr,int * UniqueArr,  int DataLength, int dbg);
//main
int main( int argc, char *argv[] )  
{
    printf("-1-\n\n");
   
    if(argc<3)
    {
        perror("\nUsage ./part_1_parallel.out <inputfile.csv> <threadCount>\n");
        exit(-1);
    }
    printf(" %s %s %s\n\n",argv[0],argv[1],argv[2]);
    
    char* one = argv[2];
    printf("-2a-");
    int tst= atoi(one);
    printf("-2b-");
    //exit(-1);
    int numThreads =tst;//atoi(argv[2]);
    printf("-2-");
    
    char ** inputData;
    char * infile = argv[1];
    
    FILE * inFilep = fopen(infile, "r");
    printf("in file = %s", infile);
   
    long long  numStrings = getUsableFileLength(inFilep);
     
    Task * TaskBag = malloc (26 * sizeof(Task) );
    inputData =malloc(sizeof(char*)*numStrings);
    printf("-3-");
    for(long long  i=0;i<numStrings;i++)
    {
        inputData[i] = malloc(STRING_LENGTH*sizeof(char));
    }

    int * resultsArray = malloc(sizeof(int) * numStrings);
     
    int cnts =fillStringArray(inFilep,inputData,numStrings, TaskBag);
    printf( "\n\n---%d---\n\n", cnts);
    
     printf( "\n\n---%c--%d-\n\n", TaskBag[2].letter, TaskBag[2].StartPos    );

    printf("-5-");
    getUniques( inputData, resultsArray, numStrings);
    timing_start();
    printf("-6-");
    curProcTask=0;
    ThreadInfo * threads = (ThreadInfo *)malloc (numThreads * sizeof(ThreadInfo));
    int thcnt;
    for(thcnt=0;thcnt<numThreads;++thcnt)
    {
        threads[thcnt].threadID     = thcnt  ;
        threads[thcnt].itemCnt6U    = 0      ;
        threads[thcnt].itemCntAllU  = 0      ;
        threads[thcnt].tskBag       = TaskBag;
        pthread_create(&threads[thcnt].thread,NULL,ProcThread ,(void *) &threads[thcnt] );
    }
    printf("-7-");
    //join the threads
    void *status;
    for (thcnt =0; thcnt<numThreads;++thcnt)
    {
        pthread_join(threads[thcnt].thread, &status);
    }
    timing_stop();
    // get the count
    // sum the items
    long long sumAllThreads6 =0;
    long long sumAllThreads =0;
    for (thcnt =0; thcnt<numThreads;++thcnt)
    {
        sumAllThreads6 += threads[thcnt].itemCnt6U   ;
        sumAllThreads  += threads[thcnt].itemCntAllU;       ;
        printf("Theread %d found %lld unique Items with 6 or more letters  \n",threads[thcnt].threadID, threads[thcnt].itemCnt6U  );
        printf("Theread %d found %lld unique\n"                               ,threads[thcnt].threadID, threads[thcnt].itemCntAllU);
    }
     

    printf( "\nUnique items with len greater than 6 %lld\n", sumAllThreads6);
    printf( "\nTotal unique words %lld\n"                  , sumAllThreads );
    print_timing();
    FILE *fpout;
    fpout = fopen(OutFileNM, "w");
    // output text file
    DisplayArray( fpout, inputData, resultsArray,  numStrings, FALSE);
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
     
    //while (fgets(word,STRING_LENGTH,file))
    char * word = (char *)malloc(STRING_LENGTH * sizeof(char));
      size_t size = STRING_LENGTH;
    while (getline(&word,&size,file)>=0)
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
    printf("flen %lld \n\n ", flen);
    fseek(file, 0, SEEK_SET);
    long long  i=0;
    //char word[25];
    char currentchr ='a';
    TskBg[0].StartPos=0;
    TskBg[0].letter='a';
    int tbidx=0;
      size_t size = STRING_LENGTH;
    char * word = (char *)malloc(STRING_LENGTH * sizeof(char));
    while((i<flen) && (getline(&word,&size,file)>=0 )    ) 
	{
        
        if( word[0]>=97 && word[0]<=122 )
        {
           
          // printf("\n!!%s %c %c  \n", word,currentchr,word[0] );
            if( currentchr!= word[0])
            {    printf("\n%s %c %c %s \n", word,currentchr,word[0], arr[i-1]);
                TskBg[tbidx].EndPos=(i-1);
                         
                currentchr = word[0];
                tbidx = currentchr - ASCTOIDX;
                TskBg[tbidx].StartPos=(i-1);
                TskBg[tbidx].letter = currentchr;
            }strncpy(arr[i],word,STRING_LENGTH);
            arr[i][strlen(arr[i]) - 1] = '\0';
            i++;
        }
    }
    TskBg[tbidx].EndPos=(i-1);
    printf("flen %lld \n\n ", flen);
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

/*
    char ** data   -  strings from file  
    int * uniqArr  -  the array with the unique data
    int numItems   -  number of data ites
    populates unique array with length of unique strings
*/
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
            int start = inParams->tskBag[toproc].StartPos;
            for( ;start<= inParams->tskBag[toproc].EndPos;++start)
            {
                if(inParams->tskBag[toproc].uniqueCount[start]>=6)
                {
                    inParams->itemCnt6U ++;
                }
                if(inParams->tskBag[toproc].uniqueCount[start]>0)
                {
                    inParams->itemCntAllU ++;
                }
            }
        }
    }
}

void DisplayArray( FILE * outLocation, char ** arr,int * UniqueArr,  int DataLength, int dbg)
{
    long long  cnt=0;
    fprintf( outLocation ,"ALL UNIQUE STRINGS\n"   );
    for(;cnt<DataLength;cnt++)
    {
        if(dbg)
        {
            fprintf( outLocation ,"%s\n",arr[cnt]   );
        }
        else if(UniqueArr[cnt]>=6)
        {
            fprintf( outLocation ,"%s\n",arr[cnt]   );
        }
    }
}




/*
    output
     To summarize, your parallel program should have the following output: 
1)the total number of words with 
unique letters, 
2)the number found by each worker, 
3)the elapsed time for the compute phase, 
4)and the words that contain at least 6 unique letters



*/