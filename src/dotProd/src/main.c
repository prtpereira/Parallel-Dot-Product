#include <stdlib.h>
#include <sys/time.h>

#include "stdio.h"

//#ifdef PHI
//#include <offload.h>
//#endif

#ifdef D_PAPI
#include "papi.h"
#define NUM_EVENTS 2
long long values[NUM_EVENTS];
int Events[NUM_EVENTS]={PAPI_L1_DCM};
int EventSet = PAPI_NULL;
int retval;
#else
double tstart;
double tstop;
#endif


double dtime(){
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime,(struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec*1.0e-6);
    return(tseconds);
}
void printMat(unsigned size, float** mat){
    for(unsigned i=0; i<size; i++){
        printf("[");
        for(unsigned j=0; j<size-1; j++){
            printf("%.2f,\t", mat[i][j]);
        }
        printf("%.2f]\n", mat[i][size-1]);
    }
}
void transposeMat(unsigned size, float** mat){
    for(unsigned i=0; i<size; i++){
        for(unsigned j=i+1; j<size; j++){
            float tmp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = tmp;
        }
    }
}

void start(){
#ifdef D_PAPI
	if( (retval = PAPI_start(EventSet)) < PAPI_OK ){ fprintf(stderr, "erro(%d) start counters\n",  retval); exit(-1);}
#else
    tstart = dtime();
#endif
}

void stop(){
#ifdef D_PAPI
    if( PAPI_stop(EventSet,values) < PAPI_OK){ fprintf(stderr, "erro(%d) stop counters\n",   retval); exit(-1);}
	printf("%lld\n", values[0]);
#else
    tstop = dtime();
    printf("%lf\n", tstop-tstart);
#endif
}


