#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define STRING_LENGTH 25
#define FALSE  0
#define TRUE 1
long long  getFileLength  ( FILE *file );
long long  fillStringArray(FILE *file, char ** arr , long long flen);
void  ShowArray (char ** arr, long long  len);
long long  getUsableFileLength( FILE *file );
long long  fillStringArray_All(FILE *file, char ** arr , long long flen);
int Unique( char * testString);
void TestUnique(char ** arr, int arrsz);
int main( int argc, char *argv[] )  
{
    char** inputData;
    char * infile = argv[1];
    FILE * inFilep = fopen(infile, "r");
    long long  numStrings = getUsableFileLength(inFilep);

    inputData =malloc(sizeof(char*)*numStrings);
    
    for(long long  i=0;i<numStrings;i++)
    {
        inputData[i] = malloc(STRING_LENGTH*sizeof(char));
    }
    int * resultsArray = malloc(sizeof(int) * numStrings);
    printf("\n len = %lld\n",numStrings);
    fillStringArray(inFilep,inputData,numStrings);
    //ShowArray(inputData,numStrings);
    TestUnique(inputData,numStrings);




    fclose(inFilep);
}




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



long long  fillStringArray_All(FILE *file, char ** arr , long long flen)
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

long long  fillStringArray(FILE *file, char ** arr , long long flen)
{
    fseek(file, 0, SEEK_SET);
    long long  i=0;
    char word[25];
    while((i<flen) && (fgets(word, STRING_LENGTH, file) )    ) 
	{
        if( word[0]>=97 && word[0]<=122 )
        {
            strcpy(arr[i],word);
            arr[i][strlen(arr[i]) - 1] = '\0';
            i++;
        }
    } 
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



