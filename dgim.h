typedef struct Bucket BUCKET;
struct Bucket 
{
    char size;
    int ts;
    BUCKET* next;
    BUCKET* prev;
};

typedef struct Buc_list BUC_LIST;  //Mark the front and end pointers of the bucket list.
struct Buc_list
{
    BUCKET* front;
    BUCKET* rear;
};

int init(char** file_names, int file_num, FILE** fps, BUC_LIST** buc_list_ptrs);
int dgim_step(FILE* fp, BUC_LIST* buc_list_ptr, int cts);
int check_rear(BUC_LIST* buc_list_ptr, int cts);
BUCKET* add_buc(BUC_LIST* buc_list_ptr, int cts);
int check_merge(BUCKET* check_pos, BUC_LIST* buc_list_ptr);
int print_buc_lists(BUC_LIST** buc_list_ptrs, int file_num, int total_read);
char* estimate(BUC_LIST** buc_list_ptrs, int file_num, int win_size ,char* win_truth, int total_read);
