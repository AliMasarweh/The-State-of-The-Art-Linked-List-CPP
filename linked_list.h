//
// Created by ali-masa on 3/18/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H

#include <cstddef>
#include "../PlatformIndependentConcurrency/mutex.h"

namespace soa {

    template<class T, unsigned int SIZE>
    class Node
    {
    public:
        Node<T, SIZE>(Node<T, SIZE>& toCopy);

        Node<T, SIZE>& operator=(Node<T, SIZE>& toCopy);

        virtual ~Node<T, SIZE>();

        bool empty() const;

        size_t size() const;

        size_t capacity() const;

        const T& operator[](size_t) const;

        T& at(size_t);

    private:
        T m_data[SIZE];
        size_t m_size;
        size_t m_capacity;
        Node<T, SIZE> m_next;
    };

    template<class T, unsigned int SIZE>
    class LinkedList {

        friend bool operator==(LinkedList<T, SIZE>& l1, LinkedList<T, SIZE>& l2);

    public:

        LinkedList<T, SIZE>(size_t capacity);

        LinkedList<T, SIZE>(LinkedList<T, SIZE>& toCopy);

        LinkedList<T, SIZE>& operator=(LinkedList<T, SIZE>& toCopy);

        virtual ~LinkedList<T, SIZE>();

        T& front();

        T& back();

        class Iterator
        {

        };
        using iterator = LinkedList<T,SIZE>::Iterator;
        iterator begin();

        const iterator cbegin() const;

        iterator end();

        const iterator cend() const;

        bool empty() const;

        size_t size() const;

        size_t capacity() const;

        iterator insert( iterator pos, const T& value );

        iterator erase( iterator pos );

        void push_back( const T& value );

        void pop_back();

        void push_front( const T& value );

        void pop_front();

        void resize( size_t count, T value = T() );

    private:
        Node<T, SIZE> m_head;
        // count
        size_t m_size;
        size_t m_capacity;
        Mutex* m_mutex;
    };
}


#endif //THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
