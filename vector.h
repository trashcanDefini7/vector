#ifndef VECTOR_H
#define VECTOR_H

/* 
*   How data is stored?
*       We simply store everything as bytes (unsigned char*) and then
*       using standard C functions we move bytes around.
*   Functions:
*       void vector_set(vector, int, void*) - sets the value at specific index,
*           throws an exception when index is out of bounds of the vector
*       byte* vector_get(vector, int) - returns bytes of a value at the specific index,
*           you probably want to call VECTOR_GET macro instead
*       void vector_reserve(vector, int) - reserves memory for a number of elements
*       void vector_resize(vector, int) - resizes the vector with the number of elements
*       vector vector_alloc() - allocates memory for the vector itself
*       vector vector_new(int) - allocates memory for the vector and accepts size of each element
*       void vector_free() - deallocates memory of the vector
*       void vector_clear() - removes every element in the list but it doesn't change it's capacity
*       int vector_max_size() - returns maximum size of the vector (used internally)
*       int vector_calculate_growth(vector, int) - calculates new capacity based on the new vector size (used internally)
*       int vector_size(vector) - returns the size of the vector
*       int vector_capacity(vector) - returns the capacity of the vector
*       void vector_push_back(vector, void*) - pushs value to the back of the vector
*       void vector_pop_back(vector) - pops value from the back of the vector and returns it
*/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define VECTOR_PANIC(msg) { fprintf(stderr, msg); abort(); }

#ifndef VECTOR_GLOBAL_PREFIX
#define VECTOR_GLOBAL_PREFIX static
#endif

typedef unsigned char byte;

typedef struct __vector
{
    int _size;
    int _capacity;
    int _element_size; // in bytes
    int _last_index;
    byte* _data;
} _vector, *vector;

VECTOR_GLOBAL_PREFIX int VECTOR_MAX_SIZE;

void vector_set(vector vec, int index, void* value);
byte* vector_get(vector vec, int index);

void vector_reserve(vector vec, int new_capacity);
void vector_resize(vector vec, int new_size);

vector vector_alloc();
vector vector_new(int element_size);
void vector_free(vector vec);
void vector_clear(vector vec);

int vector_max_size();
int vector_calculate_growth(vector vec, int new_size);

int vector_size(vector vec);
int vector_capacity(vector vec);

void vector_push_back(vector vec, void* value);
void vector_pop_back(vector vec);

#ifdef VECTOR_IMPL
#undef VECTOR_IMPL

void vector_set(vector vec, int index, void* value)
{
    if (index < 0 || index >= vec->_size)
        VECTOR_PANIC("Vector subscript is out of range!\n")

    if (vec->_last_index < index)
        vec->_last_index = index;

    memcpy(vec->_data + index * vec->_element_size, &value, vec->_element_size);
}

byte* vector_get(vector vec, int index)
{
    if (index < 0 || index >= vec->_size)
        VECTOR_PANIC("Vector subscript is out of range!\n")

    return vec->_data + index * vec->_element_size;
}

#define VECTOR_GET(vec, index, type) *(type*)vector_get(vec, index)

void vector_reserve(vector vec, int new_capacity)
{
    byte* data = vec->_data ?
        (byte*)realloc(vec->_data, new_capacity * vec->_element_size) :
        (byte*)malloc(new_capacity * vec->_element_size);
    
    if (data == NULL)
        VECTOR_PANIC("Couldn't allocate a new memory chunk!\n")
        
    vec->_data = data;
    vec->_capacity = new_capacity;
}

void vector_resize(vector vec, int new_size)
{
    vec->_size = new_size;

    if (new_size > vec->_capacity)
        vec->_capacity = new_size;

    vector_reserve(vec, vec->_capacity);
}

vector vector_alloc()
{
    return (vector)malloc(sizeof(_vector));
}

vector vector_new(int element_size)
{
    vector vec = vector_alloc();

    vec->_element_size = element_size;
    vec->_size = vec->_capacity = 0;
    vec->_last_index = -1;
    vec->_data = NULL;

    if (VECTOR_MAX_SIZE == 0)
        VECTOR_MAX_SIZE = vector_max_size();

    return vec;
}

void vector_free(vector vec)
{
    if (vec->_data)
        free(vec->_data);

    if (vec)
        free(vec);
}

void vector_clear(vector vec)
{
    while (vec->_size > 0)
    {
        vector_set(vec, vec->_size - 1, NULL);
        vec->_size--;
    }

    vec->_last_index = -1;
}

int vector_max_size()
{
    return (int)pow(2.0, (double)(64 - sizeof(int))) - 1;
}

int vector_calculate_growth(vector vec, int new_size)
{
    int old_capacity = vec->_capacity;

    if (old_capacity > VECTOR_MAX_SIZE - old_capacity / 2)
        return VECTOR_MAX_SIZE;

    int geometric = old_capacity + old_capacity / 2;

    if (geometric < new_size)
        return new_size;

    return geometric;
}

int vector_size(vector vec)
{
    return vec->_size;
}

int vector_capacity(vector vec)
{
    return vec->_capacity;
}

void vector_push_back(vector vec, void* value)
{
    if (vec->_last_index == vec->_size - 1)
    {
        if (vec->_size == vec->_capacity)
        {
            int new_capacity = vector_calculate_growth(vec, vec->_size + 1);
            vector_reserve(vec, new_capacity);
        }

        vec->_size++;
        vector_set(vec, ++vec->_last_index, value);
    }
    else
    {
        vec->_size = vec->_last_index + 2;
        vector_set(vec, ++vec->_last_index, value);
    }
}

void vector_pop_back(vector vec)
{
    if (vec->_size == 0)
    {
        fprintf(stderr, "Can't pop on an empty vector!");
        abort();
    }

    *(vec->_data + (vec->_last_index--) * vec->_element_size) = 0;
    vec->_size--;
}

#endif

#endif
