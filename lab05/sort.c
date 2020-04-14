#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <linux/sched.h>
#include <time.h>

#define LEN 200

struct Borders {
	int start;
	int end;
};

// int array[] = {4, 2, 3, 1, 5};
int array[LEN];


void prepareArray() {
	srand(time(0));
	int i;
	for (i = 0; i < LEN; i++) {
		array[i] = rand()%1000;
	}
}

// does actual sorting
void *mergeConc(void* bord) {
	struct Borders borders = *((struct Borders *) bord);
	int center = (borders.start + borders.end) / 2;
	int leftIt = borders.start;
	int rightIt = center + 1;
	int currentPos = 0;
	int tmpTab[LEN];

	while (leftIt <= center && rightIt <= borders.end) {
		if (array[leftIt] < array[rightIt]) {
			tmpTab[currentPos] = array[leftIt];
			leftIt++;
			currentPos++;
		} else {
			tmpTab[currentPos] = array[rightIt];
			rightIt++;
			currentPos++;
		}

	}

	if (leftIt <= center) {
		while (leftIt <= center) {
			tmpTab[currentPos] = array[leftIt];
			leftIt++; currentPos++;
		}
	} else if (rightIt <= borders.end) {
		while (rightIt <= borders.end) {
			tmpTab[currentPos] = array[rightIt];
			rightIt++; currentPos++;
		}
	}

	currentPos = 0;
	int i;
	for (i = borders.start; i <= borders.end; i++) {
		array[i] = tmpTab[currentPos];
		currentPos++;
	}
}

// doesn't care about array, only divide 
void *mergeSortConc(void* bord) {
	struct Borders borders = *((struct Borders *) bord);
	struct Borders left, right;
	pthread_t leftTid;
	pthread_t rightTid;

	left.start = borders.start;
	left.end = (borders.start + borders.end) / 2;
	right.start = left.end + 1;
	right.end = borders.end;

	if (borders.start < borders.end) {
		pthread_create(&leftTid, NULL, mergeSortConc, &left);
		pthread_create(&rightTid, NULL, mergeSortConc, &right);

		pthread_join(leftTid, NULL);
		pthread_join(rightTid, NULL);

		mergeConc(&borders);
	}
}

// does actual sorting
void merge(struct Borders *borders) {
	int center = (borders->start + borders->end) / 2;
	int leftIt = borders->start;
	int rightIt = center + 1;
	int currentPos = 0;
	int tmpTab[LEN];

	while (leftIt <= center && rightIt <= borders->end) {
		if (array[leftIt] < array[rightIt]) {
			tmpTab[currentPos] = array[leftIt];
			leftIt++;
			currentPos++;
		} else {
			tmpTab[currentPos] = array[rightIt];
			rightIt++;
			currentPos++;
		}

	}

	if (leftIt <= center) {
		while (leftIt <= center) {
			tmpTab[currentPos] = array[leftIt];
			leftIt++; currentPos++;
		}
	} else if (rightIt <= borders->end) {
		while (rightIt <= borders->end) {
			tmpTab[currentPos] = array[rightIt];
			rightIt++; currentPos++;
		}
	}

	currentPos = 0;
	int i;
	for (i = borders->start; i <= borders->end; i++) {
		array[i] = tmpTab[currentPos];
		currentPos++;
	}
}

// doesn't care about array, only divide 
void mergeSort(struct Borders *borders) {
	struct Borders left, right;
	left.start = borders->start;
	left.end = (borders->start + borders->end) / 2;
	right.start = left.end + 1;
	right.end = borders->end;

	if (borders->start < borders->end) {
		mergeSort(&left);
		mergeSort(&right);
		merge(borders);
	}
}


int main() {
	struct Borders borders;
	borders.start = 0;
	borders.end = (sizeof array / sizeof *array) - 1;
	prepareArray();

	int j = 0;
	for (j = 1; j <= LEN; j++) {
		printf("%d \t", array[j-1]);
		if (j % 10 == 0) printf("\n");
	}
	printf("\n\n\n");

	clock_t start = clock();
	mergeSort(&borders);
	clock_t end = clock();
	int i;
	for (i = 1; i <= LEN; i++) {
		printf("%d \t", array[i-1]);
		if (i % 10 == 0) printf("\n");
	}
	printf("\n\n");
	printf("Czas wykonywania dla asynchronicznego: %lu ms\n", end-start);

	//  CONCURRENT
	prepareArray();
	for (j = 1; j <= LEN; j++) {
		printf("%d \t", array[j-1]);
		if (j % 10 == 0) printf("\n");
	}
	printf("\n\n\n");

	clock_t start2 = clock();
	mergeSortConc(&borders);
	clock_t end2 = clock();

	for (i = 1; i <= LEN; i++) {
		printf("%d \t", array[i-1]);
		if (i % 10 == 0) printf("\n");
	}
	printf("\n\n");
	printf("Czas wykonywania dla asynchronicznego: %lu ms\n", end2-start);
}
