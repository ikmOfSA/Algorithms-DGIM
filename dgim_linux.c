#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dgim.h"  //All the structs and function declarations.

int main()
{
    int win_size=10000;  //Window size.
    int total_read=0;  //Count the total bytes read.
    int cts=1;  //Current time stamp, ranging from [1,10000].
    char win_truth[10000];  //The window of the integer numbers.

    char* file_names[]={
        "Binary_1000000_0.dat",
        "Binary_1000000_1.dat",
        "Binary_1000000_2.dat",
        "Binary_1000000_3.dat",
        "Binary_1000000_4.dat",
        "Binary_1000000_5.dat",
        "Binary_1000000_6.dat",
        "Integer_1000000.dat"};

    int file_num = sizeof(file_names)/sizeof(file_names[0]);
    FILE** fps = malloc(sizeof(FILE*) * file_num);   //Opened files.
    BUC_LIST** buc_list_ptrs = malloc(sizeof(BUC_LIST*) * file_num);  //Initialize bucket lists for every file.

    init(file_names, file_num, fps, buc_list_ptrs);  //Other initializations.

    char c;
    FILE* fp_int = fps[file_num-1];  //fp for the integer file.
    char* str_estimate = malloc(sizeof(char)*10000); //This and the following var are used to store the print outputs, to achieve better output format.
    char* new_estimate;

    while((c=fgetc(fp_int))!=EOF)  //Read integers from the integer file one by one.
    {
        total_read++;
        win_truth[cts-1]=c;  //Store the just read integer into the integer window.

        for(int i=0;i<(file_num-1);i++)
            dgim_step(fps[i],buc_list_ptrs[i],cts);  //DGIM algorithm for every bit file move forward one step, i.e., read one number.

        if(total_read==10000||total_read==500000||total_read==1000000)
            print_buc_lists(buc_list_ptrs, file_num, total_read); //Print the bucket lists.

        if((total_read%100000)==0)
        {
            new_estimate = estimate(buc_list_ptrs, file_num, win_size, win_truth, total_read); //Store the estimate output, print all later.
            strcat(str_estimate, new_estimate);
        }

        cts++;
        cts=(cts>win_size)?cts-win_size:cts;
    }

    printf("%s", str_estimate);  //Print the stored estimation.

    return 0;
}

/*Open integer and binary files, and set the data structure for them.*/
int init(char** file_names, int file_num, FILE** fps, BUC_LIST** buc_list_ptrs)
{
    for(int i=0;i<file_num;i++)
    {
        fps[i]=fopen(file_names[i],"rb");

        buc_list_ptrs[i]=malloc(sizeof(BUC_LIST)); //buc_list_ptrs[], a list of BUC_LIST pointers, which then points to the bucket list for every number files.
        buc_list_ptrs[i]->front=NULL;
        buc_list_ptrs[i]->rear=NULL;
    }

    return 0;
}

/*dgim algorithm move forward one step, that is, read one number.*/
int dgim_step(FILE* fp, BUC_LIST* buc_list_ptr, int cts)
{
    char c;

    if((c=fgetc(fp))!=EOF)
    {
        check_rear(buc_list_ptr, cts);  //Check if the last bucket is out of window.

        if(c==1)
        {
            BUCKET* second_to_front = add_buc(buc_list_ptr, cts);  //Add new bucket, return a pointer to the second bucket, which is used in the check_merge();
            check_merge(second_to_front, buc_list_ptr);  //Check if some buckets need merge.
        }
    }

    return 0;
}

/*Check if the last bucket is out of window*/
int check_rear(BUC_LIST* buc_list_ptr, int cts)
{
    BUCKET* rear=buc_list_ptr->rear;

    if((rear!=NULL)&&(rear->ts==cts))  //If out of window.
    {
        buc_list_ptr->rear=rear->prev;
        buc_list_ptr->rear->next=NULL;

        free(rear); //Delete the out-of-window bucket.
    }

    return 0;
}

