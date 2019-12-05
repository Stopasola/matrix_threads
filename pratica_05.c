#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>    // sync()
#include <stdint.h> /* for uint64 definition */
#include "omp.h"

#define BILLION 1000000000L

void PrintMatriz(double **M, int l, int c){
  int i, j;
  for(i = 0; i < l; i++){
    for(j = 0; j < c; j++){
      printf("%.1lf ", M[i][j] );
    }
    printf("\n");
  }
  printf("\n");

}

void geraMatriz(double **M, int l, int c){
  int i, j;
  for(i = 0; i < l; i++){
    for(j = 0; j < c; j++){
      M[i][j] = (double)(rand() % 1000 + 1)/100; //(0~100 double)
    }
  }

  return;
}

double **alocaMatriz(int l, int c){
  int i;
  double **A;
  A = (double **) calloc(l, sizeof(double *));
  for(i = 0; i < l; i++)
    A[i] = (double *) calloc(c, sizeof(double));

  return A;
}


double op_matriz_seq(double **M1, double **M2, double **M3, int l, int c){

  double dif_mat = 0;
  for (int i=0;i<l; i++) {
      for (int j=0; j<l; j++) {
          M1[i][j] = pow(M1[i][j], 2);

          M2[i][j] = pow(M2[i][j], 2);

          M3[i][j] = (M1[i][j] - M2[i][j]);

          dif_mat += M3[i][j];
      }
  }
  return dif_mat;

}

// Coversão de nanosegundo para segundo

double timespecToSec(struct timespec *time){
    return (double)(time->tv_sec * 1000000000 + time->tv_nsec)/1000000000.0;
}

double op_matriz_thread(double **M1, double **M2, double **M3, int l, int c){

  //int **vet = (int**) malloc(10 * sizeof(int*));
  int i, j;
  double dif_mat = 0;
  //for(int i = 0; i < 3; i++){
  //    vet[i] = (int*) malloc(10 * sizeof(int));
  //}

  omp_set_num_threads(2);
  #pragma omp parallel shared(i) private(j) firstprivate(M1, M2, M3) reduction(+: dif_mat)
  {
    //int ID = omp_get_thread_num();
    #pragma omp for
    for(int i = 0; i < l; i++){
      for(int j = 0; j < c; j++){
        M1[i][j] = pow(M1[i][j], 2);

        M2[i][j] = pow(M2[i][j], 2);

        M3[i][j] = (M1[i][j] - M2[i][j]);
        //printf("M1[i][j] %lf \n", M1[i][j]);
        //printf("M2[i][j] %lf \n", M2[i][j]);
        //printf("M3[i][j] %lf \n", M3[i][j]);
        dif_mat += M3[i][j];
      }
      //printf("\n");
    }
  }


  return dif_mat;

}

// compilar: gcc -fopenmp pratica_05.c -lm -o pratica_05
// executar: ./pratica_05 3 3 s

int main(int argc, char *argv[]){

   srand( (unsigned)time(NULL) );
   struct timespec startWALL, endWALL, StartTransposta, EndTransposta, StartMultiTransposta, EndMultiTransposta;
   int linhas = atoi(argv[1]);
   int colunas = atoi(argv[2]);
   char tipo = *argv[3];
   printf("linhas: %d \n", linhas);
   printf("colunas: %d \n", colunas);
   printf("tipo: %c \n", tipo);


   if(*argv[3] == 's'){
     double **MatA =  alocaMatriz(linhas,colunas);
     geraMatriz(MatA,linhas,colunas);
     //PrintMatriz(MatA,linhas,colunas);

     double **MatB =  alocaMatriz(linhas,colunas);
     geraMatriz(MatB,linhas,colunas);
     //PrintMatriz(MatB,linhas,colunas);


     double **MatC =  alocaMatriz(linhas,colunas);
     clock_gettime(CLOCK_MONOTONIC,&startWALL);
     double resul_dif_mat = op_matriz_seq(MatA, MatB, MatC, linhas, colunas);
     clock_gettime(CLOCK_MONOTONIC,&endWALL);

     printf("resul_dif_mat %lf \n", resul_dif_mat);
     double timeWALL = timespecToSec(&endWALL) - timespecToSec(&startWALL);
     printf("Tempo execuçaõ sequecial = %lf segundos\n", timeWALL);

     free(MatA);
     free(MatB);

   }

   if(*argv[3] == 't'){

     double **MatA =  alocaMatriz(linhas,colunas);
     geraMatriz(MatA,linhas,colunas);
     //PrintMatriz(MatA,linhas,colunas);

     double **MatB =  alocaMatriz(linhas,colunas);
     geraMatriz(MatB,linhas,colunas);
     //PrintMatriz(MatB,linhas,colunas);


     double **MatC =  alocaMatriz(linhas,colunas);
     clock_gettime(CLOCK_MONOTONIC,&startWALL);
     double resul_dif_mat = op_matriz_thread(MatA, MatB, MatC, linhas, colunas);
     clock_gettime(CLOCK_MONOTONIC,&endWALL);

     printf("resul_dif_mat %lf \n", resul_dif_mat);
     double timeWALL = timespecToSec(&endWALL) - timespecToSec(&startWALL);
     printf("Tempo execuçaõ sequecial = %lf segundos\n", timeWALL);

     free(MatA);
     free(MatB);

   }



}
