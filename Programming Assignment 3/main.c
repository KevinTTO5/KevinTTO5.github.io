#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h> 

#define MAX_PAIRS 8
#define MAX_NAME 20

// Structure to represent a garage
typedef struct {
    float x, y;
    char name[MAX_NAME];
} Garage;

// Global Variables 
Garage garages[MAX_PAIRS * 2];  // Global array of garages
int finalPermutation[MAX_PAIRS * 2];  // Global array to store the final permutation of garage indices

// Function to calculate the distance between two garages
double distanceCalc(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// Recursive function to find the minimum sum of distances
void findMinSum(int used[], int pairs[], int k, int n, double currentSum, double *minTotalDistance) {
    // Base case: If all pairs have been formed
    if (k == 2 * n) {
        // If the current sum is smaller than the minimum, update the global variables
        if (currentSum < *minTotalDistance) {
            *minTotalDistance = currentSum;

            // Manually copy the current pairs into the finalPermutation array
            for (int i = 0; i < 2 * n; i++) {
                finalPermutation[i] = pairs[i];
            }
        }
        return;
    }

    // Find the first unused garage
    int firstUnused = -1;
    for (int i = 0; i < 2 * n; i++) {
        if (!used[i]) {
            firstUnused = i;
            break;
        }
    }

    // Try pairing the first unused garage with all other unused garages
    for (int j = firstUnused + 1; j < 2 * n; j++) {
        if (!used[j]) {
            // Mark both garages as used
            used[firstUnused] = used[j] = 1;
            pairs[k] = firstUnused;
            pairs[k + 1] = j;

            // Calculate the distance for this pair
            double dist = distanceCalc(garages[firstUnused].x, garages[firstUnused].y, garages[j].x, garages[j].y);

            // Recurse with the new pair and add the distance to the current sum
            findMinSum(used, pairs, k + 2, n, currentSum + dist, minTotalDistance);

            // Unmark the garages as unused
            used[firstUnused] = used[j] = 0;
        }
    }
}

int main() {
    int n;

    // Read the number of pairs
    scanf("%d", &n);

    // Read the garage data
    for (int i = 0; i < 2 * n; i++) {
        scanf("%f %f %s", &garages[i].x, &garages[i].y, garages[i].name);
    }

    // Arrays to track used garages and pairs
    int used[2 * n];  // To mark garages as used (1) or unused (0)
    int pairs[2 * n];  // To store the current permutation of pairs

    // Initialize the used array to 0 (meaning all garages are unused initially)
    for (int i = 0; i < 2 * n; i++) {
        used[i] = 0;
    }

    // Variable to store the minimum distance, passed by reference
    double minTotalDistance = DBL_MAX;

    // Start the recursive function to find the minimum sum of distances
    findMinSum(used, pairs, 0, n, 0.0, &minTotalDistance);

    // Output the minimum distance
    printf("%.3f\n", minTotalDistance);

    // Output the pairs that resulted in the minimum distance
    for (int i = 0; i < 2 * n; i += 2) {
        int from = finalPermutation[i];
        int to = finalPermutation[i + 1];
        double dist = distanceCalc(garages[from].x, garages[from].y, garages[to].x, garages[to].y);
        printf("(%s, %s, %.3f)\n", garages[from].name, garages[to].name, dist);
    }

    return 0;
}