/*Add one new bucket, return the second bucket pointer, which is used in the merge operation.*/
BUCKET* add_buc(BUC_LIST* buc_list_ptr, int cts)
{
    BUCKET* front=buc_list_ptr->front;
    BUCKET* rear=buc_list_ptr->rear;
    BUCKET* second_to_front;

    BUCKET* new=malloc(sizeof(BUCKET));
    new->size=0; //Actuall the exponent to 2, the real size = 1<<bucket->size.
    new->ts=cts;
    new->next=NULL;
    new->prev=NULL;

    if((front==NULL) && (rear==NULL))  //i.e., if the new bucket is the first bucket.
    {
        buc_list_ptr->front = new;
        buc_list_ptr->rear = new;
        second_to_front = NULL;
    }
    else  //Already exist some buckets.
    {
        new->next = front;
        front->prev = new;
        buc_list_ptr->front = new;
        second_to_front = front;
    }

    return second_to_front;
}

/*Check if the bucket list illegal and do the fix, recurrsively call it self, every call does one merge.
The trick is, because all the size of 2^* exist in the list(starting from 2^0),
you only have to check if two adjacent buckets have the same size,
instead of checking three.*/
int check_merge(BUCKET* check_pos, BUC_LIST* buc_list_ptr)
{
    BUCKET* first = check_pos;
    BUCKET* next_to_check = NULL;

    if((first!=NULL)&&(first->next!=NULL))  //If there is a possibility to merge.
    {
        BUCKET* second=first->next;
        BUCKET* third=second->next;

        if(first->size==second->size)
        {
            first->size++; //Notice the 'size' is in fact the exponent to 2.

            if(third!=NULL)  //If there are following bucket after merge.
            {
                first->next = third;
                third->prev = first;
                next_to_check = third;
            }
            else //If the merged bucket is the last bucket, in this case, no need to do more check.
            {
                first->next = NULL;
                buc_list_ptr->rear = first;

                return 0;
            }

            free(second);

            check_merge(next_to_check, buc_list_ptr); //After one merge, check to see if there are new merges along the line.
        }
    }

    return 0;
}

/*Print the bucket list*/
int print_buc_lists(BUC_LIST** buc_list_ptrs, int file_num, int total_read)
{
    BUCKET* buc_ptr;

    printf("\nTimestamp = %d:\n", total_read);

    for(int i=0;i<(file_num-1);i++)
    {
       printf("\nBit position %d:\n",i);

       buc_ptr=buc_list_ptrs[i]->front;
       while(buc_ptr!=NULL)
       {
           printf("(%d,%d)->",buc_ptr->ts,1<<(buc_ptr->size));//Notice the 'size' is in fact the exponent to 2.
           buc_ptr=buc_ptr->next;
       }
       printf("END\n");
    }
    printf("-----------------------------");

    return 0;
}

/*Calculate the estimation and the truth value, store them in a string for later printing.*/
char* estimate(BUC_LIST** buc_list_ptrs, int file_num, int win_size ,char* win_truth, int total_read)
{
    int sum_real=0; //Calculate the real sum of the integers in the window.
    for(int i=0;i<win_size;i++)
        sum_real+=win_truth[i];
    
    int sum_estimate=0;
    int* sum_bit = malloc(sizeof(int) * (file_num-1));
    BUCKET* buc_ptr;

    for(int i=0;i<(file_num-1);i++) //Calculate the estimated sum for every bit files, then the final estimation.
    {
        sum_bit[i]=0;
        buc_ptr=buc_list_ptrs[i]->front;

        while(buc_ptr!=NULL)
        {
            if((buc_ptr->size!=0) && (buc_ptr->next==NULL))  //If the final bucket, sum half the size.Notice the 'size' is in fact the exponent to 2.
                sum_bit[i]+=1<<(buc_ptr->size-1);
            else
                sum_bit[i]+=1<<(buc_ptr->size);  

            buc_ptr=buc_ptr->next;
        }

        sum_estimate+=sum_bit[i]*(1<<i);  //Calculate the final estimation.
    }

    char* estimate_state = malloc(sizeof(char) * 200);
    sprintf(estimate_state, "\nTimestamp = %d:\n    estimate: %d\n    truth: %d\n", total_read, sum_estimate, sum_real);

    return estimate_state;
}
