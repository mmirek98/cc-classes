#include <stdio.h>
#include <omp.h>

int main() {
	int i;
	int a = 7;

	// #pragma omp parallel for private(a) num_threads(7)
	// #pragma omp parallel for firstprivate(a) num_threads(7)
	#pragma omp parallel for shared(a) num_threads(7)
	for (i = 0; i < 10; i++) {
		printf("Thread %d a=%d\n", omp_get_thread_num(), a);
		a++;
	}

	return 0;
}
