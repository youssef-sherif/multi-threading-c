#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <time.h> 
#include <math.h>
      
int * a;
int arr_size; 
int part = 0;

pthread_t threads[40]; 

typedef struct merge_sort_t {
   int low;
   int high;
} merge_sort_t;

void read_arr()
{ 
    FILE * file = fopen("arr.txt", "r");
    int i = 0, j = 0, buffer;
  
    fscanf(file, "%d" , &buffer);

    arr_size = buffer;

    a = (int *) malloc( sizeof(int *) * arr_size );

    for(i = 0; i < arr_size; i++) {
        fscanf(file, "%d" , &buffer);
        a[i] = buffer;
    }
    fclose(file);
}
  
void merge(int low, int mid, int high) 
{
    int left[mid - low + 1]; 
    int right[high - mid]; 
  
    // n1 is size of left part and n2 is size 
    // of right part 
    int n1 = mid - low + 1, n2 = high - mid, i, j; 
  
    // storing values in left part 
    for (i = 0; i < n1; i++) 
        left[i] = a[i + low]; 
  
    // storing values in right part 
    for (i = 0; i < n2; i++) 
        right[i] = a[i + mid + 1]; 
  
    int k = low; 
    i = j = 0; 
  
    // merge left and right in ascending order 
    while (i < n1 && j < n2) { 
        if (left[i] <= right[j]) 
            a[k++] = left[i++]; 
        else
            a[k++] = right[j++]; 
    } 
  
    // insert remaining values from left 
    while (i < n1) { 
        a[k++] = left[i++]; 
    } 
  
    // insert remaining values from right 
    while (j < n2) { 
        a[k++] = right[j++]; 
    } 
} 
  
// merge sort function 
void* merge_sort(void * arg) 
{   
    int low, high;
    low = ((merge_sort_t *)(arg))->low;
    high = ((merge_sort_t *)(arg))->high;

    // calculating mid point of array 
    int mid = low + (high - low) / 2; 
    if (low < high) { 

        // calling first half 
        arg = malloc(sizeof(*arg));
        ((merge_sort_t *)arg)->low = low; 
        ((merge_sort_t *)arg)->high = mid; 
        pthread_create(&threads[part], NULL, merge_sort, (void *) arg); 
        pthread_join(threads[part++], NULL);

        // calling second half
        arg = malloc(sizeof((*arg)));
        ((merge_sort_t *)arg)->low = mid + 1; 
        ((merge_sort_t *)arg)->high = high; 
        pthread_create(&threads[part], NULL, merge_sort, (void *) arg);  
        pthread_join(threads[part++], NULL);

        // merging the two halves
        merge(low, mid, high); 
    } 
}  

void multithreaded_merge_sort(int low, int high) 
{
    merge_sort_t *arg;
    int mid = low + (high - low) / 2; 
    
    arg = malloc(sizeof(*arg));
    arg->low = low; 
    arg->high = high;  
    merge_sort((void *) arg);  
}  

int main() 
{ 
    int i;

    read_arr();

    clock_t t1, t2; 
  
    t1 = clock(); 
    
    multithreaded_merge_sort(0, arr_size - 1);

    t2 = clock(); 
  
    // displaying sorted array 
    printf("\nSorted array: "); 
    for (i = 0; i < arr_size; i++) {
       printf("%d ", a[i]); 
    }
  
    // time taken by merge sort in seconds 
    printf("\nTime taken: %ld \n", (t2 - t1) ); 
  
    return 0; 
}