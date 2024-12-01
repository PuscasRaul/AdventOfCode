/* Using the solution for the first puzzle, i just went through the first array,
 * selected the element, and found how many times it's located in the second
 * array Could've used a binary search, but I didn't for this one.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct dynamic_array dynamic_array;

typedef void (*array_resize)(dynamic_array *array);

void swap(int32_t *a, int32_t *b);

struct dynamic_array {
    array_resize resize_func;
    uint32_t length;
    uint32_t capacity;
    int32_t *data;
    int32_t first_free;
};

void resize(dynamic_array *array) {

    array->capacity *= 2;
    int *new_data = reallocarray(array->data, array->capacity, sizeof(int32_t));

    if (new_data == NULL) {
        free(array->data);
        perror("reallocarray failure");
        return;
    }

    array->data = new_data;
}

dynamic_array *array_create(array_resize resize_func) {

    dynamic_array *return_array =
        (dynamic_array *) malloc(sizeof(dynamic_array));
    if (return_array == NULL) {
        perror("malloc failure");
        return NULL;
    }

    return_array->resize_func = resize_func;
    return_array->capacity = 1;
    return_array->length = 0;
    return_array->first_free = 0;
    return_array->data = calloc(return_array->capacity, sizeof(int32_t));

    if (return_array->data == NULL) {
        perror("calloc failure");
        return NULL;
    }

    for (int i = return_array->length; i < return_array->capacity; i++)
        return_array->data[i] = -1;

    return return_array;
}

void array_insert(int value, dynamic_array *array) {

    if (array->length == array->capacity) {
        array->resize_func(array);
        if (array->data == NULL) {
            perror("resize failure");
            exit(EXIT_FAILURE);
        }
    }

    array->data[array->first_free] = value;

    if (array->first_free == array->length) {
        ++array->length;
        array->first_free = array->length;
        return;
    }

    for (int i = array->first_free; i < array->capacity; i++) {
        if (array->data[i] != -1)
            continue;

        array->first_free = i;
        break;
    }

    ++array->length;
}

int32_t array_extract(dynamic_array *array, uint32_t position) {

    if (position >= array->length)
        return -1;

    if (array->first_free > position)
        array->first_free = position;

    int32_t value = array->data[position];
    array->data[position] = -1;
    --array->length;
    return value;
}

uint32_t array_find(dynamic_array *array, int32_t value) {

    for (uint32_t i = 0; i < array->length; i++)
        if (array->data[i] == value)
            return i;

    return array->length + 1;
}

int partition(dynamic_array *array, uint32_t low, uint32_t high) {
    uint32_t pivot = array->data[high];

    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (array->data[j] < pivot) {
            ++i;
            swap(&array->data[i], &array->data[j]);
        }
    }

    swap(&array->data[i + 1], &array->data[high]);
    return i + 1;
}

void QuickSort(dynamic_array *array, uint32_t low, uint32_t high) {
    if (low < high) {
        int pi = partition(array, low, high);
        QuickSort(array, low, pi - 1);
        QuickSort(array, pi + 1, high);
    }
}

void swap(int32_t *a, int32_t *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int find_all_sorted(dynamic_array *array, int32_t value) {

    int count = 0;
    int index = array_find(array, value);

    for (index; index < array->length; index++) {
        if (array->data[index] > value)
            break;
        ++count;
    }

    return count;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("Please provide the file which contains the input.");
        exit(EXIT_FAILURE);
    }

    dynamic_array *array_1;

    if ((array_1 = array_create(resize)) == NULL) {
        perror("error array_1");
        exit(EXIT_FAILURE);
    }

    dynamic_array *array_2;
    if ((array_2 = array_create(resize)) == NULL) {
        perror("error array_2");
        exit(EXIT_FAILURE);
    }

    int numar1, numar2;

    FILE *input_file;
    if ((input_file = fopen(argv[1], "r")) == NULL) {
        perror("fopen error");
        exit(EXIT_FAILURE);
    }
    int k = 1;
    while (fscanf(input_file, "%d", &numar1) > 0) {

        array_insert(numar1, array_1);

        if (fscanf(input_file, "%d", &numar2) < 0) {
            perror("hey hey hey");
            exit(EXIT_FAILURE);
        }

        array_insert(numar2, array_2);
        ++k;
    }

    // fprintf(stdout, "%d", array_1->length);
    QuickSort(array_1, 0, array_1->length - 1);
    QuickSort(array_2, 0, array_2->length - 1);

    int sum = 0;

    for (int i = 0; i < array_1->length; i++) {
        sum += array_1->data[i] * find_all_sorted(array_2, array_1->data[i]);
    }

    fprintf(stdout, "Similarity : %d\n", sum);

    exit(EXIT_SUCCESS);
}
