/* COP 3502C Programming Assignment 4
This program is written by: Kevin Rodriguez */
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#define X_MAX  10000
#define Y_MAX  10000
#define N_MAX  100000
#define S_MAX  100000
#define T_MAX  30

int myX, myY;

typedef struct coordinates {
  int x;
  int y;
  double distance;
}coordinates;

void computeDistances(coordinates arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i].distance = sqrt(pow(arr[i].x - myX, 2) + pow(arr[i].y - myY, 2));
    }
}

coordinates* ReadData(int n) {
  coordinates* c = (coordinates*)malloc(n * sizeof(coordinates));

  for (int i = 0; i < n; i++) {
    scanf("%d %d", &c[i].x, &c[i].y);
  }
  return c;
}

/*
needs to return a negative int if the point pointed by ptrPt1 is closer to you than the point pointed to by ptrPt2.

a 0 if the 2 locations pointed to by both are identical.

a positive int if the point pointed by the ptrPt1 is farther from you than the point pointed to by ptrPt2.

Exceptions to this will be when two pointers are pointing to points that are the same distance from you but are distinct point. 

In these cases if ptrPt1's x coordinate is lower than ptrPt2's x coordinate, a negative int must be returned. 

Alternatively, if ptrPt1's x coord is greater than ptrPt2's x coord a positive int must be returned.

Finally if the x coord of both is the same, the same prev rules apply for both y coords.

Furthermore, a negative int means ptrPt1 is going to be placed first in the sort, a positive int means ptrPt2 is going to be placed first and
0 means they are the same meaning no change is done.

*/
int compareTo(coordinates *ptrPt1, coordinates *ptrPt2) {
    if (ptrPt1->distance < ptrPt2->distance) {
        return -1;
    } else if (ptrPt1->distance > ptrPt2->distance) {
        return 1;
    } else {
        // Handle ties with x and y comparison
        if (ptrPt1->x < ptrPt2->x) {
            return -1;
        } else if (ptrPt1->x > ptrPt2->x) {
            return 1;
        } else {
            return (ptrPt1->y < ptrPt2->y) ? -1 : (ptrPt1->y > ptrPt2->y) ? 1 : 0;
        }
    }
}

void insertionSort(coordinates* arr, int l, int r) {
  int i, j;
  coordinates item;
  for (i = l + 1; i <= r; i++) {
    item = arr[i];
    for(j=i-1; j>=l; j--) {
      if(compareTo(&arr[j], &item) == 1) {
        arr[j+1] = arr[j];
      }
      else {
        break;
      }
    }
    arr[j+1] = item;
  }
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(coordinates arr[], int l, int m, int r)
{
  int i, j, k;

  int n1 = m - l + 1;
  int n2 = r - m;

  coordinates *L = malloc(sizeof(coordinates) * n1);
  coordinates *R = malloc(sizeof(coordinates) * n2);

  for (i = 0; i < n1; i++) {
    L[i] = arr[l+i];
  }
  for (j = 0; j < n2; j++) {
    R[j] = arr[m+1+j];
  }
  i = 0;
  j = 0;
  k = l;
  while (i < n1 && j < n2) {
      int cmp = compareTo(&L[i], &R[j]);
    if (cmp == -1 || cmp == 0) {
      arr[k] = L[i];
      i++;
      k++;
    }
    else if (cmp == 1) {
      arr[k] = R[j];
      j++;
      k++;
    } 
  }
  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }

  free(L);
  free(R);
}
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(coordinates arr[], int l, int r, int t)
{
  if (r - l + 1 <= t) {
    insertionSort(arr, l, r);
    return;
  }

  if (r > l) {
    int m = (l + r) / 2;

    mergeSort(arr, l, m, t);
    mergeSort(arr, m + 1, r, t);

    merge(arr, l, m, r);
  }
}
/* UTILITY FUNCTIONS */
/* Function to print an array */
void printArray(coordinates A[], int size) {

  int i;
  for (i=0; i < size; i++) {
    printf("%d %d", A[i].x, A[i].y);
    printf("\n");
  }
}

int binarySearch(coordinates arr[], int n, coordinates* target) {
    int left = 0, right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        int cmp = compareTo(&arr[mid], target);
        if (cmp == 0) return mid;
        else if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1; // Target not found
}

void wrapperSort(coordinates arr[], int n, int t) {
    mergeSort(arr, 0, n - 1, t);
}


int main() {
  int numGarages, searchNum, threshold;
  coordinates* garages;
  scanf("%d%d%d%d%d", &myX, &myY, &numGarages, &searchNum, &threshold);

  garages = ReadData(numGarages);
  coordinates* searchPoints = ReadData(searchNum);
  computeDistances(garages, numGarages);

  wrapperSort(garages, numGarages, threshold);
  printArray(garages, numGarages);

  computeDistances(searchPoints, searchNum);

  for (int i = 0; i < searchNum; i++) {
    int index = binarySearch(garages, numGarages, &searchPoints[i]);
    if (index == -1) {
      printf("%d %d no garage found\n", searchPoints[i].x, searchPoints[i].y);
    }
    else {
      printf("%d %d garage found at position %d in the order\n", searchPoints[i].x, searchPoints[i].y, index + 1);
    }
  }

  free(garages);
  free(searchPoints);

  return 0;
}
