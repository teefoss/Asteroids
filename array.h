#ifndef array_h
#define array_h

#include <stdlib.h>
#include <stdio.h>


template <typename T>
struct Array {
    int capacity;
    int resize_increment;
    int count;
    T * buffer;
    
    Array( int size = 64 );
    ~Array();
    
    Array( const Array& array ) {
        capacity = array.capacity;
        resize_increment = array.resize_increment;
        count = array.count;
        buffer = (T *)malloc( count * sizeof(T) );
        memcpy(buffer, array.buffer, count * sizeof(T));
    }
    
    T * append( T element );
    void insert( T element, int index );
    void remove( int index );
    void clear( void );
};


template <typename T>
Array<T>::Array( int size ) {
    capacity = size;
    resize_increment = size;
    count = 0;
    buffer = (T *)malloc( sizeof(T) * size );
    if ( buffer == NULL ) {
        fprintf( stderr, "%s: malloc failed\n", __func__ );
        exit( EXIT_FAILURE );
    }
}


template <typename T>
Array<T>::~Array() {
    free( buffer );
}


template <typename T>
T * Array<T>::append( T element ) {
    if ( count + 1 > capacity ) {
        size_t new_size = ( capacity + resize_increment ) * sizeof(T);
        buffer = (T *)realloc( buffer, new_size );
        
        if ( buffer == NULL ) {
            fprintf( stderr, "%s: realloc failed\n", __func__ );
            exit( EXIT_FAILURE );
        }
        
        capacity += resize_increment;
    }
    
    buffer[count] = element;
    ++count;
    
    return &buffer[count - 1];
}


template <typename T>
void Array<T>::insert( T element, int index ) {
    //move existing element at index to the end and replace it
    append( buffer[index] );
    buffer[index] = element;
}


template <typename T>
void Array<T>::remove( int index ) {
    buffer[index] = buffer[--count];
}


template <typename T>
void Array<T>::clear() {
    count = 0;
}


#endif /* array_h */
