#include "dynamicArray.h"
#include <stdio.h>

void initArray(DynamicArray *a, size_t initialCapacity, size_t elementSize) {
  a->array = calloc(initialCapacity, elementSize);
  a->capacity = initialCapacity;
  a->size = 0;
  a->elementSize = elementSize;
}

void freeArray(DynamicArray *a) {
  free(a->array);
  a->array = NULL;
  a->capacity = 0;
  a->size = 0;
}

void grow(DynamicArray *a) {
  a->capacity *= GROW_CAPACITY;
  a->array = realloc(a->array, a->elementSize * a->capacity);
}

void insert(DynamicArray *a, void *element) {
  if (a->size == a->capacity) {
    grow(a);
  }
  memcpy((char*)a->array + (a->size * a->elementSize), element, a->elementSize);
  a->size++;
}

void *getFromArray(DynamicArray *a, int index) {
  if ((unsigned long)index >= a->size) {
    fprintf(stderr, "Index out of bounds. Size is: %lu, but got index: %d", a->size, index);
    exit(1);
  }
  return (char*)a->array + index * a->elementSize;
}

