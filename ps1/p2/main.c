/*
 * tab:2
 *
 * main.c - I/O and BST problem setup
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Aamir Hasan
 * Version:       1
 * Creation Date: Fri Aug 30 2020
 * Filename:      main.c
 * History:
 *    AS    1    Fri Aug 30 2020
 *        First written.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "binarysearch.h"

int32_t* import_arr(char* fname, uint32_t* find, uint32_t* num) {
  FILE* f = fopen(fname, "r");
  uint32_t i, key, num_elements;


  fscanf(f, "%d %d\n", &num_elements, find);
  int32_t* arr = malloc(sizeof(int32_t) * num_elements);
  for (i = 0; i < num_elements; i++) {
    fscanf(f, "%d\n", &key);
    arr[i] = key;
  }
  *num = num_elements;
  fclose(f);
  return arr;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// int partition(int arr[], int low, int high) {
//     int pivot = arr[high];
//     int i = low - 1;
//     for (int j = low; j < high; j++) {
//         if (arr[j] < pivot) {
//             i++;
//             swap(arr + i, arr + j);
//         }
//     }
//     swap(arr[i + 1], arr[high]);
//     return i + 1;
// }

// void quickSort(int arr[], int low, int high) {
//     if (low < high) {
//         int pi = partition(arr, low, high);
//         quickSort(arr, low, pi - 1);
//         quickSort(arr, pi + 1, high);
//     }
// }

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
       printf("%d\n",arr[i]);
    }
}
void selectionSort(int* arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            swap(arr + i, arr + minIndex);
        }
    }
}
// int main() {
//     int arr[] = {12, 11, 13, 5, 6, 7};
//     int n = num_elements;
//     std::cout << "Original array: \n";
//     printArray(arr, n);
//     quickSort(arr, 0, n - 1);
//     std::cout << "Sorted array: \n";
//     printArray(arr, n);
//     return 0;
// }

int main(int argc, char** argv) {
  if(argc < 2) {
    printf("Usage: ./maze <input_maze>\n");
    return -1;
  }
  int32_t* arr = NULL;
  uint32_t find = 0;
  uint32_t num_elements = 0;
  
  arr = import_arr(argv[1], &find, &num_elements);
  //printf("Reading Tree:\n");
  //print(root);
  selectionSort(arr, num_elements);
  //printf("\nBeginning C Search\n");
  printf("search find %d: %d\n", find, binarySearch(arr, find, 0, num_elements));
  printf("search find %d: %d\n", find, binarySearch(arr, find, 0, num_elements));
  //printf("Beginning ASM Search\n");
  // printf("search find %d: %d\n", find, search_asm(root, find));
  return 0;
}
