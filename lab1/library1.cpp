#include <iostream>

using namespace std;

void printAllocs() {
    int n;
    float *arr1, *arr2;

    cout << "Podaj ilosc elementow n: " << endl;
    cin >> n;

    arr1 = (float *) malloc(n * sizeof(float));
    arr2 = (float *) calloc(n, sizeof(float));

    for (int i = 0; i < n; i++) {
        cout << arr1[i] << endl;
    }

    cout << endl;

    for (int i = 0; i < n; i++) {
        cout << arr2[i] << endl;
    }

    free(arr1);
    free(arr2);
}