#include <stdio.h>

#define NUM_BLOCKS 128
#define NUM_THREADS_PER_BLOCK 256
#define SIZE NUM_BLOCKS*NUM_THREADS_PER_BLOCK

cudaEvent_t start, stop;

void checkCUDAError(const char *msg) {
	cudaError_t err = cudaGetLastError();
	if(cudaSuccess != err) {
		fprintf(stderr, "Cuda error: %s, %s\n", msg, cudaGetErrorString(err));
		exit(-1);
	}
}

void printMat(unsigned size, float* mat){
    for(unsigned i=0; i<size; i++){
        printf("[");
        for(unsigned j=0; j<size-1; j++){
            printf("%.2f,\t", mat[i*size +j]);
        }
        printf("%.2f]\n", mat[i*size + size-1]);
    }
}


__global__
void vecAdditionKernel (int size, float *A, float *B, float *R) {
    int bi = threadIdx.x;
    int bj = threadIdx.y;
    int i  = blockIdx.x * blockDim.x + bi;
    int j  = blockIdx.y * blockDim.y + bj;

    __shared__ float sharedA[TILE_WIDTH][TILE_WIDTH];
    __shared__ float sharedB[TILE_WIDTH][TILE_WIDTH];

    float tmp = 0.0;
    for(unsigned t=0; t<size/TILE_WIDTH; t++){
        sharedA[bi][bj] = A[i*size + t*TILE_WIDTH + bj];
        sharedB[bi][bj] = B[(t*TILE_WIDTH+bi)*size + j];

        __syncthreads();//----------------------------------------

        for(unsigned k=0; k<TILE_WIDTH; k++) tmp += sharedA[bi][k] * sharedB[k][bj];

        __syncthreads();//----------------------------------------
    }
    R[i*size + j] = tmp;
}


int main( int argc, char** argv) {
    if(argc != 2){
        fprintf(stderr, "usage: ./dotProd size\n");
    }

    const unsigned size = atoi(argv[1]);
	const int bytes = size*size*sizeof(float);

    float** mat = (float***)malloc(3 * sizeof(float*));
    for(unsigned k=0; k<3; k++){
        mat[k] = (float*)malloc(size*size * sizeof(float));
    }
    float* A = mat[0];
    float* B = mat[1];
    float* R = mat[2];

    srand(1);
    for(unsigned i=0; i<size; i++){
        for(unsigned j=0; j<size; j++){
            float randf = (float)rand()/RAND_MAX*2.0 - 1.0; 
            A[i*size + j] = randf * 10.0;
            B[i*size + j] = 1.0;
            R[i*size + j] = 0.0;
        }
    }

	float *dA, *dB, *dR;
	cudaMalloc((void**) &dA, bytes);
	cudaMalloc((void**) &dB, bytes);
	cudaMalloc((void**) &dR, bytes);
	checkCUDAError("mem allocation");

    cudaEvent_t start_kernel, end_kernel, start_send, end_send, start_recv, end_recv;
    cudaEventCreate(&start_kernel   );
    cudaEventCreate(&end_kernel     );
    cudaEventCreate(&start_send     );
    cudaEventCreate(&end_send       );
    cudaEventCreate(&start_recv     );
    cudaEventCreate(&end_recv       );

	cudaEventRecord(start_send);//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	cudaMemcpy(da, a, bytes, cudaMemcpyHostToDevice);
	cudaMemcpy(db, b, bytes, cudaMemcpyHostToDevice);
	checkCUDAError("memcpy h->d");
	cudaEventRecord(end_send);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cudaEventSynchronize(end_send);

	cudaEventRecord(start_kernel);//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	dotProdKernel <<< NUM_THREADS_PER_BLOCK, NUM_BLOCKS >>>(size, da, db, dc);
	checkCUDAError("kernel invocation");
	cudaEventRecord(end_kernel);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cudaEventSynchronize(end_kernel);

	cudaEventRecord(start_recv);//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	cudaMemcpy(c, dc, bytes, cudaMemcpyDeviceToHost);
	checkCUDAError("memcpy d->h");
	cudaEventRecord(end_recv);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	cudaEventSynchronize(end_recv);


	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start_send, end_send);
    printf("memcpy h->d time: %f", milliseconds);
	cudaEventElapsedTime(&milliseconds, start_recv, end_recv);
    printf("memcpy d->h time: %f", milliseconds);
	cudaEventElapsedTime(&milliseconds, start_kernel, end_kernel);
    printf("kernel time: %f", milliseconds);




	// print matrix
#ifdef DEBUG
    //printf("A:\n"); printMat(size, A);
    //printf("B:\n"); printMat(size, B);
    printf("A*B:\n"); printMat(size, R);
    for(unsigned i=0; i<size; i++)
        for(unsigned j=0; j<size; j++)
            R[i][j] = 0.0;
    dotProd(size, B, A, R);
    printf("B*A:\n"); printMat(size, R);
#endif

	cudaFree(da); cudaFree(db); cudaFree(dc);
	checkCUDAError("mem free");
    free(A); free(B); free(R); free(mat);
    return(0);
}
