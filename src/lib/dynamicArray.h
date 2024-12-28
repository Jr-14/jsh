#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GROW_CAPACITY 2

typedef struct {
  void *array;
  size_t capacity;
  size_t size;
  size_t elementSize;
} DynamicArray;

// Initialise an Dynamic Array
void initArray(DynamicArray *a, size_t initialSize, size_t elementSize);

// Free the array
void freeArray(DynamicArray *a);

// Grow the dynamic array
void grow(DynamicArray *a);

// Insert an element into the Dynamic Array
void insert(DynamicArray *a, void *element);

void *getFromArray(DynamicArray *a, int index);

#endif

