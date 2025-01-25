#include <stdio.h>
#include <time.h>

#define VECTOR_IMPL
#include "vector.h"

int main()
{
    vector vec = vector_new(sizeof(int));

    srand(time(NULL));

    for (int i = 0; i < 100; i++)
        vector_push_back(vec, (void*)(rand() % 10));

    printf("Capacity: %i, size: %i, last index: %i\n", vector_capacity(vec), vector_size(vec), vec->_last_index);
    for (int i = 0; i < vector_size(vec); i++)
        printf("%d ", VECTOR_GET(vec, i, int));

    vector_free(vec);

    return 0;
}
