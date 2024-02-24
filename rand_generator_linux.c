#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int rand_generator(int total);

/*This program generated designated amount of uniformally 
distributed random integers. Each the integer and bit are stored
in files as one byte binary digits*/
int main(int argc, char *argv[])
{
    /*The first argument of this program ,argv[1], indicates
    the amount of the random number to be generated.*/
    int* total=(int*)malloc(sizeof(int));
    
    /*Validation of argument input and conversion into integer.*/
    if(argv[1]==NULL||sscanf(argv[1],"%d",total)!=1)
    {
        printf("Please give the amount of number to be generated"\
        " in the commandline argument. It has to be an integer.\n"\
        "E.g. $ rand_generator 1000000.\n");
        exit(EXIT_FAILURE);
    }

    /*Call the random number generating function.*/
    rand_generator(*total);

    return 0;
}

/*This function generates n randomly distributed integers
in the range of [1,127].*/
int rand_generator(int total)
{
    int n,bit;
    int i=0;
    
    /*This is used to count the occurance of each generated number,
    to test if the numbers are truely uniformlly distributed.*/
    int count[127];
    for(int j=0;j<127;j++)
        count[j]=0;

    /*Use the current second as the seed to generate random number.*/
    unsigned sec=(unsigned)time(NULL);
    srand(sec);

    /*Allocate space and create files to save the numbers.
    In binary mode to eliminate the burden of character conversion.*/
    char* file_path[8];
    FILE* fp[8];
    for(int j=0;j<8;j++)
    {
        file_path[j]=malloc(sizeof(char)*30);
        fp[j]=malloc(sizeof(FILE));
    }

    /*Create one file for integer.*/
    snprintf(file_path[7],30,"Integer_%d.dat",total);
    fp[7]=fopen(file_path[7],"wb");

    /*Create 7 files for bits.*/
    for(int j=0;j<7;j++)
    {
        snprintf(file_path[j],30,"Binary_%d_%d.dat",total,j);
        fp[j]=fopen(file_path[j],"wb"); 
    }

    /*This while loop keeps generating random number
    and save them to files until the total amount is reached.
    i stands for the amount of accepted random numbers.*/
    while(i<total)
    {
        /*Call rand() to generate random integer,\
        then adjust the range in to [1,127].*/
        n=rand();
        n=(int)ceilf((float)n/RAND_MAX*127);

        count[n-1]++;
        
        /*Save the integer into file.*/
        fputc(n,fp[7]);

        /*Save the bits into their corredponding files.*/
        for(int j=6;j>=0;j--)
        {
            bit=(n&(int)pow(2,j))>>j;
            fputc(bit,fp[j]);
        }
        
        i++;
    }

    for(int j=0;j<=7;j++)
        fclose(fp[j]);

    /*Print the count of each generated number.*/
    printf("\nCount:\n");
    for(int j=0;j<127;j++)
        printf("Integer=%d, count=%d\n",j+1,count[j]);
    printf("\n%d integers generated in total.\n",total);

    return 0;
}