void dotProd(const unsigned size, float** A, float** B, float** R){
/*
 * PHI
 */
#ifdef PHI
    #ifdef DEBUG
    fprintf(stderr, "PHI:\n");
    #endif
    start(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    #pragma offload target(mic) \
        in(A:length(size*size)) \
        in(B:length(size*size)) \
        out(R:length(size*size))

    #pragma omp parallel for default(none) shared(A,B,R,size)
    for (int i = 0; i < size; ++i){
    	for (int k = 0; k < size; ++k){
    		for (int j = 0; j < size; ++j){
    			R[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/*
 * BLOCK PAR IJK
 */
#elif defined PAR
    #ifdef DEBUG
    fprintf(stderr, "PAR:\n");
    #endif
    transposeMat(size, B);
    start(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int bsize=25;
    int tbsize = bsize * (size/bsize);
        
    for(unsigned bk=0; bk<tbsize; bk+=bsize){ 
        for(unsigned bj=0; bj<tbsize; bj+=bsize){
#pragma omp parallel for schedule(static, bsize)
            for(unsigned i=0; i<size; i++){
                for(unsigned j=bj; j<bj+bsize; j++){
                    double tmp = R[i][j];
//#pragma vector always
#pragma omp simd reduction(+:tmp)
                    for(unsigned k=bk; k<bk+bsize; k++){
                        tmp += A[i][k] * B[j][k];
                    }
                    R[i][j]=tmp;
                }
            }
        }
    }
#pragma omp parallel for
    for(unsigned i=0; i<size; i++){
        for(unsigned j=tbsize; j<size; j++){
            double tmp = 0;
//#pragma vector aligned
#pragma omp simd reduction(+:tmp)
            for(unsigned k=0; k<size; k++){
                tmp += A[i][k] * B[j][k];
            }
            R[i][j]=tmp;
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

/*
 * BLOCK PAR IJK
 */
#elif defined BLOCK
    #ifdef DEBUG
    fprintf(stderr, "BLOCK:\n");
    #endif
    transposeMat(size, B);
    start(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int bsize=25;
    int tbsize = bsize * (size/bsize);
        
    for(unsigned bk=0; bk<tbsize; bk+=bsize){ 
        for(unsigned bj=0; bj<tbsize; bj+=bsize){
            for(unsigned i=0; i<size; i++){
                for(unsigned j=bj; j<bj+bsize; j++){
                    double tmp = R[i][j];
#pragma vector always
                    for(unsigned k=bk; k<bk+bsize; k++){
                        tmp += A[i][k] * B[j][k];
                    }
                    R[i][j]=tmp;
                }
            }
        }
    }
    for(unsigned i=0; i<size; i++){
        for(unsigned j=tbsize; j<size; j++){
            double tmp = 0;
#pragma vector always
            for(unsigned k=0; k<size; k++){
                tmp += A[i][k] * B[j][k];
            }
            R[i][j]=tmp;
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/*
 * IKJ
 */
#elif defined IKJ
    #ifdef DEBUG
        fprintf(stderr, "IKJ:\n");
    #endif
    start(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    for(unsigned i=0; i<size; i++){
        for(unsigned k=0; k<size; k++){
            float tmp = A[i][k];
            for(unsigned j=0; j<size; j++){
                R[i][j] += tmp * B[k][j];
            }
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/*
 * JKI
 */
#elif defined JKI
    #ifdef DEBUG
        fprintf(stderr, "JKI:\n");
    #endif
    transposeMat(size, A);
    transposeMat(size, R);
    start(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    for(unsigned j=0; j<size; j++){
        for(unsigned k=0; k<size; k++){
            float tmp = B[k][j];
            for(unsigned i=0; i<size; i++){
                R[j][i] += A[k][i] * tmp;
            }
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    transposeMat(size, R);
    transposeMat(size, A);
/*
 * IJK
 */
#else 
    #ifdef DEBUG
        fprintf(stderr, "IJK:\n");
    #endif
    transposeMat(size, B);
    start(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<size; j++){
            float tmp = 0.0;
            for(unsigned k=0; k<size; k++){
                tmp += A[i][k] * B[j][k];
            }
            R[i][j] = tmp;
        }
    }
    stop(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    transposeMat(size, B);
#endif
}



int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "usage: ./dotProd size\n");
    }

    const unsigned size = atoi(argv[1]);
#ifdef D_PAPI
	if( (retval = PAPI_library_init(PAPI_VER_CURRENT)) < PAPI_OK )          { fprintf(stderr, "erro(%d) init papi\n",       retval); exit(-1);}
	if( (retval = PAPI_create_eventset(&EventSet)) < PAPI_OK )              { fprintf(stderr, "erro(%d) create event set\n",retval); exit(-1);}
	if( (retval = PAPI_add_events(EventSet,Events,NUM_EVENTS)) < PAPI_OK )  { fprintf(stderr, "erro(%d) add events\n",      retval); exit(-1);}
#endif

    float*** mat = (float***)malloc(3 * sizeof(float**));
    for(unsigned k=0; k<3; k++){
        //mat[k] =    (float**)malloc(size * sizeof(float*));
        //mat[k][0] = (float*)malloc(size*size * sizeof(float));
        mat[k] = (float**)_mm_malloc(size * sizeof(float*), 32);
        mat[k][0] = (float*)_mm_malloc(size*size * sizeof(float), 32);
        float* offset = mat[k][0];
        for(unsigned i=0; i<size; i++) mat[k][i] = offset + i*size;
    }
    
    __attribute__((aligned(32))) float** A = mat[0];
    __attribute__((aligned(32))) float** B = mat[1];
    __attribute__((aligned(32))) float** R = mat[2];

    srand(1);

    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<size; j++){
            float randf = (float)rand()/RAND_MAX*2.0 - 1.0; 
            A[i][j] = randf * 10.0;
            B[i][j] = 1.0;
            R[i][j] = 0.0;
        }
    }

    dotProd(size, A, B, R);


#ifdef DEBUG
    printf("A*B:\n"); printMat(size, R);
    for(unsigned i=0; i<size; i++) for(unsigned j=0; j<size; j++) R[i][j] = 0.0;
    dotProd(size, B, A, R); printf("B*A:\n"); printMat(size, R);
#endif

	//free(mat[0]);
	//free(mat);
	_mm_free(mat[0]);
	_mm_free(mat);
    return(0);
}
