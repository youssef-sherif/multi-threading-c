#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h> 


int ** result;


typedef struct matrix_t {
    int ** mat;
    int row_length;
    int col_length;
} matrix_t;

typedef struct mult_t {
   int i;
   int j;
   int k;
   matrix_t matA;
   matrix_t matB;
} mult_t;


void display_matrix(matrix_t * matrix) {
    int i, j;
    for (i = 0; i < matrix->row_length; i++) {
        for (j = 0; j < matrix->col_length; j++) {
            printf("%d ", matrix->mat[i][j]);
        }
        printf("\n");
    }
}

matrix_t * read_matrix(FILE * file)
{ 
    int i = 0, j = 0, row_length, col_length, buffer;

    for(j = 0; j < 2;) {
        fscanf(file, "%d" , &buffer);

        if(j == 0) {
            row_length = buffer;
        }
        else {
            col_length = buffer;
        }
        j++;         
    }   
    printf("%d %d \n", row_length, col_length);

    int ** mat = (int **) malloc( sizeof(int) * row_length );
    mat[0] = (int *) malloc( sizeof(int) * col_length );

    for(i = 0; i < row_length; i++) {
        mat[i] = (int *) malloc( sizeof(int) * col_length );
        for(j = 0; j < col_length;) {
            fscanf(file, "%d" , &buffer);
            mat[i][j] = buffer;
            j++;
        } 
    }

    matrix_t * matrix = malloc(sizeof(matrix));
    matrix->mat = mat;
    matrix->row_length = row_length;
    matrix->col_length = col_length;

    return matrix;
}

void* compute_row_in_thread(void* arg)
{
    int j, k, i;
    i = ((mult_t *)(arg))->i;
    matrix_t matA = ((mult_t *)(arg))->matA;
    matrix_t matB = ((mult_t *)(arg))->matB;

    // Compute row
    for (j = 0; j < matB.col_length; j++) {
        for (k = 0; k < matA.col_length; k++) {
            result[i][j] += matA.mat[i][k] * matB.mat[k][j];            
        }                
    }
}

void* compute_element_in_thread(void* arg)
{
    int i, j, k;
    i = ((mult_t *)(arg))->i;
    j = ((mult_t *)(arg))->j;
    matrix_t matA = ((mult_t *)(arg))->matA;
    matrix_t matB = ((mult_t *)(arg))->matB;

    // Compute element
    for (k = 0; k < matA.col_length; k++) {
       result[i][j] += matA.mat[i][k] * matB.mat[k][j];            
    }          
}


long multiply1(matrix_t * matA, matrix_t * matB) 
{
    if(matA->col_length != matB->row_length) {
        return -1;
    }

    int i, j, number_of_threads = matA->row_length;
    clock_t t1, t2; 
    t1 = clock(); 

    result =  (int **) malloc( sizeof(int) * matA->row_length );
    result[0] = (int *) malloc( sizeof(int) * matB->col_length );

    pthread_t threads[number_of_threads];

    for (i = 0; i < matA->row_length; i++) {  
        result[i] = (int *) malloc( sizeof(int) * matB->col_length );  
        for (j = 0; j < matB->col_length; j++) { 
            mult_t *arg = malloc(sizeof(*arg));
            arg->i = i; 
            arg->j = j; 
            arg->matA = *matA;
            arg->matB = *matB; 
            pthread_create(&threads[i], NULL, compute_element_in_thread, (void *) arg);
        }
    }

    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    t2 = clock();

    return (t2 - t1);
}


long multiply2(matrix_t * matA, matrix_t * matB) 
{
    if(matA->col_length != matB->row_length) {
        return -1;
    }

    int i, j, number_of_threads = matA->row_length;
    clock_t t1, t2; 
    t1 = clock(); 

    result =  (int **) malloc( sizeof(int) * matA->row_length );
    result[0] = (int *) malloc( sizeof(int) * matB->col_length );

    pthread_t threads[number_of_threads];

    for (i = 0; i < matA->row_length; i++) { 
        result[i] = (int *) malloc( sizeof(int) * matB->col_length );      
        mult_t *arg = malloc(sizeof(*arg));
        arg->i = i;   
        arg->matA = *matA;
        arg->matB = *matB; 
        pthread_create(&threads[i], NULL, compute_row_in_thread, (void *) arg);
    }

    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    t2 = clock();

    return (t2 - t1);
}

int main()
{
    FILE *file;
    int i, j, choose;
    long time_taken;

    file = fopen("matA.txt", "r");
    matrix_t * matA = read_matrix(file);

    file = fopen("matB.txt", "r");
    matrix_t * matB = read_matrix(file);

    printf("Matrix A: \n");
    display_matrix(matA);

    printf("\n");

    printf("Matrix B: \n");
    display_matrix(matB);


    printf("\n");

    printf("Press 1 for multiply1 and 2 for multiply2: ");
    scanf("%d",  &choose);

    if(choose == 1) {
        time_taken = multiply1(matA, matB);
    }
    else if(choose == 2) {
        time_taken = multiply2(matA, matB);
    }

if(time_taken == -1) {
    printf("Cannot multiply matrices\n");
    return -1;
}

    printf("\n");

    file = fopen("result.txt", "w");

    printf("Multiplication of A and B: \n");
    fprintf(file, "Time taken %ld \n", time_taken);
    for (i = 0; i < matA->row_length; i++) {
        for (j = 0; j < matB->col_length; j++) {
            printf("%d ", result[i][j]);
            fprintf(file, "%d ", result[i][j]);
        }
        printf("\n");
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}
